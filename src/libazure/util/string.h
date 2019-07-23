
#include <stdarg.h>
#include <string>

namespace azure {

std::string format_string(const char* str, va_list args);
inline std::string format_string(const char* str, ...) {
  va_list va;
  va_start(va, str);
  auto result = format_string(str, va);
  va_end(va);
  return result;
}

}  // namespace azure