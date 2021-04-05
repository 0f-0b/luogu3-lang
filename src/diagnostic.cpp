#include "diagnostic.hpp"
#include <algorithm>
#include <cstring>
#include <iterator>

namespace ud2::luogu3 {
  auto print_diagnostics(std::ostream& out, const std::vector<diagnostic>& diags, const char* filename, const char* source) -> bool {
    if (diags.size() == 0)
      return false;
    auto lines = std::vector<std::size_t>{};
    for (auto ptr = source;; ++ptr) {
      lines.push_back(ptr - source);
      if (!(ptr = strchr(ptr, '\n')))
        break;
    }
    for (const auto& diag : diags) {
      auto index = diag.start;
      auto it = std::prev(std::upper_bound(lines.cbegin(), lines.cend(), index));
      auto line = static_cast<std::size_t>(it - lines.cbegin());
      auto column = static_cast<std::size_t>(index - *it);
      out << filename << ':' << (line + 1) << ':' << (column + 1) << ": error: " << diag.message << '\n';
    }
    return true;
  }
}
