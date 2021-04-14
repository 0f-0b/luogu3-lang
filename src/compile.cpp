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
      return is_space(ch) || ch == '\n';
    }

    auto skip_space(const char*& ptr) -> void {
      while (is_space(*ptr))
        ++ptr;
    }

    auto skip_separator(const char*& ptr) -> void {
      while (is_separator(*ptr))
        ++ptr;
    }

    auto skip_line(const char*& ptr) -> void {
      while (*ptr && *ptr++ != '\n') { }
    }

    auto expect_space(compile_result& result, const char*& ptr, const char* start) -> bool {
      if (*ptr && !is_separator(*ptr)) {
        result.diags.push_back({ptr - start, ptr - start, "expected whitespace"});
        return false;
      }
      skip_space(ptr);
      return true;
    }

    auto expect_newline(compile_result& result, const char*& ptr, const char* start) -> bool {
      skip_space(ptr);
      if (*ptr) {
        if (*ptr != '\n') {
          result.diags.push_back({ptr - start, ptr - start, "expected newline"});
          return false;
        }
        ++ptr;
      }
      return true;
    }

    auto expect_eof(compile_result& result, const char*& ptr, const char* start) -> bool {
      skip_separator(ptr);
      if (*ptr) {
        result.diags.push_back({ptr - start, ptr - start, "expected end of file"});
        return false;
      }
      return true;
    }

    auto expect_stack(compile_result& result, const char*& ptr, const char* start) -> std::optional<std::size_t> {
      static const auto stacks = std::unordered_map<std::string, std::size_t>{
        {"A", 0},
        {"B", 1},
        {"C", 2},
      };
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

    auto expect_state(compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::size_t {
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

    auto parse_line(compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
      static const auto states = std::unordered_map<std::string, std::function<std::optional<state>(compile_result & result, std::size_t n, const char*& ptr, const char* start, const char* end)>>{
        {"TER",
          [](compile_result& result, std::size_t, const char*& ptr, const char* start, const char*) -> std::optional<state> {
            if (!expect_newline(result, ptr, start))
              return std::nullopt;
            return state_terminate{};
          }},
        {"PUS",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_push{*target, val, next};
          }},
        {"POP",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
            if (!expect_space(result, ptr, start))
              return std::nullopt;
            auto target = expect_stack(result, ptr, start);
            if (!target || !expect_space(result, ptr, start))
              return std::nullopt;
            auto next = expect_state(result, n, ptr, start, end);
            if (!~next || !expect_newline(result, ptr, start))
              return std::nullopt;
            return state_pop{*target, next};
          }},
        {"MOV",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_move{*target, *from, next};
          }},
        {"CPY",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_copy{*target, *from, next};
          }},
        {"ADD",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_add{*target, *left, *right, next};
          }},
        {"SUB",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_subtract{*target, *left, *right, next};
          }},
        {"MUL",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_multiply{*target, *left, *right, next};
          }},
        {"DIV",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_divide{*target, *left, *right, next};
          }},
        {"MOD",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_modulo{*target, *left, *right, next};
          }},
        {"EMP",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_empty{*target, consequent, alternative};
          }},
        {"CMP",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
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
            return state_less{*left, *right, consequent, alternative};
          }},
        {"T00",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
            if (!expect_space(result, ptr, start))
              return std::nullopt;
            auto target = expect_stack(result, ptr, start);
            if (!target || !expect_space(result, ptr, start))
              return std::nullopt;
            auto next = expect_state(result, n, ptr, start, end);
            if (!~next || !expect_newline(result, ptr, start))
              return std::nullopt;
            return state_prefix_sum{*target, next};
          }},
        {"T01",
          [](compile_result& result, std::size_t n, const char*& ptr, const char* start, const char* end) -> std::optional<state> {
            if (!expect_space(result, ptr, start))
              return std::nullopt;
            auto target = expect_stack(result, ptr, start);
            if (!target || !expect_space(result, ptr, start))
              return std::nullopt;
            auto next = expect_state(result, n, ptr, start, end);
            if (!~next || !expect_newline(result, ptr, start))
              return std::nullopt;
            return state_suffix_sum{*target, next};
          }},
      };
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
    detail::skip_space(ptr);
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
      detail::skip_separator(ptr);
      auto state = detail::parse_line(result, n, ptr, start, end);
      if (state)
        result.prog.states[i] = *state;
      else
        detail::skip_line(ptr);
    }
    detail::expect_eof(result, ptr, start);
    return result;
  }
}
