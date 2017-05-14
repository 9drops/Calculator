#ifndef EXP_PARSER_H
#define EXP_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG
#ifdef DEBUG
#define DPRINT(fmt, args...) printf(fmt, ##args)
#else
#define DPRINT(fmt, args...)
#endif

#define EXP_MAX_LEN 1024
#define EXP_MAX_NAME_LEN  128
#define EXP_MAX_BUF_SIZE 4096
#define EXP_MAX_STACK_LEN 32

#define M_STRING "string"
#define M_INT    "int"
#define M_LONG   "long"
#define M_FLOAT  "float"
#define M_DOUBLE "double"
#define M_BOOL   "boolean"
#define M_BYTE   "byte"
#define M_DATA   "data"
 
enum
{
    ERR_OK = 0,
    ERR_EXP_PARAM,          /*表达式参数错误*/
    ERR_EXP_DIVISOR,        /*表达式错误,被除数为0*/
    ERR_EXP_NO_FUNC,        /*表达式不支持此函数*/
    ERR_EXP_BRACKET,        /*表达式括号不匹配*/
    ERR_EXP_INNER,          /*表达式内部错误*/
    ERR_EXP_GET_ONE_VAR,    /*变量值获取失败*/
    ERR_EXP_GET_ALL_VAR,    /*所有变量值获取失败*/
    ERR_EXP_BUFFER_SIZE,    /*表达式存储空间不足*/
};

/*ID of expression function*/
enum
{
	ID_ADD = 1,
	ID_SUB,
	ID_MUL,
	ID_DIV,
	ID_MOD,
	ID_OR,
	ID_AND,
	ID_GT,
	ID_GE,
	ID_NE,
	ID_LT,
	ID_LE,
	ID_EQ,
	ID_GETVAR,
	ID_UPWODN,
	ID_STRIP,
	ID_MATCH,
	ID_ISDIGIT,
	ID_ITOA,
	ID_TRIM,
	ID_TOF,
	ID_TOS,
	ID_SUBSTRING,
	ID_TRUE,
	ID_FALSE,
	ID_NOT,
	ID_LENGTH,
	ID_NOW,
	ID_DTPRINTF,
	ID_YEAR,
	ID_MONTH,
	ID_DAY,
	ID_HOUR,
	ID_MINUTE,
	ID_SECOND,
	ID_REPLACE,
	ID_DTPARSE,
	ID_ISSPACES,
	ID_COMPARE,
	ID_UPDOWN,
	ID_HASH,
	ID_ISDIGITAL,
	ID_IP,
	ID_TASKID,
	ID_ISNULL,
	ID_SELECT,
	ID_ICONV,
	ID_UP,
	ID_DOWN
};


/*weight of function and operator, this enum is very important
*,to control order of operation
*/
enum
{
	WEIGHT_DOT,
	WEIGHT_OR,
	WEIGHT_AND,
	WEIGHT_EQ,
	WEIGHT_GT,
	WEIGHT_ADD,
	WEIGHT_MUL,
	WEIGHT_FUNC
};

/*type of value*/
typedef enum
{
	INVALID = -1,
	DUMY,
	VOID = 1,
	Bool,
	CHAR,
	INT,
	LONG,
	FLOAT,
	DOUBLE,
	STRING,
	T_DATA
}TYPE;


/*function item*/
typedef struct
{
	char id;   /*function id*/
	char weight;/*weight value*/
	char name[EXP_MAX_NAME_LEN];  /*function*/
	int param_num;/*number of parameters*/
}FUNC_ITEM;

/*paramter item*/
typedef struct
{
	TYPE type;
	char value[EXP_MAX_BUF_SIZE];
}PARAM_ITEM;

/*bracket item*/
typedef struct
{
	int func_id;	/*左括号所对应的函数*/
	int fs_top;     /*函数栈顶*/
	int is_lbrckt;  /*是否左括号*/
}BRCKT_ITEM;

/*variable table item*/
typedef struct
{
	int exist;   /*是否存在*/
	int islocal; /*是否局部变量*/
	int isnull;  /*数据区是否存在该变量*/
	TYPE type;
	char name[EXP_MAX_NAME_LEN];
	char value[EXP_MAX_BUF_SIZE];
}VAR_ITEM;


/*
* Func    : exp_parser
* Desp    : 表达式解析
* Input   : exp - 表达式串,形如“#substring('123456', 0, 3)”
* Output  : result - 运算结果
* Output  : res_type - 运算结果数据类型
* Input   : size - result的字节数
* Return  : 0 - 成功,  非0失败
*           特别的，
*           当解析形如"@a.b + @a.c + 'hello'"的表达式,
*           如果数据区中无@a.b和@a.c 或 他们都是空节点(只有节点名字无值), 
*           表达式返回0且结果类型出参res_type为0
*/
int exp_parser(char *exp, char *result, TYPE *res_type, int size);

/*
* Func   : exp_get_stype
* Desp   : 根据表达式变量类型返回数据模型类型串
* Input  : itype - 结构数据类型
* Return : 非空 - 成功, NULL - 出错
*          特别的,形如@a.b的变量
*          如果数据区中无@a.b或是空节点（有节点名字无值）
*          返回NULL
*/
char *exp_get_stype(TYPE itype);

/*去掉数字字符串中多余的0，例如1.3000处理为1.3*/
char *exp_trim_zero(char *result);

#ifdef __cplusplus
}
#endif

#endif
