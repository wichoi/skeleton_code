#ifndef __SECC_MESSAGE_H__
#define __SECC_MESSAGE_H__

int secc_msg_init(void);
int secc_msg_deinit(void);

int secc_msg_recv(char *bitstream, size_t sz);
int secc_msg_sdp_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_supported_app_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_session_setup_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_service_discover_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_service_datail_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_payment_selection_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_payment_detail_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_authorization_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_charge_param_discover_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_power_delivery_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_session_stop_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_certificate_update_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_certificate_install_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_charging_status_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_metering_receipt_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_cable_check_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_pre_charge_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_current_demand_encode(char *buf, size_t buf_sz, size_t *stream_len);
int secc_msg_welding_detection_encode(char *buf, size_t buf_sz, size_t *stream_len);

#endif

