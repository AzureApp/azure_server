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

  const char logger[] = "[Azure Daemon] ";
  char result[1024];

  strncpy(result, logger, sizeof(logger));
  strcat(result, fmt);

  vsprintf(result, fmt, args);
  va_end(args);

  __android_log_vprint(az_log_level_to_android(level), "Azure", fmt, args);

  printf("%s\n", result);

  FILE *log_file = fopen(DefaultLogLocation().c_str(), "a+");

  if (!strstr(result, "\n")) {
    result[strlen(result) + 1] = '\n';
  }

  fclose(log_file);
}

}  // namespace azure
