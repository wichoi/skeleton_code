#ifndef __MODEM_FW_UPGRADE__
#define __MODEM_FW_UPGRADE__

#ifdef __cplusplus
extern "C" {
#endif

int modemFwUpgrade(char *filename, int file_begin, int file_end, char *err_msg);

#ifdef __cplusplus
}
#endif

#endif
