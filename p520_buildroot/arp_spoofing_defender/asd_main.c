#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>

#include "asd_dbg.h"
#include "asd_main.h"

#define SIN_ADDR(x)	(((struct sockaddr_in *) (&(x)))->sin_addr.s_addr)
#define SET_SA_FAMILY(addr, family) \
    memset ((char *) &(addr), '\0', sizeof(addr)); \
    addr.sa_family = (family);

struct dyn_lease {
    uint32_t expires;
    uint32_t ip; /*IP in network order */
    uint8_t mac[6];
    char hostname[20];
    uint8_t pad[2];
};

struct chilli_lease {
    uint32_t ip;
    uint8_t mac[6];
    uint8_t pad[2];
};

typedef struct _ether_arp_frame { 
  struct ether_header ether_hdr;
  struct ether_arp arp;
} __attribute__ ((packed)) ether_arp_frame;

static pthread_t        asd_thread = -1;
static pthread_mutex_t  asd_mutex = PTHREAD_MUTEX_INITIALIZER;

static char             asd_shutdown = 0;
static char             asd_update_table = 0;
static unsigned int     asd_interval = 100;
static int              asd_dhcp_type = 0;

static void sig_cb(int sig)
{
    if(sig == SIGUSR1)
    {
        log_print(LOG_I, "sig[%d] \n", sig);
        pthread_mutex_lock(&asd_mutex);
        asd_update_table = 1;
        pthread_mutex_unlock(&asd_mutex);
    }
    else
    {
        log_print(LOG_E, "sig[%d] \n", sig);
        asd_shutdown = 1;
    }
}

static int update_arp_table(char static_flag)
{
    int result = 0;
    int arpsock = -1;
    FILE *fp;
    uint64_t written_at;
    struct dyn_lease lease;
    struct arpreq req;

    log_print(LOG_D, "arp cache update [%d]\n", static_flag);
    arpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (arpsock < 0)
    {
        log_print(LOG_E, "Couldn't create IP socket: %m(%d)", errno);
        return result;
    }

    fp = fopen("/var/udhcpd.leases", "r");
    if(fp == NULL)
    {
        log_print(LOG_E, "failed to open /var/udhcpd.leases\n");
        close(arpsock);
        return result;
    }

    fread(&written_at, 1, sizeof(written_at), fp);
    while(fread(&lease, 1, sizeof(lease), fp) == sizeof(lease))
    {
        log_print(LOG_D, "%d.%d.%d.%d\n", lease.ip&0xFF,
                                        (lease.ip>>8)&0xFF,
                                        (lease.ip>>16)&0xFF,
                                        (lease.ip>>24)&0xFF);
        log_print(LOG_D, "%02X:%02X:%02X:%02X:%02X:%02X\n",
                   lease.mac[0], lease.mac[1], lease.mac[2],
                   lease.mac[3], lease.mac[4], lease.mac[5]);

        memset(&req, 0, sizeof(req));
        req.arp_ha.sa_family = ARPHRD_ETHER;
        SET_SA_FAMILY(req.arp_pa, AF_INET);
        SIN_ADDR(req.arp_pa) = lease.ip;
        memcpy(req.arp_ha.sa_data, lease.mac, 6);
        if(static_flag) // permanent entry
            req.arp_flags = ATF_COM | ATF_PERM;
        else
            req.arp_flags = ATF_COM;

        // Delete the proxy ARP entry
        if(ioctl(arpsock, SIOCDARP, (caddr_t)&req) < 0)
        {
            log_print(LOG_W, "ioctl(SIOCDARP): %d\n", errno);
        }

        // Make a proxy ARP entry
        if(ioctl(arpsock, SIOCSARP, (caddr_t)&req) < 0)
        {
            log_print(LOG_W, "ioctl(SIOCSARP): %d\n", errno);
        }
    }

    close(arpsock);
    fclose(fp);
    return result;
}

