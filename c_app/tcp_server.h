#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

int tcp_server_init(void);
int tcp_server_deinit(void);

int tcp_server_send(int fd, char *data, size_t len);

#endif
