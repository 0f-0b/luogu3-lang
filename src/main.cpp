#include <config.h>
#include <argagg/argagg.hpp>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "compile.hpp"

template <typename CharT, typename Traits>
auto read_fully(std::basic_istream<CharT, Traits>& in) -> std::basic_string<CharT> {
  return {std::istreambuf_iterator<CharT>{in}, std::istreambuf_iterator<CharT>{}};
}

struct help_impl {
  const char* name;
  const argagg::parser& arg_parser;

  friend auto operator<<(std::ostream& out, help_impl s) -> std::ostream& {
    return out
      << "Usage: " << s.name << " [options] <file>\n\nOptions:\n"
      << s.arg_parser;
  };
};

auto main(int argc, char* argv[]) -> int {
  std::ios_base::sync_with_stdio(false);
  const char* filename;
  bool format;
  {
    auto arg_parser = argagg::parser{{
      {"version", {"-V", "--version"}, "shows the version", 0},
      {"format", {"-f", "--format"}, "formats the code instead of compiling it", 0},
      {"help", {"-h", "--help"}, "shows this help message", 0},
    }};
    auto help = help_impl{*argv, arg_parser};
    argagg::parser_results args;
    try {
      args = arg_parser.parse(argc, argv);
    } catch (...) {
      std::cerr << help;
      return 2;
    }
    if (args["help"]) {
      std::cerr
        << help << "\n"
        << "Code emitted by the compiler should be in C99/C++11.\n"
        << "Report bugs at <" PACKAGE_BUGREPORT ">.\n";
      return 0;
    }
    if (args["version"]) {
      std::cerr << PACKAGE_VERSION "\n";
      return 0;
    }
    if (args.pos.size() < 1) {
      std::cerr << help;
      return 2;
    }
    filename = args.pos[0];
    format = args["format"];
  }
  errno = 0;
  std::string source = [&]() -> std::string {
    if (std::strcmp(filename, "-")) {
      auto in = std::ifstream{filename};
      return read_fully(in);
    }
    filename = "<stdin>";
    return read_fully(std::cin);
  }();
  if (errno) {
    std::cerr << filename << ": " << std::strerror(errno) << '\n';
    return 1;
  }
  auto result = ud2::luogu3::compile(source);
  auto error = ud2::luogu3::print_diagnostics(std::cerr, result.diags, filename, source.c_str());
  if (format)
    result.prog.emit_source(std::cout);
  else
    result.prog.emit_c(std::cout);
  return error;
}
