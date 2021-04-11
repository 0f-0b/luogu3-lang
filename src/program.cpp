#include "program.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ud2::luogu3 {
  namespace detail {
    constexpr auto source_name(std::size_t s) -> const char* {
      switch (s) {
        case 0:
          return "A";
        case 1:
          return "B";
        case 2:
          return "C";
        default:
          throw std::invalid_argument{"unrepresentable stack"};
      }
    }
  }

  auto program::emit_source(std::ostream& out) const -> void {
    out << this->states.size() << ' ' << (this->init + 1) << '\n';
    for (const auto& state : this->states) {
      struct {
        std::ostream& out;

        auto operator()(std::monostate) -> void {
          this->out << "TER\n";
        }

        auto operator()(state_push s) -> void {
          this->out << "PUS " << detail::source_name(s.target) << ' ' << s.val << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_pop s) -> void {
          this->out << "POP " << detail::source_name(s.target) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_move s) -> void {
          this->out << "MOV " << detail::source_name(s.target) << ' ' << detail::source_name(s.from) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_copy s) -> void {
          this->out << "CPY " << detail::source_name(s.target) << ' ' << detail::source_name(s.from) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_add s) -> void {
          this->out << "ADD " << detail::source_name(s.target) << ' ' << detail::source_name(s.left) << ' ' << detail::source_name(s.right) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_subtract s) -> void {
          this->out << "SUB " << detail::source_name(s.target) << ' ' << detail::source_name(s.left) << ' ' << detail::source_name(s.right) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_multiply s) -> void {
          this->out << "MUL " << detail::source_name(s.target) << ' ' << detail::source_name(s.left) << ' ' << detail::source_name(s.right) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_divide s) -> void {
          this->out << "DIV " << detail::source_name(s.target) << ' ' << detail::source_name(s.left) << ' ' << detail::source_name(s.right) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_modulo s) -> void {
          this->out << "MOD " << detail::source_name(s.target) << ' ' << detail::source_name(s.left) << ' ' << detail::source_name(s.right) << ' ' << (s.next + 1) << '\n';
        }

        auto operator()(state_empty s) -> void {
          this->out << "EMP " << detail::source_name(s.target) << ' ' << (s.consequent + 1) << ' ' << (s.alternative + 1) << '\n';
        }

        auto operator()(state_less s) -> void {
          this->out << "CMP " << detail::source_name(s.right) << ' ' << detail::source_name(s.left) << ' ' << (s.alternative + 1) << ' ' << (s.consequent + 1) << '\n';
        }
      } fn{out};
      std::visit(fn, state);
    }
  }

  auto program::emit_c(std::ostream& out) const -> void {
    std::size_t max_stack = 0;
    for (const auto& state : this->states) {
      struct {
        std::size_t& max_stack;

        auto operator()(std::monostate) -> void { }

        auto operator()(state_push s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
        }

        auto operator()(state_pop s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
        }

        auto operator()(state_move s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.from > this->max_stack)
            this->max_stack = s.from;
        }

        auto operator()(state_copy s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.from > this->max_stack)
            this->max_stack = s.from;
        }

        auto operator()(state_add s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }

        auto operator()(state_subtract s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }

        auto operator()(state_multiply s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }

        auto operator()(state_divide s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }

        auto operator()(state_modulo s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }

        auto operator()(state_empty s) -> void {
          if (s.target > this->max_stack)
            this->max_stack = s.target;
        }

        auto operator()(state_less s) -> void {
          if (s.left > this->max_stack)
            this->max_stack = s.left;
          if (s.right > this->max_stack)
            this->max_stack = s.right;
        }
      } fn{max_stack};
      std::visit(fn, state);
    }
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
      struct {
        std::ostream& out;

        auto operator()(std::monostate) -> void {
          this->out
            << "  goto end;\n";
        }

        auto operator()(state_push s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  *top[" << s.target << "]++ = UINT32_C(" << s.val << ");\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_pop s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "])\n"
            << "    return 2;\n"
            << "  --top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_move s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.from << "] == stack[" << s.from << "])\n"
            << "    return 2;\n"
            << "  --top[" << s.from << "];\n"
            << "  *top[" << s.target << "] = *top[" << s.from << "];\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_copy s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.from << "] == stack[" << s.from << "])\n"
            << "    return 3;\n"
            << "  *top[" << s.target << "] = top[" << s.from << "][-1];\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_add s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  *top[" << s.target << "] = (uint_least32_t) (((uint_least64_t) top[" << s.left << "][-1] + top[" << s.right << "][-1]) % UINT32_C(" << modulo << "));\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_subtract s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  *top[" << s.target << "] = (uint_least32_t) ((UINT64_C(" << modulo << ") + top[" << s.left << "][-1] - top[" << s.right << "][-1]) % UINT32_C(" << modulo << "));\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_multiply s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  *top[" << s.target << "] = (uint_least32_t) (((uint_least64_t) top[" << s.left << "][-1] * top[" << s.right << "][-1]) % UINT32_C(" << modulo << "));\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_divide s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  if (top[" << s.right << "][-1] == 0)\n"
            << "    return 4;\n"
            << "  *top[" << s.target << "] = top[" << s.left << "][-1] / top[" << s.right << "][-1];\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_modulo s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "] + " << stack_capacity << ")\n"
            << "    return 1;\n"
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  if (top[" << s.right << "][-1] == 0)\n"
            << "    return 4;\n"
            << "  *top[" << s.target << "] = top[" << s.left << "][-1] % top[" << s.right << "][-1];\n"
            << "  ++top[" << s.target << "];\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_empty s) -> void {
          this->out
            << "  if (top[" << s.target << "] == stack[" << s.target << "])\n"
            << "    goto state_" << s.consequent << ";\n"
            << "  else\n"
            << "    goto state_" << s.alternative << ";\n";
        }

        auto operator()(state_less s) -> void {
          this->out
            << "  if (top[" << s.left << "] == stack[" << s.left << "] || top[" << s.right << "] == stack[" << s.right << "])\n"
            << "    return 3;\n"
            << "  if (top[" << s.left << "][-1] < top[" << s.right << "][-1])\n"
            << "    goto state_" << s.consequent << ";\n"
            << "  else\n"
            << "    goto state_" << s.alternative << ";\n";
        }
      } fn{out};
      std::visit(fn, state);
    }
    out
      << "end:\n"
      << "  while (*top != *stack)\n"
      << "    printf(\"%\" PRIuLEAST32 \"\\n\", *--*top);\n"
      << "  return 0;\n"
      << "}\n";
  }
}
