#ifndef __ROUTER_UPGRADE_H__
#define __ROUTER_UPGRADE_H__

#ifdef __cplusplus
extern "C" {
#endif

int routerFwUpgrade(char *filename, int file_begin, int file_end, char *err_msg);

#ifdef __cplusplus
}
#endif

#endif
