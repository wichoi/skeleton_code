#include <stdio.h>

#if 0
#include "FSPCoreAPI/api/FSPTypes.h"
#include "FSPCoreAPI/api/MSString.h"
#include "FSPCoreAPI/api/FSPDebug.h"
#include "OcfBridgeAPI/api/OcfBridge.h"
#endif

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-ocf.h"
#include "apro-ocf-parser.h"

#if 0
CMSSmartPtrT<COcfBridge> _bridge;
typedef CMSSmartPtrT<COCFDevice> COCFDevice_T;
CFSPRefObjVectorT<COCFDevice_T, COCFDevice_T&> _dev_list;

class IPropChange
    :public IResourcePropChange
{
public:
    IPropChange()
    {
    }
    EtBOOL OnPropChanged(COCFDevice * dev, COCFResource * res, COCFProp * prop, OCFValue_t * value)
    {
        printf("%s ocf_id[%u] dev_type[%u] data_type[%u]\n",
                __func__, dev->_ocf_dev_id, dev->_type, value->GetType());
        EtBOOL ret = EtTRUE;
        ocf_recv_t frame = {0,};
        memset((char*)&frame, 0, sizeof(frame));
#if 1 // wichoi 20201120 RS Demo
        int i = 0;
        int res_cnt = 1; // todo
        for(i = 0; i < res_cnt; i++)
        {
            frame.data[i].ocf_id = dev->_ocf_dev_id;
            frame.data[i].type = (u8)dev->_type;

            switch(value->GetType())
            {
            case VAL_Boolean:
                memcpy((char*)frame.data[i].value, (char*)&value->_b_val, sizeof(value->_b_val));
                break;

            case VAL_Integer:
                memcpy((char*)frame.data[i].value, (char*)&value->_n_val, sizeof(value->_n_val));
                break;

            case VAL_Double:
                memcpy((char*)frame.data[i].value, (char*)&value->_db_val, sizeof(value->_db_val));
                break;

            case VAL_String:
                snprintf((char*)frame.data[i].value, sizeof(frame.data[0].value) - 1, "%s", value->_str_val.c_str());
                break;

            case VAL_Array:
                break;

            default:
                break;
            }
            frame.cnt++;
        }
#endif
        apro_ocf_put_event((u8*)&frame, sizeof(frame));
        return ret;
    }
};

IPropChange _bridge_if;
#endif

int apro_ocf_init(void)
{
    printf("%s\n", __func__);
    return RET_SUCCESS;
}

int apro_ocf_deinit(void)
{
    printf("%s\n", __func__);
#if 0
    if(_bridge != NULL)
    {
        _dev_list.Clear();
        _bridge->Finalize();
        _bridge.Detach();
        _bridge = NULL;
    }
#endif
    return RET_SUCCESS;
}

int apro_ocf_create(void)
{
#if 0
    if(_bridge == NULL)
    {
        _bridge.Attach(CreateOcfBridge(COcfBridge::BR_ZIGBEE));
        if(_bridge->Initialize(&_bridge_if) == EtFALSE)
        {
            printf("error int \n");
            _bridge.Detach();
            _bridge = NULL;
            return RET_ERROR;
        }
    }

    printf("%s SUCCESS !!!\n", __func__);
#endif
    return RET_SUCCESS;
}

int apro_ocf_add_dev(t_node_mgr *mgr)
{
    int ret_val = RET_ERROR;
#if 0
    u32 dev_type = COCFDevice::OD_Unknown;
    int i = 0;
    for(i = 0; i < APRO_NODE_MAX; i++)
    {
        if(mgr->node[i].net_id != 0)
        {
            int j = 0;
            for(j = 0; j < APRO_MAX_EP; j++)
            {
                if(mgr->node[i].ep[j].ep_id != 0)
                {
                    int k = 0;
                    for(k = 0; k < APRO_CLUSTER_MAX; k++)
                    {
                        switch(mgr->node[i].ep[j].cl[k].id)
                        {
                        case 0x0006: // ZCL_ON_OFF_CLUSTER_ID
                            dev_type = COCFDevice::OD_Light;
                            break;

                        case 0x0402: // ZCL_TEMP_MEASUREMENT_CLUSTER_ID
                            dev_type = COCFDevice::OD_Tms;
                            break;

                        case 0x0500: // ZCL_IAS_ZONE_CLUSTER_ID
                            dev_type = COCFDevice::OD_Motion;
                            break;
                        }

                        if(dev_type != COCFDevice::OD_Unknown)
                        {
                            break;
                        }
                    }
                }

                if(dev_type != COCFDevice::OD_Unknown)
                {
                    break;
                }
            }

            if(dev_type != COCFDevice::OD_Unknown)
            {
                CMSSmartPtrT<COCFDevice> device;
                if(_bridge->CreateDevice(&device, (COCFDevice::OD_TYPE_e)dev_type) == EtFALSE)
                {
                    printf("error create device \n");
                    _dev_list.Clear();
                    _bridge->Finalize();
                    _bridge.Detach();
                    _bridge = NULL;
                    return RET_ERROR;
                }
                printf("ocf device create %d !!!\n", dev_type);
                _dev_list.Add(device);
                apro_node_ocf_set(mgr->node[i].net_id, device->_ocf_dev_id, dev_type);
                dev_type = COCFDevice::OD_Unknown;
                ret_val = RET_SUCCESS;
            }
        }
    }
#endif
    printf("%s result[%d]\n", __func__, ret_val);
    return ret_val;
}

