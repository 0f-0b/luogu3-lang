#include "program.hpp"
#include <string>
#include <unordered_map>

namespace ud2::luogu3 {
  namespace detail {
    constexpr auto source_name(program::stack s) -> const char* {
      switch (s) {
        case program::stack::a:
          return "A";
        case program::stack::b:
          return "B";
        case program::stack::c:
          return "C";
        default:
          return nullptr;
      }
    }

    constexpr auto c_name(program::stack s) -> const char* {
      switch (s) {
        case program::stack::a:
          return "a";
        case program::stack::b:
          return "b";
        case program::stack::c:
          return "c";
        default:
          return nullptr;
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
    out
      << "#include <inttypes.h>\n"
      << "#include <stdio.h>\n"
      << "\n"
      << "int main() {\n";
    for (auto s : {program::stack::a, program::stack::b, program::stack::c})
      out
        << "  static uint_least32_t " << detail::c_name(s) << "[" << max_stack_size << "];\n"
        << "  uint_least32_t* " << detail::c_name(s) << "_top = " << detail::c_name(s) << ";\n";
    out
      << "  for (unsigned long i = " << max_stack_size << "; i; i--)\n"
      << "    if (scanf(\"%\" SCNuLEAST32, " << detail::c_name(program::stack::a) << "_top) == 1)\n"
      << "      ++" << detail::c_name(program::stack::a) << "_top;\n"
      << "    else\n"
      << "      break;\n"
      << "  goto state_" << this->init << ";\n";
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
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  *" << detail::c_name(s.target) << "_top++ = " << s.val << ";\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_pop s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << ")\n"
            << "    return 2;\n"
            << "  --" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_move s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.from) << "_top == " << detail::c_name(s.from) << ")\n"
            << "    return 2;\n"
            << "  --" << detail::c_name(s.from) << "_top;\n"
            << "  *" << detail::c_name(s.target) << "_top = *" << detail::c_name(s.from) << "_top;\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_copy s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.from) << "_top == " << detail::c_name(s.from) << ")\n"
            << "    return 2;\n"
            << "  *" << detail::c_name(s.target) << "_top = " << detail::c_name(s.from) << "_top[-1];\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_add s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  *" << detail::c_name(s.target) << "_top = (" << detail::c_name(s.left) << "_top[-1] + " << detail::c_name(s.right) << "_top[-1]) % " << modulo << ";\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_subtract s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  *" << detail::c_name(s.target) << "_top = (" << modulo << " + " << detail::c_name(s.left) << "_top[-1] - " << detail::c_name(s.right) << "_top[-1]) % " << modulo << ";\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_multiply s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  *" << detail::c_name(s.target) << "_top = (" << detail::c_name(s.left) << "_top[-1] * " << detail::c_name(s.right) << "_top[-1]) % " << modulo << ";\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_divide s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  if (" << detail::c_name(s.right) << "_top[-1] == 0)\n"
            << "    return 3;\n"
            << "  *" << detail::c_name(s.target) << "_top = " << detail::c_name(s.left) << "_top[-1] / " << detail::c_name(s.right) << "_top[-1];\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_modulo s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << " + " << max_stack_size << ")\n"
            << "    return 1;\n"
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  if (" << detail::c_name(s.right) << "_top[-1] == 0)\n"
            << "    return 3;\n"
            << "  *" << detail::c_name(s.target) << "_top = " << detail::c_name(s.left) << "_top[-1] % " << detail::c_name(s.right) << "_top[-1];\n"
            << "  ++" << detail::c_name(s.target) << "_top;\n"
            << "  goto state_" << s.next << ";\n";
        }

        auto operator()(state_empty s) -> void {
          this->out
            << "  if (" << detail::c_name(s.target) << "_top == " << detail::c_name(s.target) << ")\n"
            << "    goto state_" << s.consequent << ";\n"
            << "  else\n"
            << "    goto state_" << s.alternative << ";\n";
        }

        auto operator()(state_less s) -> void {
          this->out
            << "  if (" << detail::c_name(s.left) << "_top == " << detail::c_name(s.left) << " || " << detail::c_name(s.right) << "_top == " << detail::c_name(s.right) << ")\n"
            << "    return 2;\n"
            << "  if (" << detail::c_name(s.left) << "_top[-1] < " << detail::c_name(s.right) << "_top[-1])\n"
            << "    goto state_" << s.consequent << ";\n"
            << "  else\n"
            << "    goto state_" << s.alternative << ";\n";
        }
      } fn{out};
      std::visit(fn, state);
    }
    out
      << "end:\n"
      << "  while (" << detail::c_name(program::stack::a) << "_top != " << detail::c_name(program::stack::a) << ")\n"
      << "    printf(\"%\" PRIuLEAST32 \"\\n\", *--" << detail::c_name(program::stack::a) << "_top);\n"
      << "  return 0;\n"
      << "}\n";
  }
}
