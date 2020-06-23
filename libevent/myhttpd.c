/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Mon 15 Jun 2020 07:48:34 PM CST
 @ File Name	: myhttpd.c
 @ Description	: 
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>

#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

void PrintError(FILE * stream, int my_errno, const char * headStr, int exitCode)
{

#if (-1 == my_errno)
	fprintf(stream, "%s\n", headStr);	
#else 
	if (!my_errno) {
		my_errno = errno;
	}	
	fprintf(stream, "%s : %s\n", headStr, strerror(my_errno));	
#endif
	if (exitCode) {
		exit(exitCode);	
	}
}

void PrintAddr(FILE * stream, struct sockaddr_in * addr, const char * headStr)
{
	char arr[INET_ADDRSTRLEN];	
	inet_ntop(AF_INET, &(addr->sin_addr), arr, sizeof(arr)); 
	fprintf(stream, "%s, %s:%d\n", headStr, arr, ntohs(addr->sin_port));	
}

void sigint(evutil_socket_t fd, short what, void * arg)
{	
	struct timeval t1 = {1, 0};
	struct event_base * base = (struct event_base *)arg;
	event_base_loopexit(base, &t1);
	printf("loop exit after 1 secs\n");
}

/***************************************************
* url 编码解码 
***************************************************/
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
	char res[1024];
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
	char res[1024];
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

/***************************************************
* other 
***************************************************/
struct http_request_t {
	int status;		// 0 invalid request, 1 ok
	char method[64];	
	char path[256];	
	char pro[64]; 
};

void add_http_header(struct evbuffer *buf, int code, const char * desc, const char * content_type)
{
	char header[1024]; 
	sprintf(header, "HTTP/1.1 %d %s\nContent-Type: %s\n\n", code, desc, content_type);
	evbuffer_add(buf, header, strlen(header));
}

void handler_404(struct evbuffer *buf)
{	
	add_http_header(buf, 404, "Not Found", "text/html");
	char * tmp = "<html><body><h1>404 Not Found.</h1></body></html>";
	evbuffer_add(buf, tmp, strlen(tmp));
}

void handler_invalid_request(struct evbuffer *buf)
{	
	add_http_header(buf, 200, "OK", "text/html");
	char * tmp = "<html><body><h1>invalid request</h1></body></html>";
	evbuffer_add(buf, tmp, strlen(tmp));
}

void parse_request(struct http_request_t * req, struct evbuffer *buf)
{
	char tmp[10240];
	int ret = evbuffer_remove(buf, tmp, 10240);
	printf("-------------------------------begin---------------------\n"); 
	printf("%s\n", tmp);
	printf("-------------------------------end---------------------\n"); 

	if (-1 == ret) {
		PrintError(stderr, -1, "evbuffer_remove error", 0);
		req->status = 0;
		return ;	
	}

	// 读http request 头, 必定会成功, 不成功就返回错误信息
	// GET / HTTP/1.1
	ret = sscanf(tmp, "%s %s %s", req->method, req->path, req->pro);	
	printf("before decode : %s\n", req->path);
	urldecode(req->path);
	printf("after decode : %s\n", req->path);

	if (3 != ret) {
		PrintError(stderr, -1, "sscanf error", 0);
		req->status = 0;
		return ;	
	}

	req->status = 1;	
}

const char * get_file_type(const char * file)
{
	// 后缀名
	char * dot = strrchr(file, '.');
	
	// 没有找到
	if (NULL == dot) {
		return "text/plain; charset=utf-8";	
	}
	
	if (strcmp(dot, ".html") == 0) {
		return "text/html; charset=utf-8";	
	} else if (strcmp(dot, ".jpg") == 0) {	
		return "image/jpeg";	
	} else if (strcmp(dot, ".png") == 0) {	
		return "image/png";	
	} else if (strcmp(dot, ".mp4") == 0) {	
		return "vedio/mpeg4";	
	} else if (strcmp(dot, ".mp3") == 0) {	
		return "audio/mp3";	
	} else if (strcmp(dot, ".avi") == 0) {	
		return "vedio/avi";	
	} else if (strcmp(dot, ".ico") == 0) {	
		return "image/x-icon";	
	} else { 
		return "text/plain; charset=utf-8";	
	}
}

