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
#include "libazure/util/current_device.h"
namespace azure {

using DeviceInfo = azure::proto::DeviceInfo;

DiscoveryService::DiscoveryService(uint16_t port)
    : io_context_(), client_(io_context_, port) {}

DiscoveryService::~DiscoveryService() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

void DiscoveryService::Start() {
  client_.on_receive.AddListener([](azure::DiscoveryClient* client,
                                    const asio::mutable_buffer& buffer) {
    DeviceInfo client_info;
    client_info.ParseFromArray(buffer.data(), buffer.size());

    AZLog(
        "Received discovery request from client:\n\t"
        "Client name: {0}\n\t "
        "Client system: {1}",
        client_info.device_name(), client_info.os_version());

    DeviceInfo device_info = CurrentDevice::CurrentDeviceInfo();

    int buffer_size = device_info.ByteSize();
    std::vector<char> buf(buffer_size);

    device_info.SerializeToArray(&buf[0], buffer_size);
    client->Write(buf.data(), buf.size());
  });
  thread_ = std::thread(&DiscoveryService::Run, this);
}

void DiscoveryService::Run() { io_context_.run(); }

}  // namespace azure