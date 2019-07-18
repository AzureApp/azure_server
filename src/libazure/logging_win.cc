

#include <Windows.h>
#include "logging.h"

namespace azure {

std::string DefaultLogLocation() { return "./azure.log"; }

void WriteToLog(int level, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  if (!strstr(fmt, "\n")) {
    fmt = concat(fmt, "\n");
  }

  const char logger[] = "[Azure Daemon] ";
  char result[1024];

  strncpy_s(result, sizeof(result), logger, sizeof(logger));
  strcat_s(result, sizeof(result), fmt);

  vsprintf(result, fmt, args);
  va_end(args);

  OutputDebugStringA(result);

  printf("%s\n", result);
}

}  // namespace azure