void send_regular_file(struct http_request_t * req, struct evbuffer *tmpbuf, const char * spath)
{
	int fd = open(spath, O_RDONLY);
	if (-1 == fd) {
		PrintError(stderr, -1, "open error", EXIT_FAILURE);
	}
	
	char cache[1024]; 
	int cnt;
	while ((cnt = read(fd, cache, 1024)) > 0) {	
		evbuffer_add(tmpbuf, cache, cnt);
	}

	if (-1 == cnt) {
		PrintError(stderr, -1, "read error", EXIT_FAILURE);
	}

	close(fd);
}

void send_dir(struct http_request_t * req, struct evbuffer *tmpbuf, const char * spath)
{
	char cache[1024];	
	struct dirent ** namelist;

	int ret = scandir(spath, &namelist, NULL, alphasort);	
	if (ret == -1) {
		PrintError(stderr, -1, "scandir error", EXIT_FAILURE);
	}	

	sprintf(cache, "<html><body><head><title>%s</title></head>", req->path);
	evbuffer_add(tmpbuf, cache, strlen(cache));
	sprintf(cache, "<h1>now dir : %s</h1><hr><table>", req->path);
	evbuffer_add(tmpbuf, cache, strlen(cache));

	for (int idx = 0; idx < ret; ++idx) {
		sprintf(cache, "<tr>\n");
		evbuffer_add(tmpbuf, cache, strlen(cache));

		if (strcmp(namelist[idx]->d_name, ".") == 0) {
			sprintf(cache, "<td><a href=\"%s\"><font size=5>%s</font></a></td>\n", req->path, namelist[idx]->d_name);
		} else if (strcmp(namelist[idx]->d_name, "..") == 0) {
			char path_cache[256];	
			strcpy(path_cache, req->path);
			char * last = strrchr(path_cache, '/');
			if (NULL != last && last != path_cache) {
				// 不是开头的/, 直接砍掉后面的
				*last = '\0'; 
			} else {
				// 就是开头的/, 或者没有匹配到/, 直接根目录	
				strcpy(path_cache, "/");		
			}	
			sprintf(cache, "<td><a href=\"%s\"><font size=5>%s</font></a></td>\n", path_cache, namelist[idx]->d_name);
		} else {
			// 文件size大小
			int len = strlen(req->path);  
			char file_path[1024];
			if (req->path[len - 1] == '/') {
				sprintf(file_path, "%s%s", spath, namelist[idx]->d_name);
			} else {
				sprintf(file_path, "%s/%s", spath, namelist[idx]->d_name);
			}

			struct stat statbuf;
			if (-1 == stat(file_path, &statbuf)) {
				PrintError(stderr, -1, "call stat error", EXIT_FAILURE);
			}

			if (req->path[len - 1] == '/') {
				// '/'结尾， 不用再连接 /
				sprintf(cache, "<td><a href=\"%s%s\"><font size=5>%s</font></a></td><td><font size=5>%ld</font></td>\n", req->path, namelist[idx]->d_name, namelist[idx]->d_name, statbuf.st_size);
			} else {
				sprintf(cache, "<td><a href=\"%s/%s\"><font size=5>%s</font></a></td><td><font size=5>%ld</font></td>\n", req->path, namelist[idx]->d_name, namelist[idx]->d_name, statbuf.st_size);
			}
		}

		evbuffer_add(tmpbuf, cache, strlen(cache));
		free(namelist[idx]);

		sprintf(cache, "</tr>\n");
		evbuffer_add(tmpbuf, cache, strlen(cache));
	}
		
	sprintf(cache, "</table></body></html>");
	evbuffer_add(tmpbuf, cache, strlen(cache));
}

