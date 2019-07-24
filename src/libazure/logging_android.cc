/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging_android.cc                                                 *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

// android implementation of logging function(s)

#include "logging.h"

#include <android/log.h>
#include <iostream>
#include <string>

namespace azure {

static int az_log_level_to_android(int level) {
  switch (level) {
    case AZ_LOG_INFO:
      return ANDROID_LOG_INFO;
    case AZ_LOG_WARN:
      return ANDROID_LOG_WARN;
    case AZ_LOG_DEBUG:
      return ANDROID_LOG_DEBUG;
    case AZ_LOG_ERROR:
    default:
      return ANDROID_LOG_ERROR;
  }
}

std::string DefaultLogLocation() { return "./azure.log"; }

void WriteToLog(int level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  std::string prefix = "[Azure Daemon] ";

  std::string line;
  line.resize(std::vsnprintf(nullptr, 0, fmt, args) + 1);
  std::vsprintf(&line[0], fmt, args);
  line.insert(0, prefix);
  line += '\n';

  va_end(args);

  std::cout.write(line.c_str(), line.size());

  __android_log_print(az_log_level_to_android(level), "Azure", "%s",
                      line.c_str());

  FILE *log_file = fopen(DefaultLogLocation().c_str(), "a+");
  fwrite(line.c_str(), 1, line.size(), log_file);
  fclose(log_file);
}

}  // namespace azure
