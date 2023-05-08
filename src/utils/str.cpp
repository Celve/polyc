#include <cstring>
#include <osl/strings.h>
#include <utils/str.h>

std::vector<int> ToVectorInt(std::vector<std::string> from) {
  std::vector<int> to;
  for (auto i : from) {
    to.push_back(std::stoi(i));
  }
  return to;
}

osl_strings_p ToOslStrings(std::vector<std::string> from) {
  osl_strings_p to = osl_strings_malloc();
  for (auto &str : from) {
    auto *cstr = str.c_str();
    osl_strings_add(to, cstr);
  }
  return to;
}