/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging_android.cc                                                 *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "logging.h"
#include <spdlog/common.h>
#include <vector>

namespace azure {

void Logger::LoadSinks() {
  std::vector<spdlog::sink_ptr> sinks;

  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
}


}  // namespace azure
