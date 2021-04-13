#include "program.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ud2::luogu3 {
  auto program::emit_source(std::ostream& out) const -> void {
    out << this->states.size() << ' ' << (this->init + 1) << '\n';
    for (const auto& state : this->states)
      std::visit([&](auto s) { s.emit_source(out); }, state);
  }

  auto program::emit_c(std::ostream& out) const -> void {
    auto max_stack = static_cast<std::size_t>(0);
    for (const auto& state : this->states)
      std::visit([&](auto s) { max_stack = std::max(max_stack, s.max_stack()); }, state);
    if (!~max_stack)
      throw std::invalid_argument{"too many stacks"};
    out
      << "#include <inttypes.h>\n"
      << "#include <stdio.h>\n"
      << "\n"
      << "int main() {\n"
      << "  static uint_least32_t stack[" << (max_stack + 1) << "][" << stack_capacity << "];\n"
      << "  uint_least32_t* top[] = {\n";
    for (auto i = static_cast<std::size_t>(0); i <= max_stack; ++i)
      out
        << "    stack[" << i << "],\n";
    out
      << "  };\n"
      << "  for (uint_least32_t* ptr = *stack + " << stack_capacity << "; ;) {\n"
      << "    uint_least32_t val;\n"
      << "    switch (scanf(\"%\" SCNuLEAST32, &val)) {\n"
      << "      case 1:\n"
      << "        if (ptr == *stack)\n"
      << "          return 1;\n"
      << "        *--ptr = val % UINT32_C(" << modulo << ");\n"
      << "        break;\n"
      << "      case 0:\n"
      << "        return 4;\n"
      << "      case EOF:\n"
      << "        while (ptr != *stack + " << stack_capacity << ")\n"
      << "          *(*top)++ = *ptr++;\n"
      << "        goto state_" << this->init << ";\n"
      << "    }\n"
      << "  }\n";
    auto n = this->states.size();
    for (auto i = static_cast<std::size_t>(0); i < n; ++i) {
      const auto& state = this->states[i];
      out << "state_" << i << ":\n";
      std::visit([&](auto s) { s.emit_c(out); }, state);
    }
    out
      << "end:\n"
      << "  while (*top != *stack)\n"
      << "    printf(\"%\" PRIuLEAST32 \"\\n\", *--*top);\n"
      << "  return 0;\n"
      << "}\n";
  }
}
