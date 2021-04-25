/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : discovery_service.h                                                *
 ******************************************************************************
 * Copyright 2019 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#pragma once

#include <thread>
#include "connection.pb.h"
#include "discovery_client.h"

namespace azure {

class DiscoveryService {
  using HandshakeRequest = azure::proto::Handshake;

 public:
  explicit DiscoveryService(uint16_t port);
  DiscoveryService(const DiscoveryService &) = delete;
  ~DiscoveryService();

  void Start();

 private:
  void Run();

  asio::io_context io_context_;
  DiscoveryClient client_;
  std::thread thread_;
};

}  // namespace azure