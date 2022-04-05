#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#define __USE_XOPEN
#include <time.h>
#include <cstring>
#include <string>
#include <regex.h>
#include <sys/types.h>
#include "exp_func.h"

using namespace std;

static char *format[] ={"%Y-%m-%d %T","%Y/%m/%d %T","%Y.%m.%d %T","%d %m,%Y %T","%Y%m%d%H%M%S", "%Y-%m-%d", "%H:%M:%S"};

#ifdef TEST
typedef struct
{
	char name[32];
	int value;
}VAR;

static VAR m_var_list[] = 
{
	{"var1", 11},
	{"var2", 21},
	{"var3", 31},
	{"var4", 41},
	{"var5", 51},
};
#endif

//////////////////////
//公共函数
//
///////// begin ////////////
int add_i(int a, int b)
{
	return a + b;
}

long add_l(long a, long b)
{
	return a + b;
}

double add_d(double a, double b)
{
	return a + b;
}

int add_s(char *a, char *b, char *res, int len)
{
	memset(res, 0, len);
	snprintf(res, len, "%s%s", a, b);
	return 0;
}

int sub_i(int a, int b)
{
	return a - b;
}

long sub_l(long a, long b)
{
	return a - b;
}

double sub_d(double a, double b)
{
	return a - b;
}

int64_t mul_i(int a, int b)
{
	return a * b;
}

long mul_l(long a, long b)
{
	return a * b;
}

double mul_d(double a, double b)
{
	return a * b;
}

int div_i(int a, int b)
{
	return (b != 0 ? a / b : -1);
}

long div_l(long a, long b)
{
	return (b != 0 ? a / b : -1);
}

double div_d(double a, double b)
{
	return (b != 0 ? a / b : -1);
}

int mod(int a, int b)
{
	return (0 == b ? -1 : a % b);
}

long mod_l(long a, long b)
{
	return (0 == b ? -1 : a % b);
}

int or2(int a, int b)
{
	return a || b;
}

int and2(int a, int b)
{
	return a && b;
}

int eq_i(int a, int b)
{
	return (a == b);
}

int eq_l(long a, long b)
{
	return (a == b);
}

int eq_d(double a, double b)
{
	return (a - b <= MIN_DOUBLE_INTERAL && a - b >= -MIN_DOUBLE_INTERAL) ;
}

int eq_str(const char *a, const char *b, int size)
{
	return !strncmp(a, b, size - 1);
}

int ne_i(int a, int b)
{
	return !eq_i(a, b);
}

int ne_l(long a, long b)
{
	return !eq_l(a, b);
}

int ne_d(double a, double b)
{
	return !(eq_d(a, b));
}

int gt_i(int a, int b)
{
	return (a > b);
}

int gt_l(long a, long b)
{
	return (a > b);
}

int gt_d(double a, double b)
{
	return (a > b);
}

int gt_str(const char *a, const char *b, int size)
{
	return (strncmp(a, b, size - 1) > 0 ? 1 : 0);
}

int ge_i(int a, int b)
{
	return (a >= b);
}

int ge_l(long a, long b)
{
	return (a >= b);
}

int ge_d(double a, double b)
{
	return (a >= b);
}

int ge_str(const char *a, const char *b, int size)
{
	return (strncmp(a, b, size - 1) >= 0 ? 1 : 0);
}

int lt_i(int a, int b)
{
	return (a < b);
}

int lt_l(long a, long b)
{
	return (a < b);
}

int lt_d(double a, double b)
{
	return (a < b);
}

int lt_str(const char *a, const char *b, int size)
{
	return (strncmp(a, b, size - 1) < 0 ? 1 : 0);
}

int le_i(int a, int b)
{
	return (a <= b);
}

int le_l(long a, long b)
{
	return (a <= b);
}

int le_d(double a, double b)
{
	return (a <= b);
}

int le_str(const char *a, const char *b, int size)
{
	return (strncmp(a, b, size - 1) <= 0 ? 1 : 0);
}


/*
 *数字转字符串函数
 *
 *@param num 被转换的数字
 *return str 转换成功的字符串
 */
char *itoa(long num, char str[])
{
    return str;
}


/*
 *去除字符串中的空白字符
 *
 *@param str 源字符串
 *return 去除空白字符后的字符串
 */
//char*　trim(char* str)
char *trim(char *src, char *dest)
{
	if(src == NULL)
		return NULL;
	char *p = src;
	while(*p){
		if(*p != ' '){
			*dest++ = *p;	
		}
		++p;
	}
	*dest = '\0';
	
	return dest;
}

///////// end ///////////////

/**
 * 将string转换为数字
 *
 * @param val 被转换的字符串
 * @return long 转换成的数字
 */
double tof_s(const char *var)
{
	return strtod(var,NULL);
}


