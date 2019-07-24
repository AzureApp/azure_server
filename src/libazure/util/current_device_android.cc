/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : current_device_android.cc                                          *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <sys/system_properties.h>
#include <vector>
#include "current_device.h"
#include "libazure/logging.h"
#include "libazure/util/string.h"

namespace azure {

using DeviceInfo = azure::proto::DeviceInfo;
using OperatingSystem = azure::proto::DeviceInfo::OperatingSystem;

int get_system_property(const char* property, char* output,
                        const char* default_value) {
  if (!output || !property) {
    if (default_value) {
      strncpy(output, default_value,
              strnlen(default_value, PROP_VALUE_MAX) + 1);
    }
    return 0;
  }

  int read = __system_property_get(property, output);
  if (read == 0) {
    std::string cmd = azure::format_string("getprop %s", property);

    AZLogD("Running command %s", cmd.c_str());
    FILE* file = popen(cmd.c_str(), "r");
    read = fread(output, 1, PROP_VALUE_MAX, file);
    pclose(file);

    return read;
  }

  return read;
}

std::string CurrentDevice::DeviceName() {
  char manufacturer[PROP_VALUE_MAX] = {0};
  char device[PROP_VALUE_MAX] = {0};

  get_system_property("ro.product.manufacturer", manufacturer,
                      "Unknown Manufacturer");
  get_system_property("ro.product.model", device, "Unknown Model");

  printf("xxx %s\n", manufacturer);
  return azure::format_string("%s %s", manufacturer, device);
}

OperatingSystem CurrentDevice::OperatingSystem() {
  return OperatingSystem::DeviceInfo_OperatingSystem_OS_ANDROID;
}

std::string CurrentDevice::OsVersion() {
  char sdk_ver_str[PROP_VALUE_MAX] = {0};
  get_system_property("ro.build.version.sdk", sdk_ver_str, "v?.?");

  return azure::format_string("Android (API Level %s)", sdk_ver_str);
}

std::string CurrentDevice::Udid() { return ""; }

}  // namespace azure