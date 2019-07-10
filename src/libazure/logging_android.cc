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

  __android_log_vprint(az_log_level_to_android(level), "Azure", fmt, args);

  if (!strstr(fmt, "\n")) {
    fmt = concat(fmt, "\n");
  }

  const char logger[] = "[Azure Daemon] ";
  char result[1024];

  strncpy(result, logger, sizeof(logger));
  strcat(result, fmt);

  FILE *log_file = fopen(DefaultLogLocation().c_str(), "a+");

  vprintf(result, args);
  va_end(args);

  fclose(log_file);
}

}  // namespace azure
