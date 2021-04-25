/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : connection_service.h                                               *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#pragma once

#include <grpc++/grpc++.h>
#include "connection.grpc.pb.h"

namespace azure {

using azure::proto::ConnectionService;
using azure::proto::Handshake;
using grpc::ServerContext;
using grpc::Status;

class ConnectionServiceImpl final : public proto::ConnectionService::Service {
  // *
  // Sends a handshake message on new connection to client, who sends one back
  // if client is on an incompatible version of azure, this rpc will return an
  // error instead
  Status Connect(ServerContext* context, const proto::Handshake* request,
                 proto::Handshake* response) override;
  // *
  // Disconnect from connected server.
  // This rpc should not return if a kill/restart operation is also provided
  Status Disconnect(ServerContext* context,
                    const proto::DisconnectRequest* request,
                    proto::DisconnectResponse* response) override;

  Handshake CreateHandshake() const;
};

}  // namespace azure