#ifndef __RMS_JSON_MANAGER_H__
#define __RMS_JSON_MANAGER_H__

#include "common.h"
#include "json-manager.h"

class rms_json
{
public:
    rms_json();
    ~rms_json();
    int init(void);
    int deinit(void);

    int rms_create(list<json_param> &obj_list, string &json_body);
    int rms_parse(string &response);
    int rms_parse_boot(string &response, string &nap_id, string &rms_tok, string &name, string &model_id, string &model_name, u64 &utc_time);
    int rms_parse_ca(string &response, string &fac_crt);
    int rms_parse_token(string &response, string &rms_tok);
    int rms_parse_poll(string &response, list<json_param> &get_list, list<json_param> &bulk_list, list<json_param> &set_list);
    int rms_create_result(list<json_param> &obj_list, list<json_param> &get_list, list<json_param> &set_list, list<json_param> &bulk_list, string &json_body);

    int rms_create_whitelist(list<json_param> &obj_list, bool w_conf, list<string> &white_list, string &json_body);
    int rms_parse_whitelist(string &response, bool *read_conf, list<string> &recv_list);

    int rms_parse_fwcheck(string &response, int *is_up, string &fw_url, string &crc, string &key);

private:
    list<string> _poll_obj;
};

#endif
