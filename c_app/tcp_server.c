#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"
#include "tcp_server.h"

//#undef AF_INET6

#define TCP_LISTEN_PORT         15119

typedef enum tcp_server_state_enum
{
    TCP_SERVER_ST_NONE          = 0,
    TCP_SERVER_ST_START         = 1,
    TCP_SERVER_ST_CREATE        = 2,
    TCP_SERVER_ST_PROC          = 3,
    TCP_SERVER_ST_DESTORY       = 4,
    TCP_SERVER_ST_END           = 10,
    TCP_SERVER_ST_EXIT          = 11
} tcp_server_st_e;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _tcp_server_st = TCP_SERVER_ST_NONE;

static int _server_fd = -1;
static int _conn_fd = -1;

static void tcp_server_proc(void);
static void tcp_server_state(void);
static int on_event(const event_data *ev);

static int tcp_server_listen_proc(void *arg);
static int tcp_server_recv_proc(void *arg);

int tcp_server_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 0;
    _tcp_server_st = TCP_SERVER_ST_NONE;

    pthread_t tcp_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&tcp_thread, &attr, (void*)tcp_server_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(tcp_thread);

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_SELFTEST, on_event);

    return ret_val;
}

int tcp_server_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 1;
    return ret_val;
}

static void tcp_server_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        tcp_server_state();
        usleep(1000 * 100);
    }
}

