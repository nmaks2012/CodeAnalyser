#include "cmd_options.hpp"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <gtest/gtest.h>
#include <initializer_list>
#include <ranges>
#include <string>
#include <vector>

namespace analyser::cmd::test {

using namespace analyser::cmd;

class TestCmdOptions : public ::testing::Test {
protected:
  void SetUp(std::initializer_list<std::string> list) {

    args = list;

    argv = args | std::views::transform([](const auto &arg) {
             return (char *)arg.c_str();
           }) |
           std::ranges::to<std::vector<char *>>();
  }
  std::vector<char *> argv;
  std::vector<std::string> args;
};

TEST_F(TestCmdOptions, EmptyOptions) {
  SetUp({"analyser"});
  ProgramOptions options;
  EXPECT_FALSE(options.Parse(args.size(), argv.data()));
}

TEST_F(TestCmdOptions, OneFile) {

  SetUp({"analyser", "-f", "1.py"});
  std::ofstream output("1.py");
  output << "1.py";
  output.close();

  // Тестирование парсинга командной строки
  ProgramOptions options;
  EXPECT_TRUE(options.Parse(args.size(), argv.data()));

  // Тестирование количества файлов и их содержимого
  auto files = options.GetFiles();
  EXPECT_EQ(files.size(), 1);

  std::ifstream input(*files.begin());
  std::string result;
  input >> result;

  EXPECT_EQ(result, "1.py");
}

TEST_F(TestCmdOptions, ManyFiles) {

  SetUp({"analyser", "-f", "1.py", "2.py", "3.py"});

  std::ranges::for_each(args.begin() + 2, args.end(), [](const auto &file) {
    std::ofstream output(file);
    output << file;
    output.close();
  });

  // Тестирование парсинга командной строки
  ProgramOptions options;
  EXPECT_TRUE(options.Parse(args.size(), argv.data()));

  // Тестирование количества файлов и их содержимого
  auto files = options.GetFiles();
  EXPECT_EQ(files.size(), 3);

  EXPECT_TRUE(std::ranges::all_of(files, [](const auto &file) {
    std::ifstream input(file);
    std::string result;
    input >> result;
    return result == file;
  }));
}

} // namespace analyser::cmd::test