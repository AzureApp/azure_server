/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : current_device.h                                                   *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <string>
#include "connection.pb.h"

namespace azure {

class CurrentDevice {
  using DeviceInfo = azure::proto::DeviceInfo;
  using OS = azure::proto::DeviceInfo::OperatingSystem;

 public:
  static DeviceInfo CurrentDeviceInfo() {
    DeviceInfo info;
    info.set_os_version(CurrentDevice::OsVersion());
    info.set_operating_system(CurrentDevice::OperatingSystem());
    info.set_device_name(CurrentDevice::DeviceName());
    info.set_udid(CurrentDevice::Udid());

    return info;
  }

  static std::string DeviceName();
  static OS OperatingSystem();
  static std::string OsVersion();
  static std::string Udid();
};

}  // namespace azure