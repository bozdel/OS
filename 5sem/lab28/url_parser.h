#ifndef URL_PARSER_H
#define URL_PARSER_H

struct url {
	char scheme[20];
	char login[100];
	char password[100];
	char host[100];
	char port[100];
	char path[100];
	char params[100];
	char anchor[100];
};

void init_url(struct url *url);

void print_url(struct url url);

int parse_addr(char *string, struct url *url);

int parse_path(char *string, struct url *url);

int parse_params(char *string, struct url *url);

int parse_anchor(char *string, struct url *url);

int parse_url(char *url_str, struct url *url);

#endif