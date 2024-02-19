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
#include "udp_client.h"

//#undef AF_INET6

#define UDP_LISTEN_PORT         15118

typedef enum udp_client_state_enum
{
    UDP_CLIENT_ST_NONE          = 0,
    UDP_CLIENT_ST_START         = 1,
    UDP_CLIENT_ST_CREATE        = 2,
    UDP_CLIENT_ST_PROC          = 3,
    UDP_CLIENT_ST_DESTORY       = 4,
    UDP_CLIENT_ST_END           = 10,
    UDP_CLIENT_ST_EXIT          = 11
} udp_client_st_e;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _udp_client_st = UDP_CLIENT_ST_NONE;

static int _client_fd = -1;
#if defined(AF_INET6)
static struct sockaddr_in6 _address6;
static int _addrlen = sizeof(_address6);
#else
static struct sockaddr_in _address;
static int _addrlen = sizeof(_address);
#endif

static void udp_client_proc(void);
static void udp_client_state(void);
static int on_event(const event_data *ev);

static int udp_client_listen_proc(void *arg);
static int udp_client_recv_proc(void *arg);

int udp_client_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 0;
    _udp_client_st = UDP_CLIENT_ST_NONE;

    pthread_t udp_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&udp_thread, &attr, (void*)udp_client_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(udp_thread);

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_SELFTEST, on_event);
    event_subscribe(EV_UDP_SEND, on_event);

    return ret_val;
}

int udp_client_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 1;
    return ret_val;
}

static void udp_client_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        udp_client_state();
        usleep(1000 * 100);
    }
}

static int udp_client_create(void)
{
    int ret = RET_OK;
    int opt = 1;
    log_i("%s\n", __func__);

#if defined(AF_INET6)
    _client_fd = socket(AF_INET6, SOCK_DGRAM, 0);
#else
    _client_fd = socket(AF_INET, SOCK_DGRAM, 0);
#endif
    if(_client_fd < 0)
    {
        log_e("%s socket create failed !!!\n", __func__);
        close(_client_fd);
        return RET_ERR;
    }

    if(setsockopt(_client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        log_e("%s socket setsockopt failed !!!\n", __func__);
        close(_client_fd);
        return RET_ERR;
    }

#if defined(AF_INET6)
    memset(&_address6, 0, sizeof(_address6));
    _address6.sin6_family = AF_INET6;
    _address6.sin6_addr = in6addr_loopback;
    _address6.sin6_port = htons(UDP_LISTEN_PORT);
#else
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // inet_addr( "127.0.0.1");
    _address.sin_port = htons(UDP_LISTEN_PORT);
#endif

    pthread_t recv_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&recv_thread, &attr, (void*)udp_client_recv_proc, (void*)&_client_fd) < 0)
    {
        log_e("pthread_create failed\n");
        ret = RET_ERR;
    }
    pthread_detach(recv_thread);

    return ret;
}


static int udp_client_recv_proc(void *arg)
{
    int ret = RET_OK;
    int rc = 0;
    char buffer[2048] = {0,};
    int conn_fd = *(int *)arg;
    log_i("%s[%d]\n", __func__, conn_fd);
    while(_exit_flag == 0)
    {
        if(conn_fd > 0)
        {
#if defined(AF_INET6)
            rc = recvfrom(conn_fd, buffer, sizeof(buffer), 0,
                            (struct sockaddr*)&_address6, &_addrlen);
#else
            rc = recvfrom(conn_fd, buffer, sizeof(buffer), 0,
                            (struct sockaddr*)&_address, &_addrlen);
#endif
            if (rc <= 0)
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
                int cnt = rc / EV_DATA_SZ;
                int frag = rc % EV_DATA_SZ;
                int i = 0;
                for(i = 0; i <= cnt; i++)
                {
                    if((cnt == 0 || cnt == i) && (frag > 0))
                    {
                        event_publish(EV_EVCC_UDP_RECV, OP_NONE, buffer + (i * EV_DATA_SZ), frag);
                    }
                    else
                    {
                        event_publish(EV_EVCC_UDP_RECV, OP_NONE, buffer + (i * EV_DATA_SZ), EV_DATA_SZ);
                    }
                }
            }
        }
    }

    log_i("%s[%d] exit\n", __func__, conn_fd);
    close(conn_fd);
    return ret;
}

