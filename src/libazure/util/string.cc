

#include "string.h"

namespace azure {

// https://github.com/xenia-project/xenia/blob/master/src/xenia/base/string.cc
std::string format_string(const char* format, va_list args) {
  if (!format) {
    return "";
  }
  size_t max_len = 64;
  std::string new_s;
  while (true) {
    new_s.resize(max_len);
    int ret =
        std::vsnprintf(const_cast<char*>(new_s.data()), max_len, format, args);
    if (ret > max_len) {
      // Needed size is known (+2 for termination and avoid ambiguity).
      max_len = ret + 2;
    } else if (ret == -1 || ret >= max_len - 1) {
      // Handle some buggy vsnprintf implementations.
      max_len *= 2;
    } else {
      // Everything fit for sure.
      new_s.resize(ret);
      return new_s;
    }
  }
}

}  // namespace azure