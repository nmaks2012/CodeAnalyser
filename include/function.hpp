#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <filesystem>

#include "file.hpp"

namespace fs = std::filesystem;
namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyser::function {

struct Function {
    std::string filename;
    std::optional<std::string> class_name;
    std::string name;
    std::string ast;
};

struct FunctionExtractor {
    std::vector<Function> Get(const analyser::file::File& file);

private:
    struct Position {
        size_t line;
        size_t col;
    };

    struct FunctionNameLocation {
        Position start;
        Position end;
        std::string name;
    };

    struct ClassInfo {
        std::string name;
        Position start;
        Position end;
    };

    FunctionNameLocation GetNameLocation(const std::string& function_ast);
    std::string GetNameFromSource(const std::string& function_ast,
                                  const std::vector<std::string>& lines);
    std::optional<ClassInfo> FindEnclosingClass(const std::string& ast,
                                                const FunctionNameLocation& func_loc);
    std::string GetClassNameFromSource(const ClassInfo& class_info,
                                       const std::vector<std::string>& lines);
};

} // namespace analyser::function

template <>
struct std::formatter<analyser::function::Function> : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const analyser::function::Function &func, FormatContext &ctx) const {
    std::string out = "{ filename: " + func.filename + ", class: ";
    if (func.class_name.has_value()) {
      out += func.class_name.value();
    } else {
      out += "None";
    }
    out += ", name: " + func.name + " }";
    return formatter<std::string>::format(out, ctx);
  }
};
