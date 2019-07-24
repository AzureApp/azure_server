

#include <Windows.h>
#include <iostream>
#include <string>
#include "logging.h"

namespace azure {

std::string DefaultLogLocation() { return "./azure.log"; }

void WriteToLog(int level, const char* fmt, ...) {
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

  OutputDebugStringA(line.c_str());
}

}  // namespace azure