/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : daemon.cc                                                          *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "daemon.h"
#include <gflags/gflags.h>

DEFINE_uint32(discovery_port, 19001, "Port for incoming discovery requests");
DEFINE_string(grpc_address, "0.0.0.0", "Address for gRPC to run on");
DEFINE_uint32(grpc_port, 1248, "Port for gRPC server to run on");

namespace azure {

/**
 * Create and/or return the shared daemon instance 
 */
Daemon& Daemon::Instance() {
  static Daemon daemon;
  daemon.Initialize();
  return daemon;
}

Daemon::Daemon() : discovery_service_(FLAGS_discovery_port) { }

void Daemon::Initialize() {
  AZLog("Initializing daemon");
  ServerBuilder builder;

  std::string server_address = azure::format_string(
      "%s:%hu", FLAGS_grpc_address.c_str(), FLAGS_grpc_port);
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  AZLog("gRPC Server initialized on {}", server_address);
  builder.RegisterService(&connection_service_);

  server_ = std::move(builder.BuildAndStart());

  on_init();
}

void Daemon::Run() {
  AZLog("Starting discovery service on port {}", FLAGS_discovery_port);
  discovery_service_.Start();

  AZLog("Starting gRPC service");
  server_->Wait();
}

void Daemon::Shutdown() {
  // TODO: Implement shutdown for discovery service?
  // discovery_service_.Shutdown();

  server_->Shutdown();
}


}
