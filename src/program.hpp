#ifndef LUOGU3_PROGRAM_HPP
#define LUOGU3_PROGRAM_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <variant>
#include <vector>

namespace ud2::luogu3 {
  constexpr auto max_states = static_cast<std::size_t>(100000);
  constexpr auto stack_capacity = static_cast<std::size_t>(1000000);
  constexpr auto modulo = UINT32_C(998244353);

  namespace detail {
    inline constexpr auto source_name(std::size_t s) -> const char* {
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

    template <char... SourceName>
    struct state_unary_op {
      std::size_t target;
      std::size_t from;
      std::size_t next;

      auto max_stack() const -> std::size_t {
        return std::max(this->target, this->from);
      }

      auto emit_source(std::ostream& out) const -> void {
        (out << ... << SourceName) << ' ' << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
      }
    };

    template <char... SourceName>
    struct state_binary_op {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;

      auto max_stack() const -> std::size_t {
        return std::max({this->target, this->left, this->right});
      }

      auto emit_source(std::ostream& out) const -> void {
        (out << ... << SourceName) << ' ' << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
      }

    protected:
      auto emit_c_header(std::ostream& out) const -> void {
        out
          << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
          << "    return 1;\n"
          << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
          << "    return 3;\n";
      }

      auto emit_c_footer(std::ostream& out) const -> void {
        out
          << "  ++top[" << this->target << "];\n"
          << "  goto state_" << this->next << ";\n";
      }
    };
  }

  struct state_terminate {
    auto max_stack() const -> std::size_t {
      return 0;
    }

    auto emit_source(std::ostream& out) const -> void {
      out << "TER\n";
    }

    auto emit_c(std::ostream& out) const -> void {
      out << "  goto end;\n";
    }
  };

  struct state_push {
    std::size_t target;
    std::uint_least32_t val;
    std::size_t next;

    auto max_stack() const -> std::size_t {
      return this->target;
    }

    auto emit_source(std::ostream& out) const -> void {
      out << "PUS " << detail::source_name(this->target) << ' ' << this->val << ' ' << (this->next + 1) << '\n';
    }

    auto emit_c(std::ostream& out) const -> void {
      out
        << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
        << "    return 1;\n"
        << "  *top[" << this->target << "]++ = UINT32_C(" << this->val << ");\n"
        << "  goto state_" << this->next << ";\n";
    }
  };

  struct state_pop {
    std::size_t target;
    std::size_t next;

    auto max_stack() const -> std::size_t {
      return this->target;
    }

    auto emit_source(std::ostream& out) const -> void {
      out << "POP " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
    }

    auto emit_c(std::ostream& out) const -> void {
      out
        << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
        << "    return 2;\n"
        << "  --top[" << this->target << "];\n"
        << "  goto state_" << this->next << ";\n";
    }
  };

  struct state_move : detail::state_unary_op<'M', 'O', 'V'> {
    auto emit_c(std::ostream& out) -> void {
      out
        << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
        << "    return 1;\n"
        << "  if (top[" << this->from << "] == stack[" << this->from << "])\n"
        << "    return 2;\n"
        << "  --top[" << this->from << "];\n"
        << "  *top[" << this->target << "] = *top[" << this->from << "];\n"
        << "  ++top[" << this->target << "];\n"
        << "  goto state_" << this->next << ";\n";
    }
  };

  struct state_copy : detail::state_unary_op<'C', 'P', 'Y'> {
    auto emit_c(std::ostream& out) -> void {
      out
        << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
        << "    return 1;\n"
        << "  if (top[" << this->from << "] == stack[" << this->from << "])\n"
        << "    return 3;\n"
        << "  *top[" << this->target << "] = top[" << this->from << "][-1];\n"
        << "  ++top[" << this->target << "];\n"
        << "  goto state_" << this->next << ";\n";
    }
  };

  struct state_add : detail::state_binary_op<'A', 'D', 'D'> {
    auto emit_c(std::ostream& out) -> void {
      this->emit_c_header(out);
      out << "  *top[" << this->target << "] = (uint_least32_t) (((uint_least64_t) top[" << this->left << "][-1] + top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n";
      this->emit_c_footer(out);
    }
  };

  struct state_subtract : detail::state_binary_op<'S', 'U', 'B'> {
    auto emit_c(std::ostream& out) -> void {
      this->emit_c_header(out);
      out << "  *top[" << this->target << "] = (uint_least32_t) ((UINT64_C(" << modulo << ") + top[" << this->left << "][-1] - top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n";
      this->emit_c_footer(out);
    }
  };

  struct state_multiply : detail::state_binary_op<'M', 'U', 'L'> {
    auto emit_c(std::ostream& out) -> void {
      this->emit_c_header(out);
      out << "  *top[" << this->target << "] = (uint_least32_t) (((uint_least64_t) top[" << this->left << "][-1] * top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n";
      this->emit_c_footer(out);
    }
  };

  struct state_divide : detail::state_binary_op<'D', 'I', 'V'> {
    auto emit_c(std::ostream& out) -> void {
      this->emit_c_header(out);
      out
        << "  if (top[" << this->right << "][-1] == 0)\n"
        << "    return 4;\n"
        << "  *top[" << this->target << "] = top[" << this->left << "][-1] / top[" << this->right << "][-1];\n";
      this->emit_c_footer(out);
    }
  };

  struct state_modulo : detail::state_binary_op<'M', 'O', 'D'> {
    auto emit_c(std::ostream& out) -> void {
      this->emit_c_header(out);
      out
        << "  if (top[" << this->right << "][-1] == 0)\n"
        << "    return 4;\n"
        << "  *top[" << this->target << "] = top[" << this->left << "][-1] % top[" << this->right << "][-1];\n";
      this->emit_c_footer(out);
    }
  };

  struct state_empty {
    std::size_t target;
    std::size_t consequent;
    std::size_t alternative;

    auto max_stack() const -> std::size_t {
      return this->target;
    }

    auto emit_source(std::ostream& out) const -> void {
      out << "EMP " << detail::source_name(this->target) << ' ' << (this->consequent + 1) << ' ' << (this->alternative + 1) << '\n';
    }

    auto emit_c(std::ostream& out) const -> void {
      out
        << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
        << "    goto state_" << this->consequent << ";\n"
        << "  else\n"
        << "    goto state_" << this->alternative << ";\n";
    }
  };

  struct state_less {
    std::size_t left;
    std::size_t right;
    std::size_t consequent;
    std::size_t alternative;

    auto max_stack() const -> std::size_t {
      return std::max(this->left, this->right);
    }

    auto emit_source(std::ostream& out) const -> void {
      out << "CMP " << detail::source_name(this->right) << ' ' << detail::source_name(this->left) << ' ' << (this->alternative + 1) << ' ' << (this->consequent + 1) << '\n';
    }

    auto emit_c(std::ostream& out) const -> void {
      out
        << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
        << "    return 3;\n"
        << "  if (top[" << this->left << "][-1] < top[" << this->right << "][-1])\n"
        << "    goto state_" << this->consequent << ";\n"
        << "  else\n"
        << "    goto state_" << this->alternative << ";\n";
    }
  };

  using state = std::variant<
    state_terminate,
    state_push,
    state_pop,
    state_move,
    state_copy,
    state_add,
    state_subtract,
    state_multiply,
    state_divide,
    state_modulo,
    state_empty,
    state_less>;

  struct program {
    std::vector<state> states = std::vector<state>(1);
    std::size_t init = 0;

    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };
}

#endif
