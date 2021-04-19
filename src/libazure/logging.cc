/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging_android.cc                                                 *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "logging.h"

DEFINE_string(log_location, "./azure.log",
              "The default location for file logs");

namespace azure {

Logger& Logger::SharedLogger() {
  static Logger logger;
  if (!logger.Initialize()) {
    exit(10);
  }

  return logger;
}
Logger::Logger() : logger_(nullptr) {}


bool Logger::Initialize() {
  LoadSinks();
  return true;
}



}
