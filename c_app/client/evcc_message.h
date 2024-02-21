#ifndef __EVCC_MESSAGE_H__
#define __EVCC_MESSAGE_H__

int evcc_msg_init(void);
int evcc_msg_deinit(void);

int evcc_msg_recv(char *bitstream, size_t sz);
int evcc_msg_sdp_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_supported_app_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_session_setup_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_service_discover_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_service_datail_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_payment_selection_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_payment_detail_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_authorization_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_parameter_discovery_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_power_delivery_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_session_stop_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_certificate_update_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_certificate_install_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_charging_status_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_metering_receipt_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_cable_check_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_pre_charge_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_current_demand_encode(char *buf, size_t buf_sz, size_t *stream_len);
int evcc_msg_welding_detection_encode(char *buf, size_t buf_sz, size_t *stream_len);

#endif

