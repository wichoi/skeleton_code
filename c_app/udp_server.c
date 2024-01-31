#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"
#include "udp_server.h"

//#undef AF_INET6

#define UDP_LISTEN_PORT         15118

typedef enum udp_server_state_enum
{
    UDP_SERVER_ST_NONE          = 0,
    UDP_SERVER_ST_START         = 1,
    UDP_SERVER_ST_CREATE        = 2,
    UDP_SERVER_ST_PROC          = 3,
    UDP_SERVER_ST_DESTORY       = 4,
    UDP_SERVER_ST_END           = 10,
    UDP_SERVER_ST_EXIT          = 11
} udp_server_st_e;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _udp_server_st = UDP_SERVER_ST_NONE;

static int _server_fd = -1;
#if defined(AF_INET6)
static struct sockaddr_in6 _address6;
static int _address6_size = sizeof(_address6);
#else
static struct sockaddr_in _client_addr;
static int _client_addr_size = sizeof(_client_addr);
#endif



static void udp_server_proc(void);
static void udp_server_state(void);
static int on_event(const event_data *ev);

static int udp_server_listen_proc(void *arg);
static int udp_server_recv_proc(void *arg);

int udp_server_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 0;
    _udp_server_st = UDP_SERVER_ST_NONE;

    pthread_t udp_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&udp_thread, &attr, (void*)udp_server_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(udp_thread);

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_SELFTEST, on_event);

    return ret_val;
}

int udp_server_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 1;
    return ret_val;
}

static void udp_server_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        udp_server_state();
        usleep(1000 * 100);
    }
}

static int udp_server_create(void)
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
    _server_fd = socket(AF_INET6, SOCK_DGRAM, 0);
#else
    _server_fd = socket(AF_INET, SOCK_DGRAM, 0);
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
    address6.sin6_port = htons(UDP_LISTEN_PORT);
    if(bind(_server_fd, (struct sockaddr *)&address6, sizeof(address6)) < 0)
    {
        log_e("%s socket bind failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }
#else
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(UDP_LISTEN_PORT);
    if(bind(_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        log_e("%s socket bind failed !!!\n", __func__);
        close(_server_fd);
        return RET_ERR;
    }
#endif

    pthread_t recv_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&recv_thread, &attr, (void*)udp_server_recv_proc, (void*)&_server_fd) < 0)
    {
        log_e("pthread_create failed\n");
        ret = RET_ERR;
    }
    pthread_detach(recv_thread);

    return ret;
}

static int udp_server_recv_proc(void *arg)
{
    int ret = RET_OK;
    int rc = 0;
    char buffer[2048] = {0,};
    int conn_fd = *(int *)arg;

    log_i("%s\n", __func__);
    while(_exit_flag == 0)
    {
        if(conn_fd > 0)
        {
#if defined(AF_INET6)
            rc = recvfrom(conn_fd, buffer, sizeof(buffer), 0,
                        (struct sockaddr*)&_address6, &_address6_size);
#else
            rc = recvfrom(conn_fd, buffer, sizeof(buffer), 0,
                        (struct sockaddr*)&_client_addr, &_client_addr_size);
#endif
            if (rc < 0)
            {
                log_e("%s errno[%d]\n", __func__, errno);
                if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    continue; /* restart for EINTR */
                }
                else
                {
                    /* if 0, disconnect, otherwise error */
                    break;
                }
            }
            else if(rc > 0)
            {
                // todo, recv_callback or event_publish
                udp_server_send(conn_fd, buffer, rc);
            }
        }
    }

    close(conn_fd);
    return ret;
}

int udp_server_send(int fd, char *data, int len)
{
    int ret = RET_OK;
    int rc = 0;
    int sent_len = 0;
    int retry = 0;
    log_i("%s len[%d] data[%s]\n", __func__, len, data);
    while(_exit_flag == 0)
    {
#if defined(AF_INET6)
        rc = sendto(_server_fd, data + sent_len, len - sent_len, 0,
                    (struct sockaddr*)&_address6, sizeof(_address6));
#else
        rc = sendto(_server_fd, data + sent_len, len - sent_len, 0,
                    (struct sockaddr*)&_client_addr, sizeof(_client_addr));
#endif
        if(rc < 0)
        {
           log_e("%s errno[%d]\n", __func__, errno);
           if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
           {
               continue; /* restart for EINTR */
           }
           else
           {
               return RET_ERR; /* if 0, disconnect, otherwise error */
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

static int udp_server_destroy(int fd)
{
    int ret = RET_OK;
    log_i("%s\n", __func__);
    close(_server_fd);
    return ret;
}

static void udp_server_state(void)
{
    int prev_st = _udp_server_st;
    switch(_udp_server_st)
    {
        case UDP_SERVER_ST_NONE:
            if(config_main_state_get() == 2) // MAIN_ST_PROC)
            {
                _udp_server_st = UDP_SERVER_ST_START;
            }
            break;
        case UDP_SERVER_ST_START:
            _udp_server_st = UDP_SERVER_ST_CREATE;
            break;
        case UDP_SERVER_ST_CREATE:
            if(udp_server_create() == RET_OK)
            {
                _udp_server_st = UDP_SERVER_ST_PROC;
            }
            else
            {
                usleep(1000 * 1000);
            }
            break;
        case UDP_SERVER_ST_PROC:
            if(_exit_flag != 0)
            {
                _udp_server_st = UDP_SERVER_ST_DESTORY;
            }
            break;
        case UDP_SERVER_ST_DESTORY:
            udp_server_destroy(0);
            _udp_server_st = UDP_SERVER_ST_END;
            break;
        case UDP_SERVER_ST_END:
            _udp_server_st = UDP_SERVER_ST_EXIT;
            break;
        case UDP_SERVER_ST_EXIT:
            break;
        default:
            break;
    }

    if(prev_st != _udp_server_st)
    {
        if(_udp_server_st == UDP_SERVER_ST_NONE) log_i("UDP_SERVER_ST_NONE\n");
        else if(_udp_server_st == UDP_SERVER_ST_START) log_i("UDP_SERVER_ST_START\n");
        else if(_udp_server_st == UDP_SERVER_ST_CREATE) log_i("UDP_SERVER_ST_CREATE\n");
        else if(_udp_server_st == UDP_SERVER_ST_PROC) log_i("UDP_SERVER_ST_PROC\n");
        else if(_udp_server_st == UDP_SERVER_ST_DESTORY) log_i("UDP_SERVER_ST_DESTORY\n");
        else if(_udp_server_st == UDP_SERVER_ST_END) log_i("UDP_SERVER_ST_END\n");
        else if(_udp_server_st == UDP_SERVER_ST_EXIT) log_i("UDP_SERVER_ST_EXIT\n");
        else log_i("UDP_SERVER_ST_UNKNOWN\n");
        //config_udp_server_state_set(_udp_server_st);
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

static int udp_server_selftest(const event_data *ev)
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
            udp_server_selftest(ev);
            break;
        default:
            break;
    }
    return ret_val;
}

