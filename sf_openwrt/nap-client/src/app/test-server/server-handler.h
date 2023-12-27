#ifndef __SERVER_HANDLER_H__
#define __SERVER_HANDLER_H__

#include "cmd-service.grpc.pb.h"
#include "msg-service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class grpc_server_msg final : public stbp::v1::messages::AccessPointGateway::Service
{
public:
    Status Frames(::grpc::ServerContext* context, const ::stbp::v1::messages::FrameSet* request, ::stbp::v1::messages::FramesAcknowledgement* response) override;
    Status Locations(::grpc::ServerContext* context, const ::stbp::v1::messages::Location* request, ::stbp::v1::messages::LocationAcknowledgement* response) override;
    Status Heartbeats(grpc::ServerContext* context, const stbp::v1::messages::Heartbeat* request, stbp::v1::messages::HeartbeatAcknowledgement* reply) override;
    Status Configurations(::grpc::ServerContext* context, const ::stbp::v1::messages::Configuration* request, ::stbp::v1::messages::ConfigurationAcknowledgement* response) override;
};

class grpc_server_cmd final : public stbp::v1::commands::AccessPointGatewayCommands::Service
{
public:
    grpc::Status Downlinks(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::stbp::v1::commands::DownlinkSet, ::stbp::v1::commands::DownlinkAcknowledgement>* stream) override;
    grpc::Status Configurations(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::stbp::v1::commands::Configuration, ::stbp::v1::commands::ConfigurationStatus>* stream) override;
    grpc::Status Upgrades(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::stbp::v1::commands::Upgrade, ::stbp::v1::commands::UpgradeStatus>* stream) override;
};

class server_handler
{
public:
    server_handler();
    ~server_handler();
    int init(void);
    int deinit(void);
    int start_server(std::string &addr);
    int stop_server(void);

private:
    grpc_server_msg _service_msg;
    grpc_server_cmd _service_cmd;
    std::unique_ptr<Server> _grpc_server;
};

#endif
