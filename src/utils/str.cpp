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

std::vector<std::string> SplitByDelimiter(const std::string &str) {
  std::vector<std::string> result;
  std::string curr;
  auto inside_brackets = 0;
  auto inside_parenthesis = 0;
  for (auto c : str) {
    if (c == '[') {
      inside_brackets++;
    } else if (c == '(') {
      inside_parenthesis++;
    } else if (c == ']') {
      inside_brackets--;
    } else if (c == ')') {
      inside_parenthesis--;
    }

    if (c == ',' && inside_parenthesis == 0 && inside_brackets == 0) {
      result.push_back(curr);
      curr.clear();
    } else if (c != ' ') {
      curr += c;
    }
  }

  if (!curr.empty()) {
    result.push_back(curr);
  }

  return result;
}

std::string RemovePeripheral(const std::string &str) {
  return str.substr(1, str.size() - 2);
}

std::string TrimSpaces(const std::string &str) {
  auto start = str.find_first_not_of(' ');
  auto end = str.find_last_not_of(' ');
  return str.substr(start, end - start + 1);
}