#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "url_parser.h"



void init_url(struct url *url) {
	memset(url->scheme, 0, 20);
	memset(url->login, 0, 100);
	memset(url->password, 0, 100);
	memset(url->host, 0, 100);
	memset(url->port, 0, 100);
	memset(url->path, 0, 100);
	memset(url->params, 0, 100);
	memset(url->anchor, 0, 100);
}

void print_url(struct url url) {
	printf("scheme: %s\n", url.scheme);
	printf("login: %s\n", url.login);
	printf("password: %s\n", url.password);
	printf("host: %s\n", url.host);
	printf("port: %s\n", url.port);
	printf("path: %s\n", url.path);
	printf("params: %s\n", url.params);
	printf("anchor: %s\n", url.anchor);
}

// return index plus one make correct incrementation address string leng to scheme
// parse_addr - 2 (in recursion) because we pass shifted string by 2 elems because it skip doubled slash
// so we need to extract this 2 elements from returned length
int parse_addr(char *string, struct url *url) {
	// printf("%s\n", string);
	int leng = strlen(string);
	char buff[100] = { 0 };
	int i = 2; // skipping "//" in begining
	for (i; i < leng && string[i] != '/' && string[i] != ':' && string[i] != '@'; i++) {
		buff[i - 2] = string[i];
	}
	if (i >= leng || string[i] == '/') {
		// printf("host: %s\n", buff);
		memcpy(url->host, buff, strlen(buff));
		return i + 1;
		// return i;
	}
	else if (string[i] == ':') {
		// printf("host or login: %s\n", buff);
		char buff2[100] = { 0 };
		int j = i + 1;
		for (j; j < leng && string[j] != '@' && string[j] != '/'; j++) {
			buff2[j - i - 1] = string[j];
		}
		if (string[j] == '@') {
			// printf("login: %s\n", buff);
			memcpy(url->login, buff, strlen(buff));
			// printf("password: %s\n", buff2);
			memcpy(url->password, buff2, strlen(buff2));

			return (j + 1) + (parse_addr(string + j + 1 - 2, url) - 2); // -2 for balancing "//"
			// return j + (parse_addr(string + j + 1 - 2) - 2); // -2 for balancing "//"
		}
		else if (string[j] == '/') {
			// printf("host: %s\n", buff);
			memcpy(url->host, buff, strlen(buff));
			// printf("port: %s\n", buff2);
			memcpy(url->port, buff2, strlen(buff2));
			return j + 1;
			// return j;
		}
		return j;
	}
	else if (string[i] == '@') {
		// printf("login: %s\n", buff);
		memcpy(url->login, buff, strlen(buff));
		return (i + 1) + (parse_addr(string + i + 1 - 2, url) - 2); // -2 for balancing "//"
		// return i + (parse_addr(string + i + 1 - 2) - 2); // -2 for balancing "//"
	}
}

int parse_path(char *string, struct url *url) {
	int leng = strlen(string);
	char buff[100] = { 0 };
	int i = 0;
	for (i; i < leng && string[i] != '?' && string[i] != '#'; i++) {
		buff[i] = string[i];
	}
	// printf("path: %s\n", buff);
	memcpy(url->path, buff, strlen(buff));
	return i;
}

int parse_params(char *string, struct url *url) {
	int leng = strlen(string);
	char buff[100] = { 0 };
	int i = 1; // 0 ???
	for (i; i < leng && string[i] != '#'; i++) {
		buff[i - 1] = string[i];
	}
	// printf("params: %s\n", buff);
	memcpy(url->params, buff, strlen(buff));
	return i;
}

int parse_anchor(char *string, struct url *url) {
	int leng = strlen(string);
	char buff[100] = { 0 };
	int i = 1; // 0 ???
	for (i; i < leng; i++) {
		buff[i - 1] = string[i];
	}
	// printf("anchor: %s\n", buff);
	memcpy(url->anchor, buff, strlen(buff));
	return i;
}

int parse_url(char *url_str, struct url *url) {
	init_url(url);
	int url_leng = strlen(url_str);
	printf("url: %s\n", url_str);
	// printf("leng: %d\n", url_leng);
	char buff[100] = { 0 };
	int i = 0;
	for (i = 0; i < url_leng && url_str[i] != ':'; i++) {
		buff[i] = url_str[i];
	}
	// printf("scheme: %s\n", buff);
	memcpy(url->scheme, buff, strlen(buff));

	/*int retval = 0;
	switch (url[i + 1]) {
		case '/':
			if (url[i + 2] == '/') retval = parse_addr(url + i + 1);
			parse_path(url + i + retval);
			break;
		case '?':

	}*/
	int j = 0;
	int need_parse = 1;
	do {
		if (url_str[i + 1] == '/') {
			// printf("in /\n");
			if (url_str[i + 2] == '/') {
				// printf("in //\n");
				i += parse_addr(url_str + i + 1, url) - 1;
			}
			else {
				i += parse_path(url_str + i + 1, url);
			}
			// printf("%s\n%s\n", url, url + i + 1);
			// printf("i: %d (sym: %c)\n", i, url[i]);
			// sleep(1);
		}
		if (url_str[i + 1] == '?') {
			i += parse_params(url_str + i + 1, url);
		}
		if (url_str[i + 1] == '#') {
			i += parse_anchor(url_str + i + 1, url);
		}
		need_parse = (i < (url_leng - 1)) || (url_str[i + 1] == '/') || (url_str[i + 1] == '?') || (url_str[i + 1] == '#');
		// printf("\tremain: %s\n", url + i + 1);
	} while (need_parse);
	printf("parsing completed\n");
}

/*int main() {
	char *tst_url = "https://123:4567@89ABC:DEF/";
	char *url1 = "https://v.bondarev:aloha@www.google.com:345/";
	char *url2 = "https://v.bondarev@www.google.com:345/";
	char *url3 = "https://v.bondarev:aloha@www.google.com/";
	char *url4 = "https://v.bondarev@www.google.com/";
	char *url5 = "https://www.google.com:345/";
	char *url6 = "https://www.google.com/";
	char *url7 = "https://www.google.com";
	char *url8 = "https://www.google.com/aloha/testbuff?par1=val1;par2=val2#jopa";
	int val = 0;
	val = parse_url(tst_url);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url1);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url2);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url3);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url4);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url5);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url6);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url7);
	printf("\t\tnext (val: %d)\n", val);
	val = parse_url(url8);
	printf("\t\tnext (val: %d)\n", val);
	return 0;
}*/

/*int main() {
	struct url url;
	// init_url(&url);
	char *tst_url = "https://123:4567@89ABC:DEF/";
	char *url1 = "https://v.bondarev:aloha@www.google.com:345/";
	char *url2 = "https://v.bondarev@www.google.com:345/";
	char *url3 = "https://v.bondarev:aloha@www.google.com/";
	char *url4 = "https://v.bondarev@www.google.com/";
	char *url5 = "https://www.google.com:345/";
	char *url6 = "https://www.google.com/";
	char *url7 = "https://www.google.com";
	char *url8 = "https://www.google.com/aloha/testbuff?par1=val1;par2=val2#jopa";
	int val = 0;
	val = parse_url(tst_url, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url1, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url2, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url3, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url4, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url5, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url6, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url7, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	val = parse_url(url8, &url);
	printf("\t\tnext (val: %d)\n", val);
	print_url(url);
	return 0;
}*/