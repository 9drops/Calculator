#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include "exp_func.h"
#include "exp_parser.h"

/*函数列表*/
static FUNC_ITEM m_func_list[] =
{
	/*{字面ID,权值,字面函数名,字面参数个数}*/
	{ID_ADD,		WEIGHT_ADD,  "+",		 2},
	{ID_SUB,		WEIGHT_ADD,  "-",		 2},
	{ID_MUL,		WEIGHT_MUL,  "*",		 2},
	{ID_MOD,		WEIGHT_MUL,  "%",		 2},
	{ID_DIV,		WEIGHT_MUL,  "/",		 2},
};



/*判断"+"或"-"是正负号还是运算符*/
static bool is_sign(const char *src, char *ptr)
{
	bool is_sign = FALSE;	
	char lastchar = (src == ptr ? *src : *(ptr - 1));

	if (src == ptr && ('+' == *ptr || '-' == *ptr))
		is_sign = TRUE;
	else if (('(' == lastchar || '>' == lastchar || '<' == lastchar || '=' == lastchar || ',' == lastchar) 
		&& ('+' == *ptr || '-' == *ptr))
	{
		is_sign = TRUE;
	}
	else
	{
		is_sign = FALSE;
	}
	return is_sign;
}

#define IS_OPERAND(x) \
((x) == ' ' || '(' == (x) || ((x) <= '9' && (x) >= '0') || '#' == (x) || '@' == (x))

#define IS_STROVER(x, n) \
('\'' == (x) && ('+'== (n) || ',' == (n) || ')' == (n) || '\0' == (n) || '"' == (n) || ' ' == (n) || '=' == (n)))

/*
 * Func     : is_operator_start 
 * Desp     : 判断运算符或函数是否开始, “，”作为特殊运算符
 * Input    : ptr - 当前指针
 * Return   : 运算符权值,非运算符函数返回-1,逗号返回0
 *
 */
static int is_operator_start(const char *ptr)
{
	char weight = -1;
	char ch = *ptr;
	char chnext = ('\0' == *ptr ? '\0'   : *(ptr + 1));
	char chnn   = ('\0' == chnext ? '\0' : *(ptr + 2));
	char chnnn  = ('\0' == chnn ? '\0'   : *(ptr + 3));

	/*标记函数或运算符开始*/
	if ('#' == ch)
		weight = WEIGHT_FUNC; 
	else if (',' == ch)
		weight = WEIGHT_DOT; 

	else if	('+' == ch || '-' == ch)
		weight = WEIGHT_ADD;

	else if ('*' == ch || '/' == ch || '%' == ch)
		weight = WEIGHT_MUL;

	else if ('>' == ch || '<' == ch || (('>' == ch || '<' == ch) && '=' == chnext))
		weight = WEIGHT_GT;

	else if ('=' == ch || ('!' == ch && '=' == chnext))
		weight = WEIGHT_EQ;

	else if ('o' == ch && 'r' == chnext && IS_OPERAND(chnn))
		weight = WEIGHT_OR;

	else if ('a' == ch && 'n' == chnext && 'd' == chnn && IS_OPERAND(chnnn))
		weight = WEIGHT_AND;
	else
		weight = -1;

		return weight;
}


/*将形如\n的转义字符串(长度2)为字符'\n'(长度1)*/
static char get_escaped_char(char *p)
{
	if (*p && '\\' == *p)
	{
		switch (*(p + 1))
		{
			case 'n':
				return '\n';
			case 'r':
				return '\r';
			case '\'':
				return '\'';
			case 'a':
				return '\a';
			case 'b':
				return '\b';
			case 't':
				return '\t';
			case '\\':
				return '\\';
			case '0':
				return '\0';
			default:
				break;
		}
	}
	
	return '\0';
}


/*
 * Func     : get_funcitem_by_name
 * Desp     : 通过字面函数名获取函数信息
 * Input    : list - 函数列表
 * Input    : size - 函数列表长度
 * Input    : name - 函数字面名字
 * Return   : 函数信息元素或NULL
 */
