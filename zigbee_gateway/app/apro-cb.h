#ifndef _APRO_HANDLER_H_
#define _APRO_HANDLER_H_

void apro_cb_init(void);
void apro_cb_proc(void);
int apro_cb_reset_attribute(uint8_t endpointId);
bool apro_cb_pre_cmd_received(EmberAfClusterCommand* cmd);
bool apro_cb_pre_zdo_msg_received(EmberNodeId emberNodeId, EmberApsFrame* apsFrame, uint8_t* message, uint16_t length);
bool apro_cb_retrieve_attr_and_craft_response(uint8_t endpoint, EmberAfClusterId clusterId, EmberAfAttributeId attrId, uint8_t mask, uint16_t maunfacturerCode, uint16_t readLength);
void apro_cb_zigbee_key_establishment(EmberEUI64 partner, EmberKeyStatus status);
bool apro_cb_read_attr_response(EmberAfClusterId clusterId, uint8_t* buffer, uint16_t bufLen);
bool apro_cb_report_attribute(EmberAfClusterId clusterId, uint8_t * buffer, uint16_t bufLen);
bool apro_cb_pre_msg_recv(EmberAfIncomingMessage* incomingMessage);

#endif
