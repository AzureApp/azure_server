/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : udp_client.h                                                       *
 ******************************************************************************
 * Copyright 2019 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#pragma once

#include <asio.hpp>
#include <functional>
#include "libazure/logging.h"
#include "libazure/util/delegate.h"

using namespace std::placeholders;

namespace azure {

/**
 * A barebones UDP client that has a backing buffer of 1024 bytes.
 */
class DiscoveryClient {
  using protocol = asio::ip::udp;

 public:
  DiscoveryClient(asio::io_context& io_context, uint16_t port);

  void Write(asio::mutable_buffer buffer);
  void Write(void* data, size_t size);

  void Close();

 public:
  Delegate<DiscoveryClient*> on_connect;
  Delegate<DiscoveryClient*, const asio::mutable_buffer&> on_receive;

 private:
  void OnReceive(const asio::error_code& ec);

  uint16_t port_;
  asio::io_context& io_context_;
  protocol::endpoint local_endpoint_;
  protocol::endpoint sender_endpoint_;
  std::unique_ptr<protocol::socket> socket_;
  std::array<char, 1024> buffer_;
};

}  // namespace azure