static int tcp_server_create(void)
{
    int ret = RET_OK;
#if defined(AF_INET6)
    struct sockaddr_in6 address6;
    int addrlen = sizeof(address6);
#else
    struct sockaddr_in address;
    int addrlen = sizeof(address);
#endif
    int opt = 1;

    log_i("%s\n", __func__);

#if defined(AF_INET6)
    _server_fd = socket(AF_INET6, SOCK_STREAM, 0);
#else
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if(_server_fd < 0)
    {
        log_e("%s socket create failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }

    if(setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        log_e("%s socket setsockopt failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }

#if defined(AF_INET6)
    memset(&address6, 0, sizeof(address6));
    address6.sin6_family = AF_INET6;
    address6.sin6_addr = in6addr_any;
    address6.sin6_port = htons(TCP_LISTEN_PORT);
    if(bind(_server_fd, (struct sockaddr *)&address6, sizeof(address6)) < 0)
    {
        log_e("%s socket bind failed errno[%d] !!!\n", __func__, errno);
        close(_server_fd);
        return RET_ERR;
    }
#else
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_LISTEN_PORT);
    if(bind(_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        log_e("%s socket bind failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }
#endif

    if(listen(_server_fd, 10) < 0)
    {
        log_e("%s socket listen failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }

    pthread_t listen_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&listen_thread, &attr, (void*)tcp_server_listen_proc, (void*)&_server_fd) < 0)
    {
        log_e("pthread_create failed\n");
        ret = RET_ERR;
    }
    pthread_detach(listen_thread);

    return ret;
}

static int tcp_server_listen_proc(void *arg)
{
    int ret = RET_OK;
#if defined(AF_INET6)
    struct sockaddr_in6 clientaddr;
#else
    struct sockaddr_in clientaddr;
#endif
    int addrlen = sizeof(clientaddr);
    char str[INET6_ADDRSTRLEN + 1] = {0,};
    int server_fd = *(int *)arg;
    //int conn_fd = -1; // _conn_fd
    log_i("%s\n", __func__);

    while(_exit_flag == 0)
    {
        if((_conn_fd = accept(server_fd, NULL, NULL)) < 0)
        {
            //log_e("%s socket accept failed !!!\n", __func__);
        }
        else
        {
            getpeername(_conn_fd, (struct sockaddr *)&clientaddr, &addrlen);
#if defined(AF_INET6)
            if(inet_ntop(AF_INET6, &clientaddr.sin6_addr, str, sizeof(str)))
            {
                log_i("client addr[%s]port[%d]\n", str, ntohs(clientaddr.sin6_port));
            }
#else
            if(inet_ntop(AF_INET, &clientaddr.sin_addr, str, sizeof(str)))
            {
                log_i("client addr[%s]port[%d]\n", str, ntohs(clientaddr.sin_port));
            }
#endif
            //event_publish(EV_TCP_CLIENT, OP_NONE, (char*)&clientaddr, addrlen); // todo

            pthread_t recv_thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if(pthread_create(&recv_thread, &attr, (void*)tcp_server_recv_proc, (void*)&_conn_fd) < 0)
            {
                log_e("pthread_create failed\n");
                ret = RET_ERR;
            }
            pthread_detach(recv_thread);
        }
    }

    close(server_fd);
    return ret;
}

static int tcp_server_recv_proc(void *arg)
{
    int ret = RET_OK;
    int rc = 0;
    char buffer[2048] = {0,};
    int conn_fd = *(int *)arg;
    //int flag = fcntl(conn_fd, F_GETFL, 0);
    //fcntl(conn_fd, F_SETFL, flag | O_NONBLOCK);
    log_i("%s[%d]\n", __func__, conn_fd);
    while(_exit_flag == 0)
    {
        if(conn_fd > 0)
        {
            rc = recv(conn_fd, buffer, sizeof(buffer), 0);
            if (rc <= 0)
            {
                if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                {
                }
                else
                {
                    log_e("%s errno[%d]\n", __func__, errno);
                    break;
                }
            }
            else if(rc > 0)
            {
                // todo, recv_callback or event_publish
                tcp_server_send(conn_fd, buffer, rc);
            }
            //usleep(1000 * 100);
        }
    }

    log_i("%s[%d] exit\n", __func__, conn_fd);
    close(conn_fd);
    return ret;
}

int tcp_server_send(int fd, char *data, int len)
{
    int ret = RET_OK;
    int rc = 0;
    int sent_len = 0;
    int retry = 0;
    log_i("%s len[%d] data[%s]\n", __func__, len, data);
    while(_exit_flag == 0)
    {
        rc = send(_conn_fd, data + sent_len, len - sent_len, 0);
        if(rc < 0)
        {
           log_e("%s errno[%d]\n", __func__, errno);
           if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
           {
           }
           else
           {
               return RET_ERR;
           }
        }
        else
        {
            sent_len += rc;
            if(sent_len >= len)
            {
                break;
            }
        }

        if(++retry > 3)
        {
            break;
        }
        usleep(1000 * 100);
    }

    return ret;
}

static int tcp_server_destroy(int fd)
{
    int ret = RET_OK;
    log_i("%s\n", __func__);
    close(_conn_fd);
    return ret;
}

static int tcp_server_listen_destroy(int fd)
{
    int ret = RET_OK;
    log_i("%s\n", __func__);
    close(_server_fd);
    return ret;
}

static void tcp_server_state(void)
{
    int prev_st = _tcp_server_st;
    switch(_tcp_server_st)
    {
        case TCP_SERVER_ST_NONE:
            if(config_main_state_get() == 2) // MAIN_ST_PROC)
            {
                _tcp_server_st = TCP_SERVER_ST_START;
            }
            break;
        case TCP_SERVER_ST_START:
            _tcp_server_st = TCP_SERVER_ST_CREATE;
            break;
        case TCP_SERVER_ST_CREATE:
            if(tcp_server_create() == RET_OK)
            {
                _tcp_server_st = TCP_SERVER_ST_PROC;
            }
            else
            {
                usleep(1000 * 1000);
            }
            break;
        case TCP_SERVER_ST_PROC:
            if(_exit_flag != 0)
            {
                _tcp_server_st = TCP_SERVER_ST_DESTORY;
            }
            break;
        case TCP_SERVER_ST_DESTORY:
            tcp_server_destroy(0);
            tcp_server_listen_destroy(0);
            _tcp_server_st = TCP_SERVER_ST_END;
            break;
        case TCP_SERVER_ST_END:
            _tcp_server_st = TCP_SERVER_ST_EXIT;
            break;
        case TCP_SERVER_ST_EXIT:
            break;
        default:
            break;
    }

    if(prev_st != _tcp_server_st)
    {
        if(_tcp_server_st == TCP_SERVER_ST_NONE) log_i("TCP_SERVER_ST_NONE\n");
        else if(_tcp_server_st == TCP_SERVER_ST_START) log_i("TCP_SERVER_ST_START\n");
        else if(_tcp_server_st == TCP_SERVER_ST_CREATE) log_i("TCP_SERVER_ST_CREATE\n");
        else if(_tcp_server_st == TCP_SERVER_ST_PROC) log_i("TCP_SERVER_ST_PROC\n");
        else if(_tcp_server_st == TCP_SERVER_ST_DESTORY) log_i("TCP_SERVER_ST_DESTORY\n");
        else if(_tcp_server_st == TCP_SERVER_ST_END) log_i("TCP_SERVER_ST_END\n");
        else if(_tcp_server_st == TCP_SERVER_ST_EXIT) log_i("TCP_SERVER_ST_EXIT\n");
        else log_i("TCP_SERVER_ST_UNKNOWN\n");
        //config_tcp_server_state_set(_tcp_server_st);
    }
}

static void on_timer(const u32 tid)
{
    switch(tid)
    {
        case TID_HELLO:
            log_i("%s TID_HELLO\n", __func__);
            timer_kill(TID_HELLO);
            break;
        default:
            break;
    }
}

static int tcp_server_selftest(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    timer_set(TID_HELLO, 1000, on_timer);
    return ret_val;
}

static int on_event(const event_data *ev)
{
    int ret_val = RET_OK;
    switch(ev->event)
    {
        case EV_INIT:
            break;
        case EV_DEINIT:
            break;
        case EV_EXIT:
            break;
        case EV_SELFTEST:
            tcp_server_selftest(ev);
            break;
        default:
            break;
    }
    return ret_val;
}