int tof_i(int var)
{
	return var;
}
/**
 * 将float变量转换为字符串
 *
 * @param num float变量
 * @return float变量
 */
float tof_f(float var)
{
	return var;
}

/**
 * 将bool变量转换为字符串
 *
 * @param num bool数
 * @return 字符串
 */
char *tos_b(bool num)
{
	if(num)
		return "true";
	else
		return "false";
	return NULL;
}

/*字符串精度处理函数*/
char *fstrprec(char *result, int size, int prec)
{
	char *p = result;
	while (*p)
	{
		if ('.' == *p)
			break;
		p++;
	}

	/*小数*/
	if (*p)
	{
		if (0 == prec)
		{
			*p = '\0';
			return result;
		}

		while (prec--)
		{
			p++;

			if ('\0' == *p)
				*p = '0';
		}
			
		*(++p) = '\0';
	}
	/*整数*/
	else
	{
		if (prec > 0)
			*p++ = '.';

		while (prec--)
		{
			*p++ = '0';
		}
	}
	return result;
}

/**
 * 将数字转换为字符串支持给定精度转换。
 *
 * @param result 字符串存储空间
 * @param num 被转换的的数字
 * @param prec 转换精度
 * @return 字符串
 */
char *tos_f(char *result, int size, double num,int prec)
{
	if (prec > MAX_DOUBLE_PREC || prec < 0)
		prec = 0;

	gcvt(num, size, result);
	return fstrprec(result, size, prec);
}



/**
 * 从str中获取子字符串
 *
 * @param str 可以是字符串和变量
 * @param offset 为截取字符串的开始索引，类型为变量或数字。
 * @param count 是截取字符串长度，若小于0为全部，类型可以是变量或数字。
 * @return 字符串
 */
char *substring(char* str, char* result,int offset, int count)
{
	size_t len = 0;
	//if (NULL == str || strlen(str) <= 0) return NULL;

	len = strlen(str);
	if (offset < 0 || offset >= len) offset = 0;
	if (offset + count > len) count = len - offset;

	strncpy(result, str + offset, count);	
	return result;
}



/**
 * 返回TRUE数。
 *
 * @param 无
 * @param 
 * @return TRUE
 */
bool true2()
{
	return TRUE;
}

/**
 * 返回FALSE数。
 *
 * @param 无
 * @param 
 * @return FALSE
 */
bool false2()
{
	return FALSE;
}

/**
 * 取反函数。
 *
 * @param  flag 
 * @return TRUE or FALSE
 */
bool not2(bool flag)
{
	return !flag;
}


/**
 * 计算参数字符串的长度。
 *
 * @param str目标字符串
 * @return 参数字符串的长度
 */
int  length(const char* str)
{
	return strlen(str);
}

/**
 * 该函数返回当前的时间值
 *
 * @param 无
 * @param 无
 * @return 当前时间值
 */
long now()
{
	time_t tp;
	return time(&tp); 
}

/*
* 将时间值格式化为字符串函数
*
* @param time_n为转换时间值
* @param fmt是日期转换格式，默认为”yyyy-mm-dd HH:MM:ss” 可
* 选格式“yyyy/mm/dd hh:MM:ss”“yyyymmmdd hhMMss”；“yyy.mm.dd hh:MM:ss”;“dd mm
* ,yyyy hh:MM:ss”;"yyyymmddhhMMss"
* @param s_time 时间存储空间
* @param len 空间大小
* @return 时间字符串
*/
char *dtprintf(long time_n,char *fmt, int size, char s_time[],int len)
{
	int fmt_i;
	struct tm tnow; 
	localtime_r((const time_t*)&time_n, &tnow);

	if(fmt == NULL)
		fmt_i = 0;
	else if(0 == strncmp(fmt,"yyyy-mm-dd HH:MM:ss", size))
		fmt_i = 0;
	else if(0 == strncmp(fmt,"yyyy/mm/dd hh:MM:ss", size))
		fmt_i = 1;
	else if(0 == strncmp(fmt,"yyy.mm.dd hh:MM:ss", size))
		fmt_i = 2;
	else if(0 == strncmp(fmt,"dd mm,yyyy hh:MM:ss", size))
		fmt_i = 3;
	else if(0 == strncmp(fmt,"yyyymmddhhMMss", size))
		fmt_i = 4;
	else if(0 == strncmp(fmt,"yyyy-mm-dd", size))
		fmt_i = 5;
	else if(0 == strncmp(fmt,"%H:%M:%S", size))
		fmt_i = 6;
	else
		fmt_i = 0;

	if (0 == time_n) time_n = time(NULL);

	strftime(s_time,len,format[fmt_i], &tnow);
	return s_time;
}

/**
 * 返回日期的年份函数,该函数实现从给定时间中获取年
 * 如果参数为0，返回当前年。
 *
 * @param time_n 
 * @param 
 * @return  年份
 */
