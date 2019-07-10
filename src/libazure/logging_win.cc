

#include <Windows.h>
#include "logging.h"

namespace azure {

std::string DefaultLogLocation() { return "./azure.log"; }

void WriteToLog(int level, const char* fmt, ...) {
  if (!strstr(fmt, "\n")) {
    fmt = concat(fmt, "\n");
  }

  const char logger[] = "[Azure Daemon] ";
  char result[1024];

  strncpy_s(result, sizeof(result), logger, sizeof(logger));
  strcat_s(result, sizeof(result), fmt);

  OutputDebugStringA(result);
}

}  // namespace azure