static FUNC_ITEM *get_funcitem_by_name(FUNC_ITEM *list, int size, const char *name)
{
	int i = 0;

	for (i = 0; i < size; i++)
	{
		if (0 == strcasecmp(list[i].name, name))
			return &list[i];
	}
	
	return NULL;
}

static int vartable_stats(VAR_ITEM *table, int size, int *gvarnum, int *gfailure, int *lvarnum, int *lfailure)
{
	int i   = 0;
	int gvn = 0;
	int gfn = 0;
	int lvn = 0;
	int lfn = 0;

	for (i = 0; i < size; i++)
	{
		if (table[i].exist)
		{
			if (table[i].islocal)
			{
				lvn++;
				if (table[i].isnull)
					lfn++;
			}
			else
			{
				gvn++;
				if (table[i].isnull)
					gfn++;
			}
		}
	}
	
	*gvarnum  = gvn;
	*gfailure = gfn;
	*lvarnum  = lvn;
	*lfailure = lfn;

	return 0;
}

/*
* Func   : exp_get_stype
* Desp   : 根据表达式变量类型返回数据模型类型串
* Input  : itype - 结构数据类型
* Return : 非空 - 成功, NULL - 出错
*          特别的,形如@a.b的变量
*          如果数据区中无@a.b或是空节点（有节点名字无值）
*          返回NULL
*/
char *exp_get_stype(TYPE itype)
{
	switch (itype)
	{
		case Bool:
			return M_BOOL;
		case CHAR:
			return M_BYTE;
		case INT:
			return M_INT;
		case LONG:
			return M_LONG;
		case FLOAT:
			return M_FLOAT;
		case DOUBLE:
			return M_DOUBLE;
		case STRING:
			return M_STRING;
		case T_DATA:
			return M_DATA; 
		default:
			return NULL;
	}

	return NULL;
}


/*
 * Func     : func_operate
 * Desp     : 执行具体函数
 * Input    : func   - 字面函数名字, 形如:"+","-", "*"
 * Input    : params - 参数值列表
 * Input    : num - 参数个数
 * Output   : result - 结果字符串
 * Output   : type   - 函数的返回类型
 * Return   : 0 - 成功, 非0 - 失败
 */