#if 0
static int chilli_update_arp_table(char static_flag)
{
    int result = 0;
    int arpsock = -1;
    FILE *fp;
    struct chilli_lease lease;
    struct arpreq req;

    log_print(LOG_D, "arp cache update [%d]\n", static_flag);
    arpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (arpsock < 0)
    {
        log_print(LOG_E, "Couldn't create IP socket: %m(%d)", errno);
        return result;
    }

    fp = fopen("/var/chilli.leases", "r");
    if(fp == NULL)
    {
        log_print(LOG_E, "failed to open /var/chilli.leases\n");
        close(arpsock);
        return result;
    }

    while(fread(&lease, 1, sizeof(lease), fp) == sizeof(lease))
    {
        log_print(LOG_D, "%d.%d.%d.%d\n", lease.ip&0xFF,
                                        (lease.ip>>8)&0xFF,
                                        (lease.ip>>16)&0xFF,
                                        (lease.ip>>24)&0xFF);
        log_print(LOG_D, "%02X:%02X:%02X:%02X:%02X:%02X\n",
                   lease.mac[0], lease.mac[1], lease.mac[2],
                   lease.mac[3], lease.mac[4], lease.mac[5]);

        memset(&req, 0, sizeof(req));
        req.arp_ha.sa_family = ARPHRD_ETHER;
        SET_SA_FAMILY(req.arp_pa, AF_INET);
        SIN_ADDR(req.arp_pa) = lease.ip;
        memcpy(req.arp_ha.sa_data, lease.mac, 6);
        if(static_flag) // permanent entry
            req.arp_flags = ATF_COM | ATF_PERM;
        else
            req.arp_flags = ATF_COM;

        // Delete the proxy ARP entry
        if(ioctl(arpsock, SIOCDARP, (caddr_t)&req) < 0)
        {
            log_print(LOG_W, "ioctl(SIOCDARP): %d\n", errno);
        }

        // Make a proxy ARP entry
        if(ioctl(arpsock, SIOCSARP, (caddr_t)&req) < 0)
        {
            log_print(LOG_W, "ioctl(SIOCSARP): %d\n", errno);
        }
    }

    close(arpsock);
    fclose(fp);
    return result;
}
#endif

static void *main_proc(void)
{
    int sock = 0;
    ether_arp_frame reqframe;
    struct ether_arp *arp = &reqframe.arp;
    struct sockaddr_ll ifs;
    struct ifreq ifr;
    struct sockaddr_in *sin;
    unsigned long ifaddr;

    log_print(LOG_I, "asd_interval %d msec \n", asd_interval);
    if (asd_dhcp_type == 0) {
        update_arp_table(1);
    } else {
        //chilli_update_arp_table(1);
    }

    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0)
    {
        log_print(LOG_E, "Couldn't create ARP socket: %m(%d)", errno);
        abort();
    }

    memset(ifr.ifr_name, 0, IFNAMSIZ);

    strncpy(ifr.ifr_name, (char *)"br0", IFNAMSIZ);

    // read MAC
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
    {
        log_print(LOG_E, "error ioctl SIOCGIFHWADDR br0: %s\n", strerror(errno));
        close(sock);
        abort();
    }
    memset(ifs.sll_addr, 0, ETH_ALEN);
    memcpy(ifs.sll_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    // read interface index
    if(ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        log_print(LOG_E, "error ioctl SIOCGIFINDEX br0: %s", strerror(errno));
        close(sock);
        abort();
    }

    ifs.sll_family = AF_PACKET;
    ifs.sll_protocol = htons(ETH_P_ARP);
    ifs.sll_ifindex = ifr.ifr_ifindex;
    ifs.sll_hatype = ARPHRD_ETHER;
    ifs.sll_pkttype = PACKET_BROADCAST;
    ifs.sll_halen = ETH_ALEN;

    if (asd_dhcp_type != 0) {
        strncpy(ifr.ifr_name, (char *)"tun1", IFNAMSIZ);
    }

    // read interface ip addr
    if(ioctl(sock, SIOCGIFADDR, &ifr) == 0)
    {
        sin = (struct sockaddr_in *) &ifr.ifr_addr;
        ifaddr = sin->sin_addr.s_addr;
    }
    else
    {
        log_print(LOG_E, "error ioctl SIOCGIFADDR br0: %s", strerror(errno));
        close(sock);
        abort();
    }

    memset(&reqframe.ether_hdr.ether_dhost, 0xFF, ETH_ALEN);
    memcpy(&reqframe.ether_hdr.ether_shost, ifs.sll_addr, ETH_ALEN);
    reqframe.ether_hdr.ether_type = htons(ETHERTYPE_ARP);

    // make arp reply frame
    arp->arp_hrd = htons(ARPHRD_ETHER);
    arp->arp_pro = htons(ETH_P_IP);
    arp->arp_hln = 6;
    arp->arp_pln = 4;
    memset(&arp->arp_tha, 0xFF, ETH_ALEN);
    memcpy(&arp->arp_sha, ifs.sll_addr, ETH_ALEN);

    memset(&arp->arp_tpa, 0xFF, 4);
    memcpy(&arp->arp_spa, &ifaddr, 4);

    arp->arp_op = htons(ARPOP_REPLY);

    while(1)
    {
        if(asd_shutdown)
        {
            log_print(LOG_E, "asd_shutdown[%d] \n", asd_shutdown);
            close(sock);
            pthread_exit(0);
        }

        pthread_mutex_lock(&asd_mutex);
        if(asd_update_table == 1)
        {
            if (asd_dhcp_type == 0) {
                update_arp_table(1);
            } else {
                //chilli_update_arp_table(1);
            }
            asd_update_table = 0;
        }
        pthread_mutex_unlock(&asd_mutex);

        sendto(sock, (char*)&reqframe, sizeof(ether_arp_frame), 0,
           (struct sockaddr *)&ifs, sizeof(struct sockaddr_ll));
        log_print(LOG_V, "arp reply send\n");
        usleep(asd_interval * 1000); // msec
    }

    close(sock);
    return (void*)0;
}

