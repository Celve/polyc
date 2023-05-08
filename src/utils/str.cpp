#include <utils/str.h>

std::vector<int> ToVectorInt(std::vector<std::string> from) {
  std::vector<int> to;
  for (auto i : from) {
    to.push_back(std::stoi(i));
  }
  return to;
}