void main_handler(struct http_request_t * req, struct evbuffer *buf)
{
	if (!req->status) {
		handler_invalid_request(buf);
		return ;
	}
		
	// path 路径兼容	
	char spath[300] = {0};
	if (req->path[0] == '/') {
		// 转下路径
		sprintf(spath, ".%s", req->path);			
	} else {
		sprintf(spath, "%s", req->path);	
	}		

	// 以调用进程的实际用户ID， 而不是有效用户ID， 访问文件， 判断文件是否存在
	int res = access(spath, F_OK);
	if (-1 ==res) {
		// 不存在
		handler_404(buf);
		return ;
	}

	struct stat statbuf;
	if (-1 == stat(spath, &statbuf)) {
		PrintError(stderr, -1, "call stat error", EXIT_FAILURE);
	}
	
	struct evbuffer * tmpbuf = evbuffer_new();

	char * appoint_mine_type = NULL;
	if (statbuf.st_mode & S_IFDIR) {
		// 目录
		appoint_mine_type = (char *)get_file_type(".html");	
		send_dir(req, tmpbuf, spath);
	} else {
		// 其他文件
		send_regular_file(req, tmpbuf, spath);
	}	

	if (appoint_mine_type) {
		add_http_header(buf, 200, "OK", appoint_mine_type);
	} else {
		add_http_header(buf, 200, "OK", get_file_type(spath));
	}
	evbuffer_add_buffer(buf, tmpbuf);				

	evbuffer_free(tmpbuf);	
}	

void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *readbuf, *writebuf;
	readbuf = evbuffer_new();
	writebuf = evbuffer_new();
	if (NULL == readbuf || NULL == writebuf) {
		PrintError(stderr, -1, "read_cb, evbuffer_new error", EXIT_FAILURE);
	}

	if (-1 == bufferevent_read_buffer(bev, readbuf)) {
		PrintError(stderr, -1, "read_cb, bufferevent_read_buffer error", EXIT_FAILURE);
	}

	// 解析requst
	struct http_request_t req;
	bzero(&req, sizeof(req));
	parse_request(&req, readbuf);

	// 主逻辑	
	main_handler(&req, writebuf);					
		
	if (-1 == bufferevent_write_buffer(bev, writebuf)) {
		PrintError(stderr, -1, "read_cb, bufferevent_write_buffer error", EXIT_FAILURE);
	}

	evbuffer_free(readbuf);
	evbuffer_free(writebuf);	
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	int cfd = bufferevent_getfd(bev);
	printf("fd %d write data successfully\n", cfd);

	// TODO
	// bufferevent_enable(bev, EV_READ | EV_WRITE)) 显式指定EV_WRITE 居然会在连接建立好后马上调用一下 write_cb 

	// 主动关闭对端连接, close_on_free flag
	bufferevent_free(bev);
	printf("active close fd %d\n", cfd);	
	
}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (BEV_EVENT_EOF & what) {
		bufferevent_free(bev);
		int cfd = bufferevent_getfd(bev);
		printf("close fd %d\n", cfd);	
	} else if (BEV_EVENT_TIMEOUT& what) {

	} else {
		PrintError(stderr, -1, "event_cb, someerror happen", EXIT_FAILURE);
	}
}

void connect_call_back(struct evconnlistener * evlistener, evutil_socket_t cfd, struct sockaddr * cli_addr, int socklen, void * arg)
{
	const char * aa = (const char *)arg;
	PrintAddr(stdout, (struct sockaddr_in *)cli_addr, aa);
	
	// 添加读写bufferevent
	struct event_base * base = evconnlistener_get_base(evlistener);	
	struct bufferevent * bev = bufferevent_socket_new(base, cfd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev) {
		PrintError(stderr, -1, "bufferevent_socket_new error", EXIT_FAILURE);
	}
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

	// 默认就开启write， 不要在这里指定write， 居然会在连接建立后马上调用write_cb
	if (-1 == bufferevent_enable(bev, EV_READ)) {
		PrintError(stderr, -1, "bufferevent_enable error", EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {	
	
	// 设置开始的工作目录
	if (2 != argc) {
		printf("usage : %s path", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if (-1 == chdir(argv[1])) {			
		PrintError(stderr, -1, "bufferevent_enable error", EXIT_FAILURE);
	}
	
	struct event_base * base = event_base_new();

	struct event * sigEv = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sigint, base); 
	event_add(sigEv, NULL);

	struct sockaddr_in svrAddr;
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(8888);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	struct evconnlistener * evlistener = evconnlistener_new_bind(base, connect_call_back, "new connection", LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
	if (NULL == evlistener) {
		PrintError(stderr, -1, "evconnlistener_new_bind error", EXIT_FAILURE);
	}
		
	printf("begin event loop\n");
	event_base_dispatch(base);

	event_free(sigEv);
	evconnlistener_free(evlistener);
	event_base_free(base);

	return 0;
}
