/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : logging.h                                                          *
 ******************************************************************************
 * Copyright 2021 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#pragma once

#include <ostream>
#include <algorithm>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <gflags/gflags.h>

#define AZLogE(...) azure::WriteToLog(LogLevel::Error, __VA_ARGS__)
#define AZLogW(...) azure::WriteToLog(LogLevel::Warn, __VA_ARGS__)
#define AZLogD(...) azure::WriteToLog(LogLevel::Debug, __VA_ARGS__)
#define AZLog(...) azure::WriteToLog(LogLevel::Info, __VA_ARGS__)

enum class LogLevel { Info = 1, Warn, Error, Debug };

namespace azure {

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

class Logger {
public:
  static Logger& SharedLogger();

  template <typename LogString, typename... Args>
  void Write(LogLevel level, LogString str, Args ... args);
private:
  Logger();
 ~Logger() = default;

  bool Initialize();

  void LoadSinks();

  const std::string kLoggerName = "azure";
  std::unique_ptr<spdlog::logger> logger_;
};

/**
 * Get the default location that logs are written to.
 * This function is implemented on a per-platform basis.
 */
extern std::string DefaultlogLocation();

template <typename LogString, typename ... Args>
void Logger::Write(LogLevel level, LogString str, Args ... args) {
  switch (level) {
    case LogLevel::Info: {
      logger_->info(str, std::forward<Args>(args)...);
      break;
    }
    case LogLevel::Warn: {
      logger_->warn(str, std::forward<Args>(args)...);
      break;
    }
    case LogLevel::Error: {
      logger_->error(str, std::forward<Args>(args)...);
      break;
    }
    case LogLevel::Debug: {
      logger_->error(str, std::forward<Args>(args)...);
      break;
    }
    default:
      return;
  }
}

template <typename... Args>
void WriteToLog(LogLevel level, Args... args) {
  Logger::SharedLogger().Write(level, std::forward<Args>(args)...);
}

} // namespace azure
