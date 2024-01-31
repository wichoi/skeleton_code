#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <string.h>

#include "EXITypes.h"

#include "appHandEXIDatatypes.h"
#include "appHandEXIDatatypesEncoder.h"
#include "appHandEXIDatatypesDecoder.h"

/* Activate support for DIN */
#include "dinEXIDatatypes.h"
#if DEPLOY_DIN_CODEC == SUPPORT_YES
#include "dinEXIDatatypesEncoder.h"
#include "dinEXIDatatypesDecoder.h"
#endif /* DEPLOY_DIN_CODEC == SUPPORT_YES */

/* Activate support for XMLDSIG */
#include "xmldsigEXIDatatypes.h"
#if DEPLOY_XMLDSIG_CODEC == SUPPORT_YES
#include "xmldsigEXIDatatypesEncoder.h"
#include "xmldsigEXIDatatypesDecoder.h"
#endif /* DEPLOY_XMLDSIG_CODEC == SUPPORT_YES */

/* Activate support for ISO1 */
#include "iso1EXIDatatypes.h"
#if DEPLOY_ISO1_CODEC == SUPPORT_YES
#include "iso1EXIDatatypesEncoder.h"
#include "iso1EXIDatatypesDecoder.h"
#endif /* DEPLOY_ISO1_CODEC == SUPPORT_YES */

/* Activate support for ISO2 */
#include "iso2EXIDatatypes.h"
#if DEPLOY_ISO2_CODEC == SUPPORT_YES
#include "iso2EXIDatatypesEncoder.h"
#include "iso2EXIDatatypesDecoder.h"
#endif /* DEPLOY_ISO2_CODEC == SUPPORT_YES */

#include "v2gtp.h"

#include "common.h"
#include "log_service.h"
#include "config_manager.h"

int evcc_msg_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_supported_app_protocol(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_session_setup(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_service_discovery(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_service_detail(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_payment_selection(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_payment_detail(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_authorization(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_charging_parameter(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_power_delivery(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_certificate_update(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_certificate_installation(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_session_stop(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_charging_status(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_metering_receipt(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_cable_check(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_pre_charge(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_current_demand(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int evcc_msg_welding_detection(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