int year(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);
	struct tm tnow;
	localtime_r((const time_t*)&time_n, &tnow);
	return 1900 + tnow.tm_year;
}

/**
 * 返回日期的月份函数。该函数实现从给定时间中获取月份
 * 如果参数为0，返回当前月份。
 *
 * @param time_n
 * @param 
 * @return 月份
 */
int month(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);
	struct tm tnow;
	localtime_r((const time_t*)&time_n, &tnow);
	return tnow.tm_mon + 1;
}

/**
 * 返回日期的天函数。该函数实现从给定时间中获取日
 * 如果参数为0，返回当前天。
 *
 * @param time_n
 * @param 
 * @return 天数
 */
int day(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);
	struct tm tnow;
	localtime_r( (const time_t*)&time_n, &tnow);
	return tnow.tm_mday;
}

/**
 * 返回日期的小时函数。该函数实现从给定时间中获取小时
 * 如果参数为0，返回当前小时。
 *
 * @param time_n
 * @param 
 * @return 小时
 */
int hour(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);
	struct tm tnow;
	localtime_r((const time_t*)&time_n, &tnow);
	return tnow.tm_hour;
}

/**
 * 返回日期的分钟函数。该函数实现从给定时间中获取分钟
 * 如果参数为0，返回当前分钟。
 *
 * @param time_n
 * @param 
 * @return 分钟
 */
int minute(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);
	struct tm tnow;
	localtime_r((const time_t*)&time_n, &tnow);
	return tnow.tm_min;
}

/**
 * 返回日期的秒函数。该函数实现从给定时间中获取秒钟
 * 如果参数为0，返回当前秒。
 *
 * @param time_n
 * @param 
 * @return 秒数
 */
int second(long time_n)
{
	if (0 == time_n)
		time_n = time(NULL);

	struct tm tnow;
	localtime_r((const time_t*)&time_n, &tnow);
	return tnow.tm_sec;
}

/**
 * 替换字符串中符合条件的字符串为指定的内容
 *	该函数实现了将字符串str中出现rule的地方全部替换为rep的值
 * @param src 源字符串
 * @param rule 规则
 * @param rep 替换值
 * @return 
 */
char *replace(char *src, char *rule, char *rep, bool isregex, char *dst, int size)
{
	int ret = 0;
	int n = 0;
	int i = 0;
	int rlen = 0;
	int slen = 0;
	regmatch_t mt[MAX_MATCHED_NUM];
	regmatch_t pmatch;
	char *p = src;

	if (NULL == src)
	{
		return NULL;
	}

	if (NULL == rule || NULL == rep)
		return src;
		
	memset(&mt, 0, sizeof(mt));
	memset(dst, 0, size);
	rlen = strlen(rule);
	
	if (isregex)
	{
		char ebuf[64] = {0};
		regex_t reg;
		i = 0;
		int offset = 0;
		regcomp(&reg, rule, REG_EXTENDED);
		while ((0 == (ret = regexec(&reg, p, 1, &pmatch, 0))) && i < MAX_MATCHED_NUM)
		{
			offset = p - src;
			mt[i].rm_so = pmatch.rm_so + offset;
			mt[i].rm_eo = pmatch.rm_eo + offset;
			p += pmatch.rm_eo;
			i++;
		}

		regerror(ret, &reg, ebuf, sizeof(ebuf));
		regfree(&reg);
	}
	else
	{
		while (p && i < MAX_MATCHED_NUM)
		{
			p = strstr(p, rule);
			if (NULL == p)
				break;

			mt[i].rm_so = p - src;
			mt[i].rm_eo = p - src + rlen;
			p = p + rlen;
			i++;
		}

	}
	
	p = src;
	n = i;
	for (i = 0; i < n; i++)
	{
		if (0 == i)
		{
			if (mt[i].rm_so >= size)
				return NULL;
			strncat(dst, src, mt[i].rm_so);	
			slen = strlen(dst);
			strncat(dst, rep, size - slen - 1);
		}
		else
		{
			strncat(dst, src + mt[i - 1].rm_eo, mt[i].rm_so - mt[i - 1].rm_eo);	
			slen = strlen(dst);
			strncat(dst, rep, size - slen - 1);

			if (i == n - 1)
				strncat(dst, src + mt[i].rm_eo, size - slen - 1);	
		}

	}

	return dst;
}



/**
 * 将日期字符串按照给定的格式解析为日期时间值
 *
 * @param time 日期
 * @param fmt 格式 默认为”yyyy-mm-dd HH:MM:ss” 可选格式“yyyy/mm/dd hh:MM:ss”“yyyymmmdd hhMMss”；“yyy.mm.dd hh:MM:ss”;“dd mm,yyyy hh:MM:ss”;"yyyymmddhhMMss"
 * @return 时间值
 */
