

#include <Windows.h>
#include "logging.h"

namespace azure {

std::string DefaultLogLocation() { return "./azure.log"; }

void WriteToLog(int level, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  const char logger[] = "[Azure Daemon] ";
  char result[1024] = {0};

  strncpy_s(result, sizeof(result), logger, sizeof(logger));
  strcat_s(result, sizeof(result), fmt);

  vsprintf(result, fmt, args);
  va_end(args);

  if (!strstr(result, "\n")) {
    result[strlen(result) + 1] = '\n';
  }

  OutputDebugStringA(result);

  printf("%s\n", result);
}

}  // namespace azure