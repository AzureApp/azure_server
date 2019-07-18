#include <grpc++/grpc++.h>
#include <libazure/discovery_service.h>
#include <libazure/services/connection_service.h>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using azure::DiscoveryService;

const unsigned short BROADCAST_PORT = 19001;

struct DeviceInfo {
  char device_name[32];
  char device_system[32];
  char device_ip[17];
  unsigned short device_port;

  DeviceInfo() {
    memset(device_name, 0, 32);
    memset(device_system, 0, 32);
    memset(device_ip, 0, 16);
  }

  DeviceInfo(const char *name, const char *system, unsigned short port) {
    memcpy(device_name, name, 32);
    memcpy(device_system, system, 32);
    device_port = port;
  }

  void set_ip(const std::string &ip) {
    memcpy(device_ip, ip.data(), strnlen(ip.data(), 16) + 1);
  }

  static DeviceInfo SystemInfo() {
    return DeviceInfo{"WIndows PC", "Windows", 1248};
  }
};

int main(int argc, char **argv) {
  std::printf("This is a development executable. Please do not use\n");

  // setup discovery client
  AZLog("Setting up peer-discovery client");
  DiscoveryService discovery_service(19001);
  discovery_service.Start();

  // setup grpc
  AZLog("Setting up gRPC server");
  std::string server_address("0.0.0.0:1248");
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
