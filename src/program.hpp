#ifndef LUOGU3_PROGRAM_HPP
#define LUOGU3_PROGRAM_HPP

#include <cstdint>
#include <ostream>
#include <variant>
#include <vector>

namespace ud2::luogu3 {
  constexpr auto max_states = 100000U;
  constexpr auto max_stack_size = 1000000U;
  constexpr auto modulo = UINT32_C(998244353);

  struct program {
    enum class stack {
      a,
      b,
      c
    };

    struct state_push {
      stack target;
      std::uint_least32_t val;
      std::size_t next;
    };

    struct state_pop {
      stack target;
      std::size_t next;
    };

    struct state_move {
      stack target;
      stack from;
      std::size_t next;
    };

    struct state_copy {
      stack target;
      stack from;
      std::size_t next;
    };

    struct state_add {
      stack target;
      stack left;
      stack right;
      std::size_t next;
    };

    struct state_subtract {
      stack target;
      stack left;
      stack right;
      std::size_t next;
    };

    struct state_multiply {
      stack target;
      stack left;
      stack right;
      std::size_t next;
    };

    struct state_divide {
      stack target;
      stack left;
      stack right;
      std::size_t next;
    };

    struct state_modulo {
      stack target;
      stack left;
      stack right;
      std::size_t next;
    };

    struct state_empty {
      stack target;
      std::size_t consequent;
      std::size_t alternative;
    };

    struct state_less {
      stack left;
      stack right;
      std::size_t consequent;
      std::size_t alternative;
    };

    using state = std::variant<
      std::monostate,
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

    std::vector<state> states;
    std::size_t init = 0;

    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };
}

#endif
