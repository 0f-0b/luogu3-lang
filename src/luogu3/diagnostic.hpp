#ifndef LUOGU3_DIAGNOSTIC_HPP
#define LUOGU3_DIAGNOSTIC_HPP

#include <cstddef>
#include <ostream>
#include <span>
#include <string>
#include <string_view>

namespace ud2::luogu3 {
  struct diagnostic {
    std::size_t start;
    std::size_t end;
    std::string message;
  };

  auto print_diagnostics(std::ostream& out, const std::span<diagnostic> diags, const std::string_view filename, const std::string_view source) -> bool;
}

#endif