static int func_operate(char *func, PARAM_ITEM *params, int num, char *result, TYPE *type, int len,
                        VAR_ITEM *table, int table_size)
{
	int ret = 0;
	int size = 0;
	TYPE final_type = 0;/*类型转换后最终的类型*/
	char suffix[8];
	char final_func_name[EXP_MAX_NAME_LEN];
	size = sizeof(suffix);
	memset(final_func_name, 0, EXP_MAX_NAME_LEN);
	memset(suffix, 0, size);
	memset(result, 0, len);

	DPRINT("(%s.%4d) p0:%s p1:%s p2:%s p3:%s\n", __FILE__, __LINE__, 
		params[0].value, params[1].value, params[2].value, params[3].value);

	/*参数数值类型转换,形如1+3.0则调用add_f函数*/
	final_type = (params[0].type > params[1].type) ? params[0].type : params[1].type;

	/*根据字面函数名字和参数类型重载到具体函数名字,形如:"add_i","add_f","add_s"*/
	/*加运算符重载到具体函数*/
	if ('+' == *func)
	{
		if (INT == final_type)
		{
			snprintf(result, len, "%d", add_i(atoi(params[0].value), atoi(params[1].value))); 
			*type = INT;
		}
		else if (LONG == final_type)
		{
			snprintf(result, len, "%ld", add_l(atol(params[0].value), atol(params[1].value))); 
			*type = LONG;
		}
		else if (STRING == final_type)
		{
			if (len <= strlen(params[0].value) +  strlen(params[1].value))
			{
				ret = ERR_EXP_BUFFER_SIZE;
				goto END;
			}
			add_s(params[0].value, params[1].value, result, len); 
			*type = STRING;

		}
		else if (DOUBLE == final_type)
		{
			tos_f(result, size, add_d(atof(params[0].value), atof(params[1].value)), MAX_DOT_PREC);
			*type = DOUBLE;
		}
		else
		{
			ret = ERR_EXP_PARAM;
		}

		goto END;
	}
	/*减运算符重载到具体函数*/
	else if ('-' == *func)
	{
		if (INT == final_type)
		{
			snprintf(result, len, "%d", sub_i(atoi(params[0].value), atoi(params[1].value))); 
			*type = INT;
		}
		else if (LONG == final_type)
		{
			snprintf(result, len, "%ld", sub_l(atol(params[0].value), atol(params[1].value))); 
			*type = LONG;
		}
		else if (DOUBLE == final_type)
		{
			tos_f(result, size, sub_d(atof(params[0].value), atof(params[1].value)), MAX_DOT_PREC);
			*type = DOUBLE;
		}
		else
		{
			ret = ERR_EXP_PARAM;
		}

		goto END;
	}
	/*乘运算符重载到具体函数*/
	else if ('*' == *func)
	{
		if (INT == final_type)
		{
			snprintf(result, len, "%lld", mul_i(atoi(params[0].value), atoi(params[1].value)));
			*type = LONG;
		}
		else if (LONG == final_type)
		{
			snprintf(result, len, "%ld", mul_l(atol(params[0].value), atol(params[1].value))); 
			*type = LONG;
		}
		else if (DOUBLE == final_type)
		{
			tos_f(result, size, mul_d(atof(params[0].value), atof(params[1].value)), MAX_DOT_PREC);
			*type = DOUBLE;
		}
		else
		{
			ret = ERR_EXP_PARAM;
		}

		goto END;
	}
	/*除运算符重载到具体函数*/
	else if ('/' == *func)
	{
		if ('0' == *(params[1].value) && 1 == strlen(params[1].value))
		{
			ret = ERR_EXP_DIVISOR; 
		}
		else if (INT == final_type)
		{
			snprintf(result, len, "%d", div_i(atoi(params[0].value), atoi(params[1].value))); 
			*type = INT;
		}
		else if (LONG == final_type)
		{
			snprintf(result, len, "%ld", div_l(atol(params[0].value), atol(params[1].value))); 
			*type = LONG;
		}
		else if (DOUBLE == final_type)
		{
            snprintf(result, size, "%f", div_d(atof(params[0].value), atof(params[1].value)));
			*type = DOUBLE;
		}
		else
		{
			ret = ERR_EXP_PARAM;
		}

		goto END;
	}
	/*取余运算符*/
	else if ('%' == *func)
	{
		if ('0' == *(params[1].value) && 1 == strlen(params[1].value))
		{
			ret = ERR_EXP_DIVISOR; 
		}
		else if (INT == final_type)
		{
			snprintf(result, len, "%d", mod(atoi(params[0].value), atoi(params[1].value))); 
			*type = INT;
		}
		else if (LONG == final_type)
		{
			snprintf(result, len, "%ld", mod_l(atol(params[0].value), atol(params[1].value))); 
			*type = LONG;
		}
		else
		{
			ret = ERR_EXP_PARAM;
		}
		goto END;

	}
		
END:
	return ret;
}


/*获取栈中最靠近顶部的函数的权值*/
static int get_dot_weight(FUNC_ITEM func_stack[], BRCKT_ITEM brckt_stack[], int bs_top)
{
	int fs_top;

	if (NULL == func_stack || bs_top < 0)
	{
		return WEIGHT_DOT;
	}

	fs_top = brckt_stack[bs_top].fs_top;
	if (fs_top >= 0)
		return func_stack[fs_top].weight; 

	return WEIGHT_DOT;
}

