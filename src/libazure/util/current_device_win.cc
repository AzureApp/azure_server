/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : current_device_win.cc                                              *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <Windows.h>

#include <VersionHelpers.h>
#include <vector>
#include "current_device.h"
#include "libazure/util/string.h"
#include <cassert>

namespace azure {

using DeviceInfo = azure::proto::DeviceInfo;
using OperatingSystem = azure::proto::DeviceInfo::OperatingSystem;

std::string CurrentDevice::DeviceName() {
  // get inital size
  DWORD length = 0;
  GetComputerNameExA(ComputerNamePhysicalDnsHostname, nullptr, &length);

  // create string with buffer big enough to hold name
  std::string name;
  name.reserve(length);

  GetComputerNameExA(ComputerNamePhysicalDnsHostname, &name[0], &length);

  return name;
}

OperatingSystem CurrentDevice::OperatingSystem() {
  return OperatingSystem::DeviceInfo_OperatingSystem_OS_WINDOWS;
}

std::string CurrentDevice::OsVersion() {
  const auto kernel32 = L"kernel32.dll";
  DWORD dummy;
  const auto info_size =
      GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, kernel32, &dummy);

  std::vector<char> buffer(info_size);
  GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, kernel32, dummy, buffer.size(),
                        &buffer[0]);
  void *p = nullptr;
  UINT size = 0;
  VerQueryValueW(buffer.data(), L"\\", &p, &size);

  assert(size >= sizeof(VS_FIXEDFILEINFO));
  assert(p != nullptr);

  auto file_info = static_cast<const VS_FIXEDFILEINFO *>(p);

  std::string version = azure::format_string(
      "Windows (%ld.%ld.%ld.%ld)", HIWORD(file_info->dwFileVersionMS),
      LOWORD(file_info->dwFileVersionMS), HIWORD(file_info->dwFileVersionLS),
      LOWORD(file_info->dwFileVersionLS));

  return version;
}

std::string CurrentDevice::Udid() { return ""; }

}  // namespace azure