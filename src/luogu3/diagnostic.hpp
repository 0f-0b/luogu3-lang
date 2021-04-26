#ifndef LUOGU3_DIAGNOSTIC_HPP
#define LUOGU3_DIAGNOSTIC_HPP

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace ud2::luogu3 {
  struct diagnostic {
    std::size_t start;
    std::size_t end;
    std::string message;
  };

  auto print_diagnostics(std::ostream& out, const std::vector<diagnostic>& diags, const char* filename, const char* source) -> bool;
}

#endif
