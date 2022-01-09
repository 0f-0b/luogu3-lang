#include <luogu3/program.hpp>
#include <ostream>
#include <stdexcept>

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

  auto state_terminate::max_stack() const -> std::size_t {
    return 0;
  }

  auto state_terminate::emit_source(std::ostream& out) const -> void {
    out << "TER\n";
  }

  auto state_terminate::emit_c(std::ostream& out) const -> void {
    out << "  goto end;\n";
  }

  auto state_push::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_push::emit_source(std::ostream& out) const -> void {
    out << "PUS " << detail::source_name(this->target) << ' ' << this->val << ' ' << (this->next + 1) << '\n';
  }

  auto state_push::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  *top[" << this->target << "]++ = UINT32_C(" << this->val << ");\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_pop::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_pop::emit_source(std::ostream& out) const -> void {
    out << "POP " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_pop::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
      << "    return 2;\n"
      << "  --top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_move::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_move::emit_source(std::ostream& out) const -> void {
    out << "MOV" << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_move::emit_c(std::ostream& out) -> void {
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

  auto state_copy::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_copy::emit_source(std::ostream& out) const -> void {
    out << "CPY " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_copy::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->from << "] == stack[" << this->from << "])\n"
      << "    return 3;\n"
      << "  *top[" << this->target << "] = top[" << this->from << "][-1];\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_add::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_add::emit_source(std::ostream& out) const -> void {
    out << "ADD " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_add::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  *top[" << this->target << "] = (uint_least32_t) (((uint_least64_t) top[" << this->left << "][-1] + top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_subtract::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_subtract::emit_source(std::ostream& out) const -> void {
    out << "SUB " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_subtract::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  *top[" << this->target << "] = (uint_least32_t) ((UINT64_C(" << modulo << ") + top[" << this->left << "][-1] - top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_multiply::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_multiply::emit_source(std::ostream& out) const -> void {
    out << "MUL " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_multiply::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  *top[" << this->target << "] = (uint_least32_t) (((uint_least64_t) top[" << this->left << "][-1] * top[" << this->right << "][-1]) % UINT32_C(" << modulo << "));\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_divide::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_divide::emit_source(std::ostream& out) const -> void {
    out << "DIV " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_divide::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  if (top[" << this->right << "][-1] == 0)\n"
      << "    return 4;\n"
      << "  *top[" << this->target << "] = top[" << this->left << "][-1] / top[" << this->right << "][-1];\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_modulo::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_modulo::emit_source(std::ostream& out) const -> void {
    out << "MOD " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_modulo::emit_c(std::ostream& out) -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "] + " << stack_capacity << ")\n"
      << "    return 1;\n"
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  if (top[" << this->right << "][-1] == 0)\n"
      << "    return 4;\n"
      << "  *top[" << this->target << "] = top[" << this->left << "][-1] % top[" << this->right << "][-1];\n"
      << "  ++top[" << this->target << "];\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_empty::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_empty::emit_source(std::ostream& out) const -> void {
    out << "EMP " << detail::source_name(this->target) << ' ' << (this->consequent + 1) << ' ' << (this->alternative + 1) << '\n';
  }

  auto state_empty::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
      << "    goto state_" << this->consequent << ";\n"
      << "  else\n"
      << "    goto state_" << this->alternative << ";\n";
  }

  auto state_less::max_stack() const -> std::size_t {
    return std::max(this->left, this->right);
  }

  auto state_less::emit_source(std::ostream& out) const -> void {
    out << "CMP " << detail::source_name(this->right) << ' ' << detail::source_name(this->left) << ' ' << (this->alternative + 1) << ' ' << (this->consequent + 1) << '\n';
  }

  auto state_less::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->left << "] == stack[" << this->left << "] || top[" << this->right << "] == stack[" << this->right << "])\n"
      << "    return 3;\n"
      << "  if (top[" << this->left << "][-1] < top[" << this->right << "][-1])\n"
      << "    goto state_" << this->consequent << ";\n"
      << "  else\n"
      << "    goto state_" << this->alternative << ";\n";
  }

  auto state_prefix_sum::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_prefix_sum::emit_source(std::ostream& out) const -> void {
    out << "T00 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_prefix_sum::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
      << "    return 3;\n"
      << "  {\n"
      << "    uint_least32_t k = top[" << this->target << "][-1];\n"
      << "    if (top[" << this->target << "] - 1 - stack[" << this->target << "] < k)\n"
      << "      return 3;\n"
      << "    uint_least32_t* ptr = top[" << this->target << "] - 1 - k;\n"
      << "    for (uint_least32_t i = 1; i < k; ++i)\n"
      << "      ptr[k - i - 1] += ptr[k - i];\n"
      << "  }\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_suffix_sum::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_suffix_sum::emit_source(std::ostream& out) const -> void {
    out << "T01 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_suffix_sum::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
      << "    return 3;\n"
      << "  {\n"
      << "    uint_least32_t k = top[" << this->target << "][-1];\n"
      << "    if (top[" << this->target << "] - 1 - stack[" << this->target << "] < k)\n"
      << "      return 3;\n"
      << "    uint_least32_t* ptr = top[" << this->target << "] - 1 - k;\n"
      << "    for (uint_least32_t i = 1; i < k; ++i)\n"
      << "      ptr[i] += ptr[i - 1];\n"
      << "  }\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_finite_difference::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_finite_difference::emit_source(std::ostream& out) const -> void {
    out << "T02 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_finite_difference::emit_c(std::ostream& out) const -> void {
    out
      << "  if (top[" << this->target << "] == stack[" << this->target << "])\n"
      << "    return 3;\n"
      << "  {\n"
      << "    uint_least32_t k = top[" << this->target << "][-1];\n"
      << "    if (top[" << this->target << "] - 1 - stack[" << this->target << "] < k)\n"
      << "      return 3;\n"
      << "    uint_least32_t* ptr = top[" << this->target << "] - 1 - k;\n"
      << "    for (uint_least32_t i = 1; i < k; ++i)\n"
      << "      ptr[i - 1] -= ptr[i];\n"
      << "  }\n"
      << "  goto state_" << this->next << ";\n";
  }

  auto state_reverse::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_reverse::emit_source(std::ostream& out) const -> void {
    out << "T03 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_reverse::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_sort_ascending::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_sort_ascending::emit_source(std::ostream& out) const -> void {
    out << "T04 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_sort_ascending::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_sort_descending::max_stack() const -> std::size_t {
    return this->target;
  }

  auto state_sort_descending::emit_source(std::ostream& out) const -> void {
    out << "T05 " << detail::source_name(this->target) << ' ' << (this->next + 1) << '\n';
  }

  auto state_sort_descending::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_rotate::max_stack() const -> std::size_t {
    return std::max(this->target, this->count);
  }

  auto state_rotate::emit_source(std::ostream& out) const -> void {
    out << "T06 " << detail::source_name(this->target) << ' ' << detail::source_name(this->count) << ' ' << (this->next + 1) << '\n';
  }

  auto state_rotate::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_move::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_move::emit_source(std::ostream& out) const -> void {
    out << "T07 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_move::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_copy::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_copy::emit_source(std::ostream& out) const -> void {
    out << "T08 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_copy::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_fill::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_fill::emit_source(std::ostream& out) const -> void {
    out << "T09 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_fill::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_iota::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_iota::emit_source(std::ostream& out) const -> void {
    out << "T10 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_iota::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_sum::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_sum::emit_source(std::ostream& out) const -> void {
    out << "T11 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_sum::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_product::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_product::emit_source(std::ostream& out) const -> void {
    out << "T12 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_product::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_add::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_add::emit_source(std::ostream& out) const -> void {
    out << "T14 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_add::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_subtract::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_subtract::emit_source(std::ostream& out) const -> void {
    out << "T15 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_subtract::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_multiply::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_multiply::emit_source(std::ostream& out) const -> void {
    out << "T16 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_multiply::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_divide::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_divide::emit_source(std::ostream& out) const -> void {
    out << "T17 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_divide::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_bulk_modulo::max_stack() const -> std::size_t {
    return std::max(this->target, this->from);
  }

  auto state_bulk_modulo::emit_source(std::ostream& out) const -> void {
    out << "T18 " << detail::source_name(this->target) << ' ' << detail::source_name(this->from) << ' ' << (this->next + 1) << '\n';
  }

  auto state_bulk_modulo::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_vector_add::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_vector_add::emit_source(std::ostream& out) const -> void {
    out << "T19 " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_vector_add::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_vector_subtract::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_vector_subtract::emit_source(std::ostream& out) const -> void {
    out << "T20 " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_vector_subtract::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

  auto state_vector_multiply::max_stack() const -> std::size_t {
    return std::max({this->target, this->left, this->right});
  }

  auto state_vector_multiply::emit_source(std::ostream& out) const -> void {
    out << "T21 " << detail::source_name(this->target) << ' ' << detail::source_name(this->left) << ' ' << detail::source_name(this->right) << ' ' << (this->next + 1) << '\n';
  }

  auto state_vector_multiply::emit_c(std::ostream& out) const -> void {
    out
      << "  fputs(\"unimplemented\\n\", stderr);\n"
      << "  abort();\n";
  }

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
      << "#include <stdlib.h>\n"
      << "\n"
      << "int main(void) {\n"
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
