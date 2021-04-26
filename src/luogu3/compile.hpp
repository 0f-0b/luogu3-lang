#ifndef LUOGU3_COMPILE_HPP
#define LUOGU3_COMPILE_HPP

#include <luogu3/diagnostic.hpp>
#include <luogu3/program.hpp>
#include <string>
#include <vector>

namespace ud2::luogu3 {
  struct compile_result {
    std::vector<diagnostic> diags;
    program prog;
  };

  auto compile(const std::string& source) -> compile_result;
}

#endif
