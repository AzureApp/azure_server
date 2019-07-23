#include <gflags/gflags.h>
#include <grpc++/grpc++.h>
#include <libazure/discovery/discovery_service.h>
#include <libazure/logging.h>
#include <libazure/services/connection_service.h>
#include <libazure/util/string.h>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using azure::DiscoveryService;

DEFINE_uint32(discovery_port, 19001, "Port for incoming discovery requests");
DEFINE_string(grpc_address, "0.0.0.0", "Address for gRPC to run on");
DEFINE_uint32(grpc_port, 1248, "Port for gRPC server to run on");

int main(int argc, char **argv) {
  AZLogW("This is a development executable. Please do not use\n");

  // setup discovery client
  AZLog("Setting up peer-discovery client");
  DiscoveryService discovery_service(FLAGS_discovery_port);
  discovery_service.Start();

  // setup grpc
  AZLog("Setting up gRPC server");
  std::string server_address = azure::format_string(
      "%s:%hu", FLAGS_grpc_address.c_str(), FLAGS_grpc_port);

  azure::ConnectionServiceImpl connection_service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&connection_service);
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  // builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  AZLog("gRPC server listening on %s", server_address.c_str());

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();

  std::cin.get();
  return 0;
}
