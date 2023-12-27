#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

class config_service
{
public:
    config_service();
    ~config_service();
    int init(void);
    int deinit(void);

    int config_read(void);
    int config_write(void);

    //int object_list(list<json_param> &obj_list);
};

#endif // CONFIG_SERVICE_H
