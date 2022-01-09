#ifndef LUOGU3_PROGRAM_HPP
#define LUOGU3_PROGRAM_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <variant>
#include <vector>

namespace ud2::luogu3 {
  constexpr auto max_states = static_cast<std::size_t>(100000);
  constexpr auto stack_capacity = static_cast<std::size_t>(1000000);
  constexpr auto modulo = UINT32_C(998244353);

  struct state_terminate {
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_push {
    std::size_t target;
    std::uint_least32_t val;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_pop {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_move {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_copy {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_add {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_subtract {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_multiply {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_divide {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_modulo {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) -> void;
  };

  struct state_empty {
    std::size_t target;
    std::size_t consequent;
    std::size_t alternative;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_less {
    std::size_t left;
    std::size_t right;
    std::size_t consequent;
    std::size_t alternative;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_prefix_sum {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_suffix_sum {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_finite_difference {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_reverse {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_sort_ascending {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_sort_descending {
    std::size_t target;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_rotate {
    std::size_t target;
    std::size_t count;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_move {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_copy {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_fill {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_iota {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_sum {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_product {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_add {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_subtract {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_multiply {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_divide {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_bulk_modulo {
    std::size_t target;
    std::size_t from;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_vector_add {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_vector_subtract {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };

  struct state_vector_multiply {
    std::size_t target;
    std::size_t left;
    std::size_t right;
    std::size_t next;
    auto max_stack() const -> std::size_t;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
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
    state_less,
    state_prefix_sum,
    state_suffix_sum,
    state_finite_difference,
    state_reverse,
    state_sort_ascending,
    state_sort_descending,
    state_rotate,
    state_bulk_move,
    state_bulk_copy,
    state_fill,
    state_iota,
    state_sum,
    state_product,
    state_bulk_add,
    state_bulk_subtract,
    state_bulk_multiply,
    state_bulk_divide,
    state_bulk_modulo,
    state_vector_add,
    state_vector_subtract,
    state_vector_multiply>;

  struct program {
    std::vector<state> states = std::vector<state>(1);
    std::size_t init = 0;
    auto emit_source(std::ostream& out) const -> void;
    auto emit_c(std::ostream& out) const -> void;
  };
}

#endif
