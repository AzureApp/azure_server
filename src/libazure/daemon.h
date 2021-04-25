/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : daemon.h                                                           *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#pragma once

#include <memory>
#include <grpc++/grpc++.h>
#include <libazure/discovery/discovery_service.h>
#include <libazure/logging.h>
#include <libazure/services/connection_service.h>
#include <libazure/util/string.h>
#include <libazure/util/delegate.h>

using azure::DiscoveryService;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace azure {

class Daemon {
  using ConnectionService = ConnectionServiceImpl;
public:
  static Daemon& Instance();

  void Run();

  void Shutdown();

 public:
  Delegate<> on_init;

 private:
  Daemon();

  void Initialize();

  std::unique_ptr<Server> server_;
  DiscoveryService discovery_service_;
  ConnectionServiceImpl connection_service_;
};

}