int apro_ocf_start(void)
{
    printf("%s\n", __func__);
#if 0
    if(_bridge->Start() == EtFALSE)
    {
        printf("error start \n");
        _bridge->Finalize();
        _bridge.Detach();
        _bridge = NULL;
        return RET_ERROR;
    }
#endif
    return RET_SUCCESS;
}

int apro_ocf_stop(void)
{
#if 0
    printf("%s\n", __func__);
    if(_bridge->Stop() == EtFALSE)
    {
        printf("error stop \n");
    }
#endif
    return RET_SUCCESS;
}

int apro_ocf_send(char *data, u32 len)
{
#if 0
    //printf("%s\n", __func__);
    ocf_send_t *ocf_pay = (ocf_send_t*)data;
    u32 ocf_id = 0;
    u32 ocf_type = 0;
    apro_node_ocf_get(ocf_pay->net_id, &ocf_id, &ocf_type);
    int i = 0;
#if 1 // wichoi 20201120 RS Demo
    switch(ocf_type)
    {
    case COCFDevice::OD_Tms:
        {
            for(i = 0; i < _dev_list.Size(); i++)
            {
                if(_dev_list.GetAt(i)->_ocf_dev_id == ocf_id)
                {
                    s16 value = 0; //(s16*)ocf_pay->data[0];
                    memcpy((char*)&value, (char*)&ocf_pay->data[0], sizeof(value));
                    CMSSmartPtrT<CServerTemperature> tmp;
                    tmp = reinterpret_cast<CServerTemperature*>(_dev_list.GetAt(i).GetPtr());
                    if(ocf_pay->cluster == 0x0402) // ZCL_TEMP_MEASUREMENT_CLUSTER_ID
                    {
                        printf("%s temp[%d]\n", __func__, value);
                        double conv = value /100;
                        tmp->UpdateTemperature(conv);
                    }
                    else // 0x0405 ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID:
                    {
                        printf("%s humid[%d]\n", __func__, value);
                        int conv = value /100;
                        tmp->UpdateHumidity(conv);
                    }
                    break;
                }
            }
        }
        break;

    case COCFDevice::OD_Door:
        {
            for(i = 0; i < _dev_list.Size(); i++)
            {
                if(_dev_list.GetAt(i)->_ocf_dev_id == ocf_id)
                {
                    u8 val = ocf_pay->data[0];
                    CMSSmartPtrT<CServerDoor> dor;
                    dor = reinterpret_cast<CServerDoor*>(_dev_list.GetAt(i).GetPtr());
                    dor->UpdateOpenAlarm(1);
                    printf("%s door[%d]\n", __func__, val);
                    if(val == 0)
                    {
                        dor->UpdateOpenState("Closed");
                    }
                    else
                    {
                        dor->UpdateOpenState("Open");
                    }
                    break;
                }
            }
        }
        break;

    case COCFDevice::OD_Motion:
        {
            for(i = 0; i < _dev_list.Size(); i++)
            {
                if(_dev_list.GetAt(i)->_ocf_dev_id == ocf_id)
                {
                    u8 motion = ocf_pay->data[0];
                    printf("%s motion[%d]\n", __func__, motion);
                    CMSSmartPtrT<CServerMotion> mot;
                    mot = reinterpret_cast<CServerMotion*>(_dev_list.GetAt(i).GetPtr());
                    mot->UpdateMotionValue((bool)motion);
                    break;
                }
            }
        }
        break;

    case COCFDevice::OD_Light:
        {
            for(i = 0; i < _dev_list.Size(); i++)
            {
                if(_dev_list.GetAt(i)->_ocf_dev_id == ocf_id)
                {
                    u8 value = ocf_pay->data[0];
                    CMSSmartPtrT<CServerLight> led;
                    led = reinterpret_cast<CServerLight*>(_dev_list.GetAt(i).GetPtr());
                    if(ocf_pay->cluster == 0x0006) // ZCL_ON_OFF_CLUSTER_ID
                    {
                        printf("%s onoff[%d]\n", __func__, value);
                        led->UpdateOnOff((bool)value);
                    }
                    else // 0x0008 ZCL_LEVEL_CONTROL_CLUSTER_ID
                    {
                        printf("%s dimming[%d]\n", __func__, value);
                        led->UpdateDimming((int)value);
                    }
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
#endif
#endif
    return RET_SUCCESS;
}