static void print_help(void)
{
    printf("\n");
    printf("************************************************\n");
    printf("arp_spoofing_defender <log_level> <arp interval> <dhcp_server_type> \n");
    printf("  <log_level> : -V, -D, -I, -W, -E, -B \n");
    printf("  <arp_interval> : 10 ~ 1000 msec (default 100) \n");
    printf("  <dhcp_server_type> : 0 (dhcpd), 1 (chilli) \n");
    printf("(example) arp_spoofing_defender -E 100 0 &\n");
    printf("************************************************\n");
}

static void arg_parser(int argc, char **argv, log_level_e *log_level)
{
    if(argc != 4)
    {
        print_help();
        exit(0);
    }

    if(strncmp((char*)argv[1], (char*)"-N", strlen("-N")) == 0)
    {
        *log_level = LOG_NONE;
    }
    else if(strncmp((char*)argv[1], (char*)"-V", strlen("-V")) == 0)
    {
        *log_level = LOG_V;
    }
    else if (strncmp((char*)argv[1], (char*)"-D", strlen("-D")) == 0)
    {
        *log_level = LOG_D;
    }
    else if (strncmp((char*)argv[1], (char*)"-I", strlen("-I")) == 0)
    {
        *log_level = LOG_I;
    }
    else if (strncmp((char*)argv[1], (char*)"-W", strlen("-W")) == 0)
    {
        *log_level = LOG_W;
    }
    else if (strncmp((char*)argv[1], (char*)"-E", strlen("-E")) == 0)
    {
        *log_level = LOG_E;
    }
    else if (strncmp((char*)argv[1], (char*)"-B", strlen("-B")) == 0)
    {
        *log_level = LOG_I;
        sleep(30); // when boot wait for done
    }
    else
    {
        print_help();
        exit(0);
    }

    asd_interval = atoi((char*)argv[2]);
    if(asd_interval < 10 || asd_interval > 1000)
    {
        asd_interval = 100;
    }

    if (strcmp(argv[3], "0") == 0) {
        asd_dhcp_type = 0;
    } else {
        asd_dhcp_type = 1;
    }

    printf("arp_spoofing_defender log_level[%d] interval[%d]\n", *log_level, asd_interval);
}

int main(int argc, char **argv)
{
    int result = 0;
    pthread_attr_t attr;
    log_level_e log_level = LOG_I;

    signal(SIGINT, sig_cb);
    signal(SIGTERM, sig_cb);
    signal(SIGHUP, sig_cb);
    signal(SIGABRT, sig_cb);
    signal(SIGUSR1, sig_cb); // from udhcp

    arg_parser(argc, argv, &log_level);
    debug_init(DBG_ENABLE, log_level);
    log_print(log_level, "asd daemon start\n");

    pthread_attr_init(&attr);
    if(pthread_create(&asd_thread, &attr, (void*)main_proc, NULL) < 0)
    {
        log_print(LOG_E, "pthread_create failed \n");
        exit(0);
    }

    result = pthread_join(asd_thread, (void*)&result);
    if (asd_dhcp_type == 0) {
        update_arp_table(0);
    } else {
        //chilli_update_arp_table(0);
    }

    log_print(log_level, "asd daemon exit (%d)\n", result);
    return result;
}

