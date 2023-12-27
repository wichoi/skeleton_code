#include <unistd.h>

#include <iostream>
#include <memory>
#include <string>
#include <list>

#include <grpcpp/grpcpp.h>
//#include <grpcpp/health_check_service_interface.h>
//#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "common.h"
#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "server-handler.h"

Status grpc_server_msg::Frames(::grpc::ServerContext* context,
                                  const ::stbp::v1::messages::FrameSet* request,
                                  ::stbp::v1::messages::FramesAcknowledgement* response
                                  )
{
    return Status::OK;
}

Status grpc_server_msg::Locations(::grpc::ServerContext* context,
                                     const ::stbp::v1::messages::Location* request,
                                     ::stbp::v1::messages::LocationAcknowledgement* response
                                     )
{
    return Status::OK;
}

Status grpc_server_msg::Heartbeats(grpc::ServerContext* context,
                                      const stbp::v1::messages::Heartbeat* request,
                                      stbp::v1::messages::HeartbeatAcknowledgement* reply
                                      )
{
    return Status::OK;
}

Status grpc_server_msg::Configurations(::grpc::ServerContext* context,
                                          const ::stbp::v1::messages::Configuration* request,
                                          ::stbp::v1::messages::ConfigurationAcknowledgement* response
                                          )
{
    return Status::OK;
}

grpc::Status grpc_server_cmd::Downlinks(::grpc::ServerContext* context,
                                        ::grpc::ServerReaderWriter< ::stbp::v1::commands::DownlinkSet,
                                        ::stbp::v1::commands::DownlinkAcknowledgement>* stream
                                        )
{
    return Status::OK;
}

grpc::Status grpc_server_cmd::Configurations(::grpc::ServerContext* context,
                                               ::grpc::ServerReaderWriter< ::stbp::v1::commands::Configuration,
                                               ::stbp::v1::commands::ConfigurationStatus>* stream
                                               )
{
    return Status::OK;
}

grpc::Status grpc_server_cmd::Upgrades(::grpc::ServerContext* context,
                                          ::grpc::ServerReaderWriter< ::stbp::v1::commands::Upgrade,
                                          ::stbp::v1::commands::UpgradeStatus>* stream
                                          )
{
    return Status::OK;
}

server_handler::server_handler() :
    _service_msg(),
    _grpc_server(),
    _service_cmd()
    //_proc()
{
}

server_handler::~server_handler()
{
}

int server_handler::init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    return ret_val;
}

int server_handler::deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    return ret_val;
}

int server_handler::start_server(std::string &addr)
{
    int ret_val = RET_OK;
    return ret_val;
}

int server_handler::stop_server(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _grpc_server->Shutdown();
    return ret_val;
}