int udp_client_send(int fd, char *data, int len)
{
    int ret = RET_OK;
    int rc = 0;
    int sent_len = 0;
    int retry = 0;
    log_i("%s len[%d] data[%s]\n", __func__, len, data);
    while(_exit_flag == 0)
    {
#if defined(AF_INET6)
        rc = sendto(_client_fd, data + sent_len, len - sent_len, 0,
                        (struct sockaddr*)&_address6, sizeof(_address6));
#else
        rc = sendto(_client_fd, data + sent_len, len - sent_len, 0,
                        (struct sockaddr*)&_address, sizeof(_address));
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

static int udp_client_destroy(int fd)
{
    int ret = RET_OK;
    log_i("%s\n", __func__);
    close(_client_fd);
    return ret;
}

static void udp_client_state(void)
{
    int prev_st = _udp_client_st;
    switch(_udp_client_st)
    {
        case UDP_CLIENT_ST_NONE:
            if(config_main_state_get() == 2) // MAIN_ST_PROC)
            {
                _udp_client_st = UDP_CLIENT_ST_START;
            }
            break;
        case UDP_CLIENT_ST_START:
            _udp_client_st = UDP_CLIENT_ST_CREATE;
            break;
        case UDP_CLIENT_ST_CREATE:
            if(udp_client_create() == RET_OK)
            {
                _udp_client_st = UDP_CLIENT_ST_PROC;
            }
            else
            {
                usleep(1000 * 1000);
            }
            break;
        case UDP_CLIENT_ST_PROC:
            if(_exit_flag != 0)
            {
                _udp_client_st = UDP_CLIENT_ST_DESTORY;
            }
            break;
        case UDP_CLIENT_ST_DESTORY:
            udp_client_destroy(0);
            _udp_client_st = UDP_CLIENT_ST_END;
            break;
        case UDP_CLIENT_ST_END:
            _udp_client_st = UDP_CLIENT_ST_EXIT;
            break;
        case UDP_CLIENT_ST_EXIT:
            break;
        default:
            break;
    }

    if(prev_st != _udp_client_st)
    {
        if(_udp_client_st == UDP_CLIENT_ST_NONE) log_i("UDP_CLIENT_ST_NONE\n");
        else if(_udp_client_st == UDP_CLIENT_ST_START) log_i("UDP_CLIENT_ST_START\n");
        else if(_udp_client_st == UDP_CLIENT_ST_CREATE) log_i("UDP_CLIENT_ST_CREATE\n");
        else if(_udp_client_st == UDP_CLIENT_ST_PROC) log_i("UDP_CLIENT_ST_PROC\n");
        else if(_udp_client_st == UDP_CLIENT_ST_DESTORY) log_i("UDP_CLIENT_ST_DESTORY\n");
        else if(_udp_client_st == UDP_CLIENT_ST_END) log_i("UDP_CLIENT_ST_END\n");
        else if(_udp_client_st == UDP_CLIENT_ST_EXIT) log_i("UDP_CLIENT_ST_EXIT\n");
        else log_i("UDP_CLIENT_ST_UNKNOWN\n");
        //config_udp_client_state_set(_UDP_client_st);
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

static int udp_client_selftest(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
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
            udp_client_selftest(ev);
            break;
        case EV_UDP_SEND:
            udp_client_send(_client_fd, "hello_world!!!", strlen("hello_world!!!"));
            break;
        default:
            break;
    }
    return ret_val;
}

