/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging.h                                                          *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef AZURE_LOGGING_H
#define AZURE_LOGGING_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ostream>
#include <string>
#include "targets.h"

#define AZ_LOG_INFO 1
#define AZ_LOG_WARN 2
#define AZ_LOG_ERROR 3
#define AZ_LOG_DEBUG 4

#define AZLogE(...) azure::WriteToLog(AZ_LOG_ERROR, __VA_ARGS__)
#define AZLogW(...) azure::WriteToLog(AZ_LOG_WARN, __VA_ARGS__)
#define AZLogD(...) azure::WriteToLog(AZ_LOG_DEBUG, __VA_ARGS__)
#define AZLog(...) azure::WriteToLog(AZ_LOG_INFO, __VA_ARGS__)

namespace azure {

static const char* concat(const char* s1, const char* s2) {
  size_t size = strlen(s1) + strlen(s2) + 1;
  char* result = (char*)malloc(size);
  strncpy(result, s1, strlen(s1));
  strcat(result, s2);
  return result;
}

// http://www.i42.co.uk/stuff/hexdump.htm
template <typename Elem, typename Traits>
inline void hex_dump(const void* aData, std::size_t aLength,
                     std::basic_ostream<Elem, Traits>& aStream,
                     std::size_t aWidth = 16) {
  const char* const start = static_cast<const char*>(aData);
  const char* const end = start + aLength;
  const char* line = start;
  while (line != end) {
    aStream.width(4);
    aStream.fill('0');
    aStream << std::hex << line - start << " : ";
    std::size_t lineLength =
        std::min(aWidth, static_cast<std::size_t>(end - line));
    for (std::size_t pass = 1; pass <= 2; ++pass) {
      for (const char* next = line; next != end && next != line + aWidth;
           ++next) {
        char ch = *next;
        switch (pass) {
          case 1: {
            if (next != line) aStream << " ";
            aStream.width(2);
            aStream.fill('0');
            aStream << std::hex << std::uppercase
                    << static_cast<int>(static_cast<unsigned char>(ch));
            break;
          }
          case 2: {
            aStream << (isprint(ch) && (ch != '\x20') ? ch : '.');
            break;
          }
        }
      }
      if (pass == 1 && lineLength != aWidth)
        aStream << std::string(aWidth - lineLength, ' ');
      aStream << " ";
    }
    aStream << std::endl;
    line = line + lineLength;
  }
}

/**
 * Get the default location that logs are written to.
 * This function is implemented on a per-platform basis.
 */
extern std::string DefaultlogLocation();

/**
 * Write string to the configured log file/stream.
 * This function is implemented on a per-platform basis.
 */
void WriteToLog(int level, const char* fmt, ...);

}  // namespace azure

#endif  // AZURE_LOGGING_H