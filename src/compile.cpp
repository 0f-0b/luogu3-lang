#include "compile.hpp"
#include <charconv>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <system_error>
#include <unordered_map>

namespace ud2::luogu3 {
  namespace detail {
    constexpr auto is_space(unsigned char ch) noexcept -> bool {
      return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
    }

    constexpr auto is_separator(unsigned char ch) noexcept -> bool {
      return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r';
    }

    auto expect_space(compile_result& result, const char*& ptr, const char* const& start) -> bool {
      if (*ptr && !is_separator(*ptr)) {
        result.diags.push_back({ptr - start, ptr - start, "expected whitespace"});
        return false;
      }
      while (is_space(*ptr))
        ++ptr;
      return true;
    }

    auto expect_newline(compile_result& result, const char*& ptr, const char* const& start) -> bool {
      while (*ptr) {
        if (*ptr == '\n') {
          ++ptr;
          break;
        }
        if (!is_space(*ptr)) {
          result.diags.push_back({ptr - start, ptr - start, "expected newline"});
          return false;
        }
        ++ptr;
      }
      return true;
    }

    auto expect_stack(compile_result& result, const char*& ptr, const char* const& start) -> std::optional<program::stack> {
      static const auto stacks = []() -> std::unordered_map<std::string, program::stack> {
        return {
          {"A", program::stack::a},
          {"B", program::stack::b},
          {"C", program::stack::c},
        };
      }();
      auto begin = ptr;
      while (*ptr && !is_separator(*ptr))
        ++ptr;
      if (begin == ptr) {
        result.diags.push_back({ptr - start, ptr - start, "expected stack name"});
        return std::nullopt;
      }
      auto s = std::string{begin, ptr};
      if (auto fn = stacks.find(s); fn != stacks.end())
        return fn->second;
      result.diags.push_back({begin - start, ptr - start, "unknown stack name"});
      return std::nullopt;
    }

