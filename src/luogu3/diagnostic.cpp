#include <algorithm>
#include <iterator>
#include <luogu3/diagnostic.hpp>
#include <string_view>
#include <vector>

namespace ud2::luogu3 {
  auto print_diagnostics(std::ostream& out, const std::span<diagnostic> diags, const std::string_view filename, const std::string_view source) -> bool {
    if (diags.size() == 0)
      return false;
    auto lines = std::vector<std::size_t>{};
    for (auto pos = static_cast<std::string_view::size_type>(0);; ++pos) {
      lines.push_back(pos);
      if (!~(pos = source.find('\n', pos)))
        break;
    }
    for (const auto& diag : diags) {
      auto index = diag.start;
      auto it = std::prev(std::upper_bound(lines.begin(), lines.end(), index));
      auto line = static_cast<std::size_t>(it - lines.begin());
      auto column = static_cast<std::size_t>(index - *it);
      out << filename << ':' << (line + 1) << ':' << (column + 1) << ": error: " << diag.message << '\n';
    }
    return true;
  }
}
