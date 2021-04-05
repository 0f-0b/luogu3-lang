#ifndef LUOGU3_COMPILE_HPP
#define LUOGU3_COMPILE_HPP

#include <string>
#include <vector>
#include "diagnostic.hpp"
#include "program.hpp"

namespace ud2::luogu3 {
  struct compile_result {
    std::vector<diagnostic> diags;
    program prog;
  };

  auto compile(const std::string& source) -> compile_result;
}

#endif
