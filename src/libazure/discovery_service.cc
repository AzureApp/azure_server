/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : discovery_service.cc                                               *
 ******************************************************************************
 * Copyright 2019 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "discovery_service.h"

namespace azure {

DiscoveryService::DiscoveryService(uint16_t port)
    : io_context_(), client_(io_context_, port) {}

DiscoveryService::~DiscoveryService() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

void DiscoveryService::Start() {
  client_.on_receive.AddListener(
      [](azure::DiscoveryClient* client, const asio::mutable_buffer& buffer) {
        DeviceInfo info;
        memcpy(&info, buffer.data(), sizeof(info));
        std::cout << "\n\tClient name: " << info.device_name
                  << "\n\tClient system: " << info.device_system
                  << "\n\tClient address: " << info.device_ip << ":"
                  << info.device_port << std::endl;

        DeviceInfo client_info = DeviceInfo::SystemInfo();

        client->Write(&client_info, sizeof(client_info));
      });
  thread_ = std::thread(&DiscoveryService::Run, this);
}

void DiscoveryService::Run() { io_context_.run(); }

}  // namespace azure