    auto expect_state(compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::size_t {
      std::size_t state;
      auto conv = std::from_chars(ptr, end, state);
      if (conv.ec == std::errc::invalid_argument) {
        result.diags.push_back({ptr - start, ptr - start, "invalid integer"});
        return -1;
      }
      if (conv.ec == std::errc::result_out_of_range) {
        result.diags.push_back({ptr - start, conv.ptr - start, "invalid state"});
        return -1;
      }
      if (state > n) {
        result.diags.push_back({ptr - start, conv.ptr - start, "state out of bounds"});
        return -1;
      }
      if (state == 0) {
        result.diags.push_back({ptr - start, conv.ptr - start, "invalid state; did you mean state 1?"});
        return -1;
      }
      ptr = conv.ptr;
      return --state;
    }

    auto parse_line(compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
      static const auto states = []() -> std::unordered_map<std::string, std::function<std::optional<program::state>(compile_result & result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end)>> {
        return {
          {"TER",
            [](compile_result& result, std::size_t, const char*& ptr, const char* const& start, const char* const&) -> std::optional<program::state> {
              if (!expect_newline(result, ptr, start))
                return std::nullopt;
              return std::monostate{};
            }},
          {"PUS",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              std::uint_least32_t val;
              {
                auto conv = std::from_chars(ptr, end, val);
                if (conv.ec == std::errc::invalid_argument) {
                  result.diags.push_back({ptr - start, ptr - start, "invalid integer"});
                  return std::nullopt;
                }
                if (val >= modulo) {
                  result.diags.push_back({ptr - start, conv.ptr - start, "value out of bounds"});
                  return std::nullopt;
                }
                ptr = conv.ptr;
              }
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_push{*target, val, next};
            }},
          {"POP",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_pop{*target, next};
            }},
          {"MOV",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto from = expect_stack(result, ptr, start);
              if (!from || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_move{*target, *from, next};
            }},
          {"CPY",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto from = expect_stack(result, ptr, start);
              if (!from || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_copy{*target, *from, next};
            }},
          {"ADD",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_add{*target, *left, *right, next};
            }},
          {"SUB",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_subtract{*target, *left, *right, next};
            }},
          {"MUL",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_multiply{*target, *left, *right, next};
            }},
          {"DIV",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_divide{*target, *left, *right, next};
            }},
          {"MOD",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto next = expect_state(result, n, ptr, start, end);
              if (!~next || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_modulo{*target, *left, *right, next};
            }},
          {"EMP",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto target = expect_stack(result, ptr, start);
              if (!target || !expect_space(result, ptr, start))
                return std::nullopt;
              auto consequent = expect_state(result, n, ptr, start, end);
              if (!~consequent || !expect_space(result, ptr, start))
                return std::nullopt;
              auto alternative = expect_state(result, n, ptr, start, end);
              if (!~alternative || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_empty{*target, consequent, alternative};
            }},
          {"CMP",
            [](compile_result& result, std::size_t n, const char*& ptr, const char* const& start, const char* const& end) -> std::optional<program::state> {
              if (!expect_space(result, ptr, start))
                return std::nullopt;
              auto right = expect_stack(result, ptr, start);
              if (!right || !expect_space(result, ptr, start))
                return std::nullopt;
              auto left = expect_stack(result, ptr, start);
              if (!left || !expect_space(result, ptr, start))
                return std::nullopt;
              auto alternative = expect_state(result, n, ptr, start, end);
              if (!~alternative || !expect_space(result, ptr, start))
                return std::nullopt;
              auto consequent = expect_state(result, n, ptr, start, end);
              if (!~consequent || !expect_newline(result, ptr, start))
                return std::nullopt;
              return program::state_less{*left, *right, consequent, alternative};
            }},
        };
      }();
      auto begin = ptr;
      while (*ptr && !is_separator(*ptr))
        ++ptr;
      if (begin == ptr) {
        result.diags.push_back({ptr - start, ptr - start, "expected state type"});
        return std::nullopt;
      }
      auto s = std::string{begin, ptr};
      if (auto fn = states.find(s); fn != states.end())
        return fn->second(result, n, ptr, start, end);
      result.diags.push_back({begin - start, ptr - start, "unknown state type"});
      return std::nullopt;
    }
  }

  auto compile(const std::string& source) -> compile_result {
    auto result = compile_result{};
    auto start = source.c_str();
    auto end = start + source.size();
    auto ptr = start;
    std::size_t n;
    {
      auto conv = std::from_chars(ptr, end, n);
      if (conv.ec == std::errc::invalid_argument) {
        result.diags.push_back({ptr - start, ptr - start, "invalid integer"});
        return result;
      }
      if (conv.ec == std::errc::result_out_of_range || n > max_states) {
        result.diags.push_back({ptr - start, conv.ptr - start, "too many states"});
        return result;
      }
      if (n == 0) {
        result.diags.push_back({ptr - start, conv.ptr - start, "too few states"});
        return result;
      }
      ptr = conv.ptr;
    }
    result.prog.states.resize(n);
    if (!detail::expect_space(result, ptr, start))
      return result;
    auto init = detail::expect_state(result, n, ptr, start, end);
    if (!~init)
      return result;
    result.prog.init = init;
    if (!detail::expect_newline(result, ptr, start))
      return result;
    for (auto i = static_cast<std::size_t>(0); i < n; ++i) {
      auto state = detail::parse_line(result, n, ptr, start, end);
      if (!state) {
        while (*ptr) {
          if (*ptr == '\n') {
            ++ptr;
            break;
          }
          ++ptr;
        }
        continue;
      }
      result.prog.states[i] = *state;
    }
    while (*ptr) {
      if (!detail::is_separator(*ptr)) {
        result.diags.push_back({ptr - start, ptr - start, "expected end of file"});
        break;
      }
      ++ptr;
    }
    return result;
  }
}