/*
* Func    : exp_parser
* Desp    : 表达式解析
* Input   : exp - 表达式串,形如“#substring('123456', 0, 3)”
* Output  : result - 运算结果
* Output  : res_type - 运算结果数据类型
* Input   : size - result的字节数
* Return  : 0 - 成功, 非0失败
*           特别的，
*           当解析形如"@a.b + @a.c + 'hello'"的表达式,
*           如果数据区中无@a.b和@a.c 或 他们都是空节点(只有节点名字无值), 
*           表达式返回0且结果类型出参res_type为0
*/
int exp_parser(char *exp, char *result, TYPE *res_type, int size)
{
	int ret            = 0;
	int i              = 0;
	int len            = 0;
	TYPE result_type   = 0;
	TYPE param_type    = 0;
	int param_num      = 0;
	int func_name_len  = 0;
	int init_weight    = 0;
	int weight         = 0;
	int weight_tmp     = 0;
	int param_len      = 0;/*参数长度*/
	int parambuf_index = 0;
	int fs_top         = -1;/*函数栈顶*/
	int ps_top         = -1;/*参数栈顶*/
	int bs_top         = -1;/*括号栈顶*/
	int gvarnum        = 0;
	int gfailure       = 0; 
	int lvarnum        = 0;
	int lfailure       = 0;
	int isnull_flag    = 0;
	char is_double = 0;/*是否浮点数*/
	char ch;
	char sign = 0;/*正负号*/
	char *p = NULL;
	char *ptr = NULL;
	//char exp_tmp[EXP_MAX_LEN];
	char func_name[EXP_MAX_NAME_LEN];
	char param_buf[EXP_MAX_BUF_SIZE];
	/*运算或函数符栈*/
	FUNC_ITEM func_stack[EXP_MAX_STACK_LEN];
	/*参数栈*/
	PARAM_ITEM param_stack[EXP_MAX_STACK_LEN];
	/*括号栈*/
	BRCKT_ITEM brckt_stack[EXP_MAX_STACK_LEN];
	/*变量表*/
	int table_size = EXP_MAX_STACK_LEN;
	VAR_ITEM var_table[EXP_MAX_STACK_LEN];

	if (NULL == exp || NULL ==result || 0 == size)
	{
		ret = ERR_EXP_PARAM;
		goto END; 
	}
	else if (0 == strlen(exp))
	{
		ret = ERR_OK;
		goto END;
	}

	/*初始化各栈*/
	memset(func_stack,  0, sizeof(func_stack));
	memset(param_stack, 0, sizeof(param_stack));
	memset(brckt_stack, 0, sizeof(brckt_stack));
	memset(param_buf,   0, sizeof(param_buf));
	memset(var_table,   0, sizeof(var_table));
#if 0
	memset(exp_tmp, 0, EXP_MAX_LEN);

	if (strtrim(exp, exp_tmp, EXP_MAX_LEN))
	{
		ret = ERR_EXP_MEMORY; 
		return ret;
	}

	exp_tmp = exp;
	ptr = exp_tmp;
	len = strlen(exp_tmp);
#endif
	ptr = exp;
	len = (int)strlen(exp);

	/*遍历表达式*/
	for (i = 0; i <= len; i++)
	{
		p = ptr + i;
		/*当前字符*/
		ch = *p;

		/*获取数字,形如：123,此处没有考虑负数*/
		if (('0' <= ch && '9' >= ch) || '.' == ch)
		{
			param_len = 0;
			
			/*如果为负数给数字补上符号*/
			if ('-' == sign)
			{
				*param_buf = '-';
				parambuf_index = 1;
				sign = 0;
			}
			else 
			{
				parambuf_index = 0;
			}
			
			while (*p)
			{
				/*遇到运算符、函数或括号数字串结束*/
				if (is_operator_start(p) >= 0 || ')' == *p)
					break;
				if ('.' == *p)
					is_double = 1;
				param_len++;
				param_buf[parambuf_index++] = *p;
				p++;
			}

			param_type = INT;

			if (is_double)
				param_type = DOUBLE;

			/*参数入栈*/
			strncpy(param_stack[++ps_top].value, param_buf, EXP_MAX_BUF_SIZE - 1); 
			param_stack[ps_top].type = param_type;/*参数的类型*/
			memset(param_buf, 0, sizeof(param_buf));
			
			/*跳过数字串*/
			i += param_len - 1;
		}/*end of if ('0' <= ch && '9' >= ch)*/
		
		/*获取字符串常量,形如:'abc'*/
		else if ('\'' == ch)
		{
			char escape = '\0';
			param_len = 0;
			parambuf_index = 0;
			while (*p++)
			{
				/*处理转义字符*/
				if ((escape = get_escaped_char(p)))
				{
					param_buf[parambuf_index++] = escape;
					param_len += 2;
					p++;
					continue;
				}
				else if (IS_STROVER(*p, *(p + 1)))
					break;
					
				param_len++;
				param_buf[parambuf_index++] = *p;
			}

			/*参数入栈*/
			strncpy(param_stack[++ps_top].value, param_buf, EXP_MAX_BUF_SIZE - 1); 
			param_stack[ps_top].type = STRING;/*参数的类型*/
			memset(param_buf, 0, sizeof(param_buf));
			/*跳过字符串常量*/
			i += param_len + 1;

		}/*end of else if (''' == ch)*/

        /*标记函数或运算符开始*/
		else if ((weight_tmp = is_operator_start(p)) >= 0)
		{
			FUNC_ITEM *pitem;
			FUNC_ITEM tmp_operator;
			/*清空函数名字*/
			memset(func_name, 0, sizeof(func_name));
			memset(&tmp_operator, 0, sizeof(tmp_operator));
			func_name_len = 0;
			
			/*获取自定义函数名字*/
			if ('#' == ch)
			{
				while (*p++)
				{
					/*遇到运算符或函数变量串结束*/
					if ('(' == *p)
					{
						break;
					}

					func_name[func_name_len++] = *p;
				}
			
				/*跳过变量串*/
				i += func_name_len;
				/*设置函数权值*/
				init_weight = WEIGHT_FUNC;
			}/*end of if ('#' == ch)*/

			/*获取运算符名字*/
			else
			{
				/*and*/
				if ('a' == ch)
				{
					strncpy(func_name, "and", EXP_MAX_NAME_LEN - 1);
					/*设置函数权值*/
					init_weight = WEIGHT_AND;
					/*跳过"nd"*/
					i += strlen("and") - 1;
				}
				/*or*/
				else if ('o' == ch)
				{
					strncpy(func_name, "or", EXP_MAX_NAME_LEN - 1);
					/*设置函数权值*/
					init_weight = WEIGHT_OR;
					/*跳过"o"后的"r"*/
					i += 1;
				}
				/*>*/
				else if ('>' == ch && '=' != *(p + 1))
				{
					*func_name = '>';
					/*设置函数权值*/
					init_weight = WEIGHT_GT;
				}
				/*<*/
				else if ('<' == ch && '=' != *(p + 1))
				{
					*func_name = '<';
					/*设置函数权值*/
					init_weight = WEIGHT_GT;
				}
				/*>=*/
				else if ('>' == ch && '=' == *(p + 1))
				{
					strncpy(func_name, ">=", EXP_MAX_NAME_LEN - 1);
					/*设置函数权值*/
					init_weight = WEIGHT_GT;
					/*跳过">"后的"="*/
					i += 1;
				}
				/*<=*/
				else if ('<' == ch && '=' == *(p + 1))
				{
					strncpy(func_name, "<=", EXP_MAX_NAME_LEN - 1);
					/*设置函数权值*/
					init_weight = WEIGHT_GT;
					/*跳过"<"后的"="*/
					i += 1;
				}
				/*!=*/
				else if ('!' == ch && '=' == *(p + 1))
				{
					strncpy(func_name, "!=", EXP_MAX_NAME_LEN - 1);
					/*设置函数权值*/
					init_weight = WEIGHT_EQ;
					/*跳过"!"后的"="*/
					i += 1;
				}
				/*=*/
				else if ('=' == ch)
				{
					*func_name = '=';
					/*设置函数权值*/
					init_weight = WEIGHT_EQ;
				}

				/*单字符运算符，形如：+,-等*/
				else if ('+' == ch || '-' == ch) 
				{
					/*正负号*/
					if (is_sign(/*exp_tmp*/exp, p))
					{
						DPRINT("(%s.%4d) sign\n", __FILE__, __LINE__);
						sign = ch;
					}
					/*运算符*/
					else
					{
						func_name[0] = ch;
						init_weight = WEIGHT_ADD;
					}
				}

				/*单字符运算符，形如：*,/ 等*/
				else if ('*' == ch || '/' == ch || '%' == ch) 
				{
					func_name[0] = ch;
					init_weight = WEIGHT_MUL;
				}
				/*逗号运算符初始优先级init_weight等于上一个运算符优先级*/
				else if (',' == ch)
				{
					func_name[0] = ch;
					init_weight = func_stack[fs_top].weight;
				}
				/*默认情况*/
				else
				{
					/*do nothing*/
				}
				
			}/*end of else*/
			
			/*扫描的关键字非运算符或函数*/
			if (strlen(func_name) <= 0)
			{
				continue;
			}

			/*将运算符转化为FUNC_ITEM类型*/
			/*","前为")"其优先级为函数栈栈顶函数优先级*/
			if (',' == *func_name)
			{
				tmp_operator.weight = 1 + get_dot_weight(func_stack, brckt_stack, bs_top);
				DPRINT("(%s.%4d) dot weight:%d\n", __FILE__, __LINE__, tmp_operator.weight);
			}
			else
				tmp_operator.weight = init_weight + (weight << 2);/*权值*/

			if (0 == strcasecmp(func_name, "isnull"))
			{
				isnull_flag = 1;
			}

			strncpy(tmp_operator.name, func_name, EXP_MAX_NAME_LEN - 1);/*函数字面名字*/	
			/*参数个数*/
			if (',' != *func_name)
			{
				DPRINT("(%s.%4d) func name:%s\n", __FILE__, __LINE__, func_name);
				pitem = get_funcitem_by_name(m_func_list, sizeof(m_func_list) / sizeof(FUNC_ITEM), func_name);
				if (pitem)
				{
					DPRINT("(%s.%4d) pitem.param_num:%d\n", __FILE__, __LINE__, pitem->param_num);
					tmp_operator.param_num = pitem->param_num;
				}
				else
				{
					DPRINT("(%s.%4d) No the function\n", __FILE__, __LINE__);
					ret = ERR_EXP_NO_FUNC;
					goto END;
				}

				DPRINT("(%s.%4d) TMP OP, func:%s weight:%d\n", __FILE__, __LINE__, tmp_operator.name, tmp_operator.weight);
			}

			
			/*如果tmp_operator的权值小于或等于函数栈顶元素将函数栈顶元素出栈并执行，结果入参数栈*/
			while (fs_top >= 0 && tmp_operator.weight <= func_stack[fs_top].weight)
			{
				/*清空result*/
				memset(result, 0, size);
				/*获取参数的个数*/
				param_num = func_stack[fs_top].param_num;
				/*调用栈顶函数*/
				ret = func_operate(func_stack[fs_top].name, &param_stack[ps_top - param_num + 1], 
				                   param_num, result, &result_type, size, var_table, table_size);
				if (ret)
				{
					goto END;
				}

				/*清空出栈元素*/
				memset(&param_stack[ps_top - param_num + 1], 0, param_num * sizeof(PARAM_ITEM));
				/*调整参数栈顶*/
				ps_top -= param_num;
				fs_top--;
				/*结果入参数栈*/
				param_stack[++ps_top].type = result_type;
				strncpy(param_stack[ps_top].value, result, EXP_MAX_BUF_SIZE - 1);
			}/*end of while (tmp_operator.weight <= func_stack[fs_top].weight)*/
			
			/*如果运算符不为“,”,将tmp_operator入函数栈*/
			if (ch != ',')
			{
				memcpy(&func_stack[++fs_top], &tmp_operator, sizeof(FUNC_ITEM));
			}

			continue;
		}/*end of else if ((weight_tmp = is_operator_start(p)) >= 0)*/

		/*左括号权值加1*/
		else if ('(' == ch)
		{
			++bs_top;
			brckt_stack[bs_top].fs_top = fs_top;
			brckt_stack[bs_top].is_lbrckt = 1;
			weight += 2;
			continue;
		}

		/*右括号权值减1*/
		else if (')' == ch)
		{
			if (brckt_stack[bs_top].is_lbrckt)
			{
				brckt_stack[bs_top].fs_top    = 0;
				brckt_stack[bs_top].is_lbrckt = 0;
				bs_top--;
			}
			else
			{
				ret = ERR_EXP_BRACKET;
				goto END;
			}
			weight -= 2;
			continue;
		}
		/*忽略掉空格等无用字符*/
		else
		{
			continue;
		}
		
	} /*end of for (i = 0; i < len; i++)*/

	/*调用函数栈中剩余函数*/
	while (fs_top != -1)
	{
		/*清空result*/
		memset(result, 0, size);
		/*获取参数的个数*/
		param_num = func_stack[fs_top].param_num;
				
		/*调用栈顶函数*/
		ret = func_operate(func_stack[fs_top].name, &param_stack[ps_top - param_num + 1], param_num, 
		                   result, &result_type, size, var_table, table_size);
		
		if (ret)
		{
			goto END;
		}

		/*清空出栈元素*/
		memset(&param_stack[ps_top - param_num + 1], 0, param_num * sizeof(PARAM_ITEM));
		/*调整参数栈顶*/
		ps_top -= param_num;
		fs_top--;
		/*结果入参数栈*/
		param_stack[++ps_top].type = result_type;
		strncpy(param_stack[ps_top].value, result, EXP_MAX_BUF_SIZE - 1);
	}

	
	/*参数栈非0则失败*/
	if (ps_top)
	{
		ret = ERR_EXP_PARAM;
		goto END;
	}

	if (bs_top != -1)
	{
		ret = ERR_EXP_BRACKET;
		goto END;
	}

	strncpy(result, param_stack[0].value, size - 1);

	if (res_type)
		*res_type = param_stack[0].type;
	DPRINT("(%s.%4d) Result:%s\n", __FILE__, __LINE__, param_stack[0].value);
END:
	/*失败则结果为空*/
	if (ret)
	{
		if (result)
			memset(result, 0, size);

		if (res_type)
			*res_type = INVALID;
	}

	/*如果获取所有局部变量都失败则认为该记录空*/
	vartable_stats(var_table, table_size, &gvarnum, &gfailure, &lvarnum, &lfailure);
	DPRINT("(%s.%4d) gvarnum:%d, gfailure:%d, lvarnum:%d, lfailure:%d, isnull_flag:%d\n", 
		__FILE__, __LINE__, gvarnum, gfailure, lvarnum, lfailure, isnull_flag);

	/*变量数大于1且所有变量都不存在*/
	if (lvarnum + gvarnum > 0 && gvarnum == gfailure && lvarnum == lfailure && !isnull_flag)
	{
		memset(result, 0, size);
		*res_type = DUMY;
		ret = ERR_EXP_GET_ALL_VAR;
	}
	else if (lvarnum + gvarnum > 1 && gfailure + lfailure > 0 && !isnull_flag)
	{
		memset(result, 0, size);
		*res_type = DUMY;
		ret = ERR_EXP_GET_ONE_VAR;
	}

	DPRINT("(%s.%4d) exp:%s, ret:%d result:%s type:%d\n", __FILE__, __LINE__, exp, ret, result, *res_type);
	return ret;
}

