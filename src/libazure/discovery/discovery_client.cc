/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : udp_client.cc                                                      *
 ******************************************************************************
 * Copyright 2019 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "discovery_client.h"
#include <iostream>

namespace azure {

DiscoveryClient::DiscoveryClient(asio::io_context& io_context, uint16_t port)
    : io_context_(io_context), socket_(nullptr), port_(port) {
  asio::error_code ec;

  local_endpoint_ = protocol::endpoint(asio::ip::address_v4::any(), port_);

  try {
    socket_ = std::make_unique<protocol::socket>(io_context_, local_endpoint_);
  } catch (const asio::system_error& e) {
    AZLogE("Could not create socket: {}", std::string{e.what()});
    exit(1);
  }

  // allow address reuse and set socket to broadcast mode
  socket_->set_option(asio::ip::udp::socket::reuse_address(true));
  socket_->set_option(asio::socket_base::broadcast(true));

  socket_->async_receive_from(asio::buffer(buffer_, sizeof(buffer_)),
                              sender_endpoint_,
                              std::bind(&DiscoveryClient::OnReceive, this, _1));
  on_connect(this);
}

void DiscoveryClient::Write(asio::mutable_buffer buffer) {
  try {
    socket_->send_to(buffer, sender_endpoint_);
  } catch (const asio::system_error& e) {
    AZLogE("Could not send device info: {}", std::string{e.what()});
    exit(1);
  }
}

void DiscoveryClient::Write(void* data, size_t size) {
  Write(asio::buffer(data, size));
}

void DiscoveryClient::OnReceive(const asio::error_code& ec) {
  if (ec) {
    AZLogE("Could not receive data: [error: {}]", ec.message());
    exit(1);
  } else {
    on_receive(this, asio::buffer(buffer_, sizeof(buffer_)));
    socket_->async_receive_from(
        asio::buffer(buffer_, sizeof(buffer_)), sender_endpoint_,
        std::bind(&DiscoveryClient::OnReceive, this, _1));
  }
}

}  // namespace azure