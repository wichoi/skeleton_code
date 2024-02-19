#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

int tcp_client_init(void);
int tcp_client_deinit(void);

int tcp_client_send(int fd, char *data, size_t len);

#endif