/*去掉数字字符串中多余的0，例如1.3000处理为1.3*/
char *exp_trim_zero(char *result) {
    char *dot = strchr(result, '.');
    char *p = dot;
    if (dot) {
        while (*p++) {
            if (*(p + 1) == '\0') {
                break;
            }
        }
        
        while (p!= dot) {
            if (*p != '0') {
                *(p + 1) = '\0';
                break;
            }
            
            p--;
        }
        
        if (p == dot) {
            *p = '\0';
        }
    }
    
    return result;
}

#if 0
int main(int argc, char **argv)
{
	int ret = 0;
	TYPE res_type = -1;
	FILE *fp = fopen("exp.test", "r");
	if (NULL == fp)
	{
		printf("open exp.test falied!\n");
		printf("if exp.test not exist, please create it, then fill one expression in one line.\n");
		return 0;
	}
	
	char exp[128] = {0};
	char result[1024]= {0};
	int exp_size = sizeof(exp);
	int res_size = sizeof(result);
	
	while (fgets(exp, sizeof(exp), fp))
	{
		*(exp + strlen(exp) - 1) = '\0';
		printf("exp:%s\n", exp);
		ret = exp_parser(exp, result, &res_type, res_size);
		printf("ret:%d, result:'%s' type:%d stype:%s\n\n", ret, result, res_type, exp_get_stype(res_type));
		memset(exp, 0, exp_size);
		memset(result, 0, res_size);
	}

	fclose(fp);
	return 0;
}
#endif
