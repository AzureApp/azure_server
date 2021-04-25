/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : connection_service.cc                                              *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "connection_service.h"
#include "libazure/util/current_device.h"
#include "libazure/daemon.h"
#include "libazure/logging.h"

namespace azure {

std::string OperatingSystemName(azure::proto::DeviceInfo_OperatingSystem os) {
  return azure::proto::DeviceInfo_OperatingSystem_Name(os);
}

Status ConnectionServiceImpl::Connect(ServerContext* context,
                                      const proto::Handshake* request,
                                      proto::Handshake* response) {
  const proto::DeviceInfo& info = request->device_info();

  AZLog("Received connection request from client (azure version {}",
        request->azure_version());

  AZLog(
      "Client info: OS: {0} [Version {1}], Device Name: {2}, Unique "
      "Identifier: {3} ",
      OperatingSystemName(info.operating_system()), info.os_version(),
      info.device_name(),
      info.udid());

  auto handshake = CreateHandshake();

  response->CopyFrom(handshake);

  return Status::OK;
}

Status ConnectionServiceImpl::Disconnect(
    ServerContext* context, const proto::DisconnectRequest* request,
    proto::DisconnectResponse* response) {
  auto operation = request->operation();
  if (operation == proto::OP_RESTART) {
    AZLogW("Server restart requested however restart is not supported.");

    return Status(
        grpc::StatusCode::UNIMPLEMENTED,
        "Server restart operation not implemented. Server will now shutdown");
  } else if (operation == proto::OP_KILL) {
    AZLogE("Force shutdown requested");
    exit(64);
  } else if (operation == proto::OP_NONE) {
    AZLog("Shutting down daemon");

    auto& daemon = Daemon::Instance();

    daemon.Shutdown();

    // should not be reached
    return Status::OK;
  }
}

Handshake ConnectionServiceImpl::CreateHandshake() const {
  Handshake hs;
  auto device_info = hs.mutable_device_info();
  hs.set_azure_version("1.0.0"); // TODO: Don't hardcode this
  device_info->set_device_name(CurrentDevice::DeviceName());
  device_info->set_os_version(CurrentDevice::OsVersion());
  device_info->set_device_name(CurrentDevice::DeviceName());
  device_info->set_udid(CurrentDevice::Udid());

  return hs;
}


} // namespace azure
