#ifndef LUOGU3_PROGRAM_HPP
#define LUOGU3_PROGRAM_HPP

#include <cstdint>
#include <ostream>
#include <variant>
#include <vector>

namespace ud2::luogu3 {
  constexpr auto max_states = static_cast<std::size_t>(100000);
  constexpr auto stack_capacity = static_cast<std::size_t>(1000000);
  constexpr auto modulo = UINT32_C(998244353);

  struct program {
    struct state_push {
      std::size_t target;
      std::uint_least32_t val;
      std::size_t next;
    };

    struct state_pop {
      std::size_t target;
      std::size_t next;
    };

    struct state_move {
      std::size_t target;
      std::size_t from;
      std::size_t next;
    };

    struct state_copy {
      std::size_t target;
      std::size_t from;
      std::size_t next;
    };

    struct state_add {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;
    };

    struct state_subtract {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;
    };

    struct state_multiply {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;
    };

    struct state_divide {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;
    };

    struct state_modulo {
      std::size_t target;
      std::size_t left;
      std::size_t right;
      std::size_t next;
    };

    struct state_empty {
      std::size_t target;
      std::size_t consequent;
      std::size_t alternative;
    };

    struct state_less {
      std::size_t left;
      std::size_t right;
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

    std::vector<state> states = std::vector<state>(1);
    std::size_t init = 0;

    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };
}

#endif
