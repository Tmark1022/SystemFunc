/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Tue 23 Jun 2020 06:24:31 PM CST
 @ File Name	: url_encode_decode.c
 @ Description	: url的编码解码demo 
 ************************************************************************/
/*
	1、字符'a'-'z','A'-'Z','0'-'9','.','-','*'和'_' 都不被编码，维持原值；
	2、空格' '被转换为加号'+'。(新标准是%20, encode时当做下边第3种的情况处理【并没有转为+】, 在decode时并没有处理【并没有把+转成空格】)
	3、其他每个字节都被表示成"%XY"的格式，X和Y分别代表一个十六进制位。编码为UTF-8。
 * */

#include <stdio.h>
#include <string.h>

#define BURSIZE 1024

// 16进制的字符串转换成10进制的二进制存储方式
int hex2dec(char c)
{
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else {
		return -1;
	}
}

// 10机制的二级制存储转换成16进程的字符形式
char dec2hex(short int c)
{
	if (0 <= c && c <= 9) {
		return c + '0';
	} else if (10 <= c && c <= 15) {
		return c + 'A' - 10;
	} else {
		return -1;
	}
}


/*
 * 编码一个url
 */
void urlencode(char url[])
{
	int i = 0;
	int len = strlen(url);
	int res_len = 0;
	char res[BURSIZE];
	for (i = 0; i < len; ++i) {
		char c = url[i];
		if (('0' <= c && c <= '9') ||
				('a' <= c && c <= 'z') ||
				('A' <= c && c <= 'Z') || c == '/' || c == '.') {
			res[res_len++] = c;
		} else {
			int j = (short int)c;
			if (j < 0)
				j += 256;
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1 * 16;
			res[res_len++] = '%';
			res[res_len++] = dec2hex(i1);
			res[res_len++] = dec2hex(i0);
		}
	}
	res[res_len] = '\0';
	strcpy(url, res);
}

/*
 * 解码url
 */
void urldecode(char url[])
{
	int i = 0;
	int len = strlen(url);
	int res_len = 0;
	char res[BURSIZE];
	for (i = 0; i < len; ++i) {
		char c = url[i];
		if (c != '%') {
			res[res_len++] = c;
		} else {
			char c1 = url[++i];
			char c0 = url[++i];
			int num = 0;
			num = hex2dec(c1) * 16 + hex2dec(c0);
			res[res_len++] = num;
		}
	}
	res[res_len] = '\0';
	strcpy(url, res);
}

int main(int argc, char *argv[])
{
	char url[BURSIZE] = "http://'www.baidu.com/a b名字.txt";
	urlencode(url);
	printf("%s\n", url);



	char url2[BURSIZE] = "/%E5%90%8D%20%E5%AD%97.txt";
	urldecode(url2);
	printf("%s\n", url2);

	char url3[BURSIZE] = "/%E5%90%8D+%E5%AD%97.txt";
	urldecode(url3);
	printf("%s\n", url3);

	return 0;
}
