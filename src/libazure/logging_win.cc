/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging_android.cc                                                 *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "logging.h"

DECLARE_string(log_location);

namespace azure {

void Logger::LoadSinks() {
  std::vector<spdlog::sink_ptr> sinks;

  sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
  if (!FLAGS_log_location.empty()) {
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>(
        FLAGS_log_location));   
  }

  logger_ =
      std::make_unique<spdlog::logger>(kLoggerName, sinks.begin(),
                                             sinks.end());
}


}  // namespace azure