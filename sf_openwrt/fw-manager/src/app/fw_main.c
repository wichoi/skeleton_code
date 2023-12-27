#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "fw_main.h"
#include "fw_dbg.h"
#include "fw_manager.h"
#include "fw_crypto.h"

enum FW_COMMAND_ID
{
    FW_MERGE = 0,
    FW_MERGE_CRYPT = 1,
    FW_SPLITE,
    SHOW_HELP,
    MAX_COMMAND_COUNT,
};

void ShowHelpMessage()
{
    printf("\r\n=======================================================================");
    printf("\r\n fw_manager {merge, splite, help} ..");
    printf("\r\n    - merge            : Multiple firmware is merged into one firmware.");
    printf("\r\n      fw_manager merge [ProductFirmwareName] [FW1] .. [FW10]");
    printf("\r\n");
    printf("\r\n    - crypto           : Multiple crypto firmware is merged into one firmware.");
    printf("\r\n      fw_manager crypto [ProductFirmwareName] [FW1] .. [FW10]");
    printf("\r\n");
    printf("\r\n    - splite           : Split into multiple firmware.");
    printf("\r\n      fw_manager splite [ProductFirmwareName] [Dest Path]");
    printf("\r\n");
    printf("\r\n=======================================================================\r\n");
}

int ProcessCommand(char *strCMD)
{
    static int            s_nMaxCMDCount    = MAX_COMMAND_COUNT;
    static const char*    s_aCommand[]      = {
        "merge",
        "crypto",
        "splite",
        "help",
    };
    int nID = 0;

    for (nID = 0; nID < s_nMaxCMDCount; nID++)
    {
        if(!strcmp(strCMD, s_aCommand[nID]))
        {
            log_print(LOG_I, "cmd[%s] \n", strCMD);
            return nID;
        }
    }

    log_print(LOG_I, "invalid command !!! \n");
    return -1;
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        ShowHelpMessage();
        return -1;
    }

    debug_init(DBG_ENABLE, LOG_I);

    //for (int nIndex = 0; nIndex < argc; nIndex++)
    //    printf("[%d] %s\n", nIndex, argv[nIndex]);

    if (argc >= 2)
    {
        char *strCMD = argv[1];
        int nStartIndex = 0;
        int nID = ProcessCommand(strCMD);

        switch (nID)
        {
        case FW_MERGE:
            {
                int nIndex;
                if (nID == -1)
                    nStartIndex = 1;
                else
                    nStartIndex = 2;

                fw_Init();

                for (nIndex = nStartIndex + 1; nIndex < argc; nIndex++)
                {
                    log_print(LOG_V, "argv [%s] \n", argv[nIndex]);
                    fw_Append(argv[nIndex]);
                }

                if(fw_Merge(argv[nStartIndex], FW_FILE_NORMAL) == -1)
                {
                    log_print(LOG_E, "firmware merge failed !!! \n");
                    return -1;
                }
                log_print(LOG_I, "skeleton firmware created! [%s]\n", argv[nStartIndex]);
            }
            break;

        case FW_MERGE_CRYPT:
            {
                int nIndex;
                if (nID == -1)
                    nStartIndex = 1;
                else
                    nStartIndex = 2;

                fw_Init();

                for (nIndex = nStartIndex + 1; nIndex < argc; nIndex++)
                {
                    char base_encryption[128] = {0,};
                    sprintf(base_encryption, "%s@crypto", argv[nIndex]);
                    if(AproEncryptAes((const char*)argv[nIndex], (const char*)base_encryption) == false)
                    {
                        printf("\r\n Base firmware crypto failed !!! \r\n");
                        break;
                    }
                    fw_Append(base_encryption);
                }

                if(fw_Merge(argv[nStartIndex], FW_FILE_CRYPTION) == -1)
                {
                    log_print(LOG_E, "firmware merge failed !!! \n");
                    return -1;
                }
                log_print(LOG_I, "skeleton firmware created! [%s]\n", argv[nStartIndex]);
            }
            break;

        case FW_SPLITE:
            {
                char fw_path[128] ={0x0,};
                if (argc >= 4)
                {
                    snprintf(fw_path, 128 - 1, "%s", argv[3]);
                    if(fw_path[strlen(fw_path) - 1] != '/' || fw_path[strlen(fw_path) - 1] != '\\')
                        fw_path[strlen(fw_path)] = '/';
                }
                log_print(LOG_D, "splite path [%s]\n", fw_path);
                fw_sp_init(fw_path);
                if(fw_Splite(argv[2], 0) == false)
                {
                    log_print(LOG_E, "firmware splite failed \n");
                    return -1;
                }
                log_print(LOG_I, "skeleton Firmware Splite Done !!! [%s]\n", argv[2]);
            }
            break;

        case SHOW_HELP:
            ShowHelpMessage();
            break;
        }
    }

    printf("\r\n");

    return 0;
}

