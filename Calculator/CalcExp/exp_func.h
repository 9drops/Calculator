/**
 * 表达式计算所用函数接口
 */
#ifndef EXP_FUNC_H_
#define EXP_FUNC_H_

#ifdef __cplusplus
extern "C" {
#endif


/*double类型最大位数*/
#define MAX_DOUBLE_PREC 14
/*double类型小数点最大位数*/
#define MAX_DOT_PREC 6
/*浮点数和0的偏差区间*/
#define MIN_DOUBLE_INTERAL 0.000001

#define bool char
#define TRUE  1
#define FALSE 0


#define ISSPACE(x)	( (x)==' ' || (x)=='\r' || (x)=='\n' || (x)=='\t' || (x)=='\f')

#define MAX_STRING_LEN  (512)
#define MAX_MATCHED_NUM (32)

int add_i(int a, int b);
double add_d(double a, double b);
int add_s(char *a, char *b, char *res, int len);
int sub_i(int a, int b);
double sub_d(double a, double b);
int64_t mul_i(int a, int b);
double mul_d(double a, double b);
int div_i(int a, int b);
double div_d(double a, double b);
int mod(int a, int b);
int or(int a, int b);
int and(int a, int b);
int eq_i(int a, int b);
int ne_i(int a, int b);
int gt_i(int a, int b);
int ge_i(int a, int b);
int lt_i(int a, int b);
int le_i(int a, int b);
int eq_d(double a, double b);
int eq_str(const char *a, const char *b, int size);
int ne_d(double a, double b);
int gt_d(double a, double b);
int gt_str(const char *a, const char *b, int size);
int lt_str(const char *a, const char *b, int size);
int le_str(const char *a, const char *b, int size);
int ge_str(const char *a, const char *b, int size);
int ge_d(double a, double b);
int lt_d(double a, double b);
int le_d(double a, double b);

long add_l(long a, long b);
long sub_l(long a, long b);
long mul_l(long a, long b);
long div_l(long a, long b);
long mod_l(long a, long b);
int eq_l(long a, long b);
int ne_l(long a, long b);
int gt_l(long a, long b);
int ge_l(long a, long b);
int lt_l(long a, long b);
int le_l(long a,  long b);

/*字符串精度处理函数*/
char *fstrprec(char *result, int size, int prec);

/*三元运算*/
int ternaryop(bool isnot, char *a, char *b);

/*编码转换函数*/
int encode(char *out, int *outlen, const char *outcode, char *in, int *inlen, const char *incode);

/*将字符串转换为大写*/
char *up(char *str);

/*将字符串转换为大写*/
char *down(char *str);

/*
 *数字转字符串函数
 *
 *@param num 被转换的数字
 *return str 转换成功的字符串
 */
char *itoa(long num, char str[]);

/*
 *去除字符串中的空白字符
 *
 *@param str 源字符串
 *return 去除空白字符后的字符串
 */
char *trim(char *src, char *dest);


/**
 * 将string转换为数字
 *
 * @param val 被转换的字符串
 * @return long 转换成的数字
 */
double tof_s(const char *var);

/**
 * 将bool变量转换为字符串
 *
 * @param num bool数
 * @return 字符串
 */
char *tos_b(bool num);

/**
 * 将数字转换为字符串支持给定精度转换。
 *
 * @param buf 字符串存储空间
	* @param num 被转换的的数字
 * @param prec 转换精度
 * @return 字符串
 */
char *tos_f(char* buf, int size, double num,int prec);



/**
 * 从str中获取子字符串
 *
 * @param str 可以是字符串和变量
 * @param offset 为截取字符串的开始索引，类型为变量或数字。
 * @param count 是截取字符串个数，默认为全部，类型可以是变量或数字。
 * @return 字符串
 */
char *substring(char* str, char* result,int offset, int count);


/**
 * 返回TRUE函数。
 *
 * @param 无
 * @param 
 * @return TRUE
 */
bool true();


/**
 * 返回FALSE数。
 *
 * @param 无
 * @param 
 * @return FALSE
 */
bool false();


/**
 * 取反函数。
 *
 * @param flag
 * @return TRUE or FALSE
 */
bool not(bool flag);


/**
 * 计算参数字符串的长度。
 *
 * @param str目标字符串
 * @return 参数字符串的长度
 */
int  length(const char* str);


/**
 * 该函数返回当前的时间值
 *
 * @param 无
 * @param 无
 * @return 当前时间值
 */
long now();



/**
 * 将时间值格式化为字符串函数
 *
 * @param time_n为转换时间值
 * @param fmt是日期转换格式，默认为”yyyy-mm-dd HH:MM:ss” 可选格式“yyyy/mm/dd hh:MM:ss”“yyyymmmdd hhMMss”；“yyy.mm.dd hh:MM:ss”;“dd mm,yyyy hh:MM:ss”;"yyyymmddhhMMss"
	* @param s_time 时间存储空间
	* @param len 空间大小
 * @return 空
 */
char *dtprintf(long time_n, char *fmt, int size, char s_time[], int len);



/**
 * 返回日期的年份函数,该函数实现从给定时间中获取年份，time参数为可选参数。如果参数为空，返回当前年。
 *
 * @param time_n
 * @param 
 * @return  年份
 */
int year(long time_n);


/**
 * 返回日期的月份函数。该函数实现从给定时间中获取月份，time参数为可选参数。如果参数为空，返回当前月份。
 *
 * @param time_n
 * @param 
 * @return 月份
 */
int month(long time_n);



/**
 * 返回日期的天函数。该函数实现从给定时间中获取日，time参数为可选参数。如果参数为空，返回当前天。
 *
 * @param time_n
 * @param 
 * @return 天数
 */
int day(long time_n);



/**
 * 返回日期的小时函数。该函数实现从给定时间中获取小时，time参数为可选参数，如果参数为空，返回当前小时。
 *
 * @param time_n
 * @param 
 * @return 小时
 */
int hour(long time_n);



/**
 * 返回日期的分钟函数。该函数实现从给定时间中获取分钟，time参数为可选参数，如果参数为空，返回当前分钟。
 *
 * @param time_n
 * @param 
 * @return 分钟
 */
int minute(long time_n);



/**
 * 返回日期的秒函数。该函数实现从给定时间中获取秒钟，time参数为可选参数，如果参数为空，返回当前秒。
 *
 * @param time_n
 * @param 
 * @return 秒数
 */
int second(long time_n);



/**
 * 替换字符串中符合条件的字符串为指定的内容
 *	该函数实现了将字符串str中出现rule的地方全部替换为rep的值
 * @param src 源字符串
 * @param rule 规则
 * @param rep 替换值
 * @return 失败 -1 ;成功 0
 */
char *replace(char* src, char* rule, char* rep, bool regex, char *dst, int size);




/**
 * 将日期字符串按照给定的格式解析为日期时间值
 *
 * @param time 日期
 * @param fmt 格式 默认为”yyyy-mm-dd HH:MM:ss” 可选格式“yyyy/mm/dd hh:MM:ss”“yyyymmmdd hhMMss”；“yyy.mm.dd hh:MM:ss”;“dd mm,yyyy hh:MM:ss”;"yyyymmddhhMMss"
 * @return 时间值
 */
long dtparse(char *time, char* fmt, int size);


/**
 * 返回字符串是否以空白符号组成函数。
 *
 * @param str
 * @param 
 * @return 若有空白字符，返回TRUE，否则返回FALSE
 */
bool isspaces(char* str);



/**
 * 比较两个字符串的差值函数。
 *
 * @param str1  字符串1
 * @param str2 字符串2
 * @return 差值
 */
int compare(char *str1, char *str2, size_t size);


/*取得字符串的hash值*/
char *hash(const char *str);

/*将字符串转换为大小写*/
char *updown(char *str, bool flag);

/*去掉字符串前/后的空格, flag=1去掉开头空格,flag=2去掉结尾空格,其他去掉头尾的空格*/
char *strip_e(char *str, int flag);

/*正则表达式匹配*/
bool match(const char *str, const char *regex);

/*判断字符串是否数字串，支持int\float\科学计数法"1.0E+08"*/
bool isdigital(const char *str);

#ifdef __cplusplus
}
#endif

#endif 
