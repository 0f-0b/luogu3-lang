#include <config.h>
#include <argagg/argagg.hpp>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <luogu3/compile.hpp>
#include <string>

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
  std::string filename;
  std::string output;
  bool format;
  {
    auto arg_parser = argagg::parser{{
      {"version", {"-V", "--version"}, "show the version", 0},
      {"output", {"-o", "--output"}, "output file (default: -)", 1},
      {"format", {"-f", "--format"}, "format the code instead of compiling it", 0},
      {"help", {"-h", "--help"}, "show this help message", 0},
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
    output = args["output"].as<std::string>("-");
    format = args["format"];
  }
  errno = 0;
  std::string source;
  {
    auto is_std = filename == "-";
    auto& in = is_std ? std::cin : *new std::ifstream{filename};
    if (is_std)
      filename = "<stdin>";
    source.assign(std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{});
    if (!is_std)
      delete &in;
  }
  if (errno) {
    std::cerr << filename << ": " << std::strerror(errno) << '\n';
    return 1;
  }
  auto result = ud2::luogu3::compile(source);
  auto error = ud2::luogu3::print_diagnostics(std::cerr, result.diags, filename.c_str(), source.c_str());
  errno = 0;
  {
    auto is_std = output == "-";
    auto& out = is_std ? std::cout : *new std::ofstream{output};
    if (is_std)
      output = "<stdout>";
    if (format)
      result.prog.emit_source(out);
    else
      result.prog.emit_c(out);
    if (!is_std)
      delete &out;
  }
  if (errno) {
    std::cerr << output << ": " << std::strerror(errno) << '\n';
    return 1;
  }
  return error;
}