long dtparse(char *time, char* fmt, int size)
{
	int fmt_i;
	struct tm tm;

	if(fmt == NULL)
		fmt_i = 0;
	else if(0 == strncmp(fmt,"yyyy-mm-dd HH:MM:ss", size))
		fmt_i = 0;
	else if(0 == strncmp(fmt,"yyyy/mm/dd hh:MM:ss", size))
		fmt_i = 1;
	else if(0 == strncmp(fmt,"yyy.mm.dd hh:MM:ss", size))
		fmt_i = 2;
	else if(0 == strncmp(fmt,"dd mm,yyyy hh:MM:ss", size))
		fmt_i = 3;
	else if(0 == strncmp(fmt,"yyyymmddhhMMss", size))
		fmt_i = 4;
	else if(0 == strncmp(fmt,"yyyy-mm-dd", size))
		fmt_i = 5;
	else if(0 == strncmp(fmt,"%H:%M:%S", size))
		fmt_i = 6;
	else
		fmt_i = 0;
	
	strptime(time,format[fmt_i], &tm);
	return mktime(&tm);
}


/**
 * 返回字符串是否全以空白符号组成函数。
 *
 * @param str
 * @param 
 * @return 返回TRUE或FALSE
 */
bool isspaces(char* str){
		
		while (*str)
		{
			if (!ISSPACE(*str))
			{
				return FALSE;
			}

			str++;
		}
		return TRUE;
}


/**
 * 比较两个字符串的差值函数。
 *
 * @param str1  字符串1
 * @param str2 字符串2
 * @return 差值
 */
int compare(char *str1, char *str2, size_t size)
{
	return strncmp(str1, str2, size);
}


#if 0
/*取得字符串的hash值*/
char *hash(const char *str)
{
	int i = 0;
	MD5_CTX ctx;
	unsigned char *md5 = (unsigned char *)malloc(16 + 1);
	unsigned char *md5_outs = (unsigned char *)malloc(2*16 + 1);
	
	if (NULL == md5 || NULL == md5_outs)
		return NULL;

	memset(md5, 0, sizeof(md5));
	memset(md5_outs, 0, sizeof(md5_outs));

	if (NULL == str)
		return NULL;

	MD5_Init(&ctx);
	MD5_Update(&ctx, str, strlen(str));
	MD5_Final(md5, &ctx);
	
	for (i = 0; i < 16; i++)
	{
		sprintf(md5_outs[2*i], "%02x", md5[i]);
	}
	
	free(md5);
	return md5_outs;
}
#endif

/*将字符串转换为大小写*/
char *updown(char *str, bool flag)
{
	char *p = str;
	
	if (flag)
	{
		while (*p)
		{
			if (*p >= 'a' && *p <= 'z')
				*p -= ('a' - 'A');
			p++;
		}
	}
	else
	{
		while (*p)
		{
			if (*p >= 'A' && *p <= 'Z')
				*p += ('a' - 'A');
			p++;
		}

	}
	
	return str;
}


/*将字符串转换为大写*/
char *up(char *str)
{
	char *p = str;
	while (*p)
	{
		if (*p >= 'a' && *p <= 'z')
			*p -= ('a' - 'A');
		p++;
	}
	
	return str;
}

/*将字符串转换为大写*/
char *down(char *str)
{
	char *p = str;
	while (*p)
	{
		if (*p >= 'A' && *p <= 'Z')
			*p += ('a' - 'A');
		p++;
	}
	
	return str;
}

/*去掉字符串开头或结尾的空格 
 * flag为TRUE去掉开头空格
 * flag为FALSE去掉后面空格
 */
char *strip2(char *str, bool flag)
{
	int i = 0;
	char *p = str;

	/*去掉开头的空格*/
	if (flag)
	{
		/*开头无空格*/
		if (' ' != *str)
			return str;

		while (' ' == *p)
			p++;

		while (*p)
		{
			*(str+i) = *p;
			*p = '\0';
			i++;
			p++;
		}
	}
	else
	{
		p = str + strlen(str) - 1;

		/*结尾无空格*/
		if (' ' != *p)
			return str;

		while (' ' == *p)
		{
			*p = '\0';
			p--;
		}
	}
	
	return str;
}


/*去掉字符串前/后的空格, flag=1去掉开头空格,flag=2去掉结尾空格,其他去掉头尾的空格*/
char *strip_e(char *str, int flag)
{
	/*去掉开头的空格*/
	if (1 == flag)
		return strip2(str, TRUE);
	/*去掉结尾的空格*/
	else if (2 == flag)
		return strip2(str, FALSE);
	/*去掉字符串前后的空格*/
	else	
	{
		strip2(str, TRUE);
		strip2(str, FALSE);
	}
	return str;
}





