/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Wed 17 Jun 2020 02:27:07 PM CST
 @ File Name	: regex.c
 @ Description	: 正则相关函数库使用
 ************************************************************************/
/*
 * 一个简单例子
 * ./regex "(^http.?)://(.+)/(.*)\?(.*)" https://www.tapd.cn/my_worktable?source_user=378414182&workspace_id=51351936&workitem_type=task&workitem_id=1151351936001062150#&filter_close=true
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>

void print_reg_error(int errcode, const regex_t *preg)
{
	char errbuf[1024];
	regerror(errcode, preg, errbuf, 1024);
	fprintf(stderr, "regerror:%s\n", errbuf);
}

void print_match_struct(const char * str, size_t nmatch, regmatch_t pmatch[]) 
{
	char tmp[1024];
	printf("------------------------begin\n");
	for(int idx = 0; idx < nmatch; ++idx) {
		if (-1 != pmatch[idx].rm_so) {
			int diff = pmatch[idx].rm_eo - pmatch[idx].rm_so;
			memcpy(tmp, str + pmatch[idx].rm_so, diff);
			tmp[pmatch[idx].rm_eo - pmatch[idx].rm_so] = '\0';
			printf("%d.(%d,%d) --> %s\n", idx+1, pmatch[idx].rm_so, pmatch[idx].rm_eo, tmp);
		} else {
			printf("%d.(%d,%d)\n", idx+1, pmatch[idx].rm_so, pmatch[idx].rm_eo);
		}
	}	
	printf("------------------------end\n");
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage : %s regex [str1,str2,...]\n", __FILE__);		
		exit(EXIT_FAILURE);
	}

	regex_t preg;
	const char * regex = argv[1];

	int ret = regcomp(&preg, regex, REG_EXTENDED);	
	if (0 != ret) {
		print_reg_error(ret, &preg);
		regfree(&preg);
		exit(EXIT_FAILURE);
	}

	size_t nmatch = 10;	
	regmatch_t pmatch[10];

	for(int idx = 2; idx < argc; ++idx) {	
		ret = regexec(&preg, argv[idx], nmatch, pmatch, 0);
		if (0 == ret) {
			printf("%s : match\n", argv[idx]);
			print_match_struct(argv[idx], nmatch, pmatch);
		} else if ( REG_NOMATCH == ret) {
			printf("%s : nomatch\n", argv[idx]);
		} else {
			print_reg_error(ret, &preg);
			regfree(&preg);
			exit(EXIT_FAILURE);
		}
	}	

	return 0;
}
