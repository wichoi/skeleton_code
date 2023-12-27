#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// grpc-1.34\third_party\boringssl-with-bazel\src\include\openssl
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/err.h>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "cert-manager.h"

cert_manager::cert_manager()
{
}

cert_manager::~cert_manager()
{
    log_d("%s\n", __func__);
}

int cert_manager::init(void)
{
    int ret_val = RET_OK;
    log_d("cert_manager::%s \n", __func__);
    return ret_val;
}

int cert_manager::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}

