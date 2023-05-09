#pragma once

#include <iostream>
#include <osl/strings.h>
#include <vector>

std::vector<int> ToVectorInt(std::vector<std::string> from);

osl_strings_p ToOslStrings(std::vector<std::string> from);

/**
 * @brief Split the given string according to the delimiter, which is comma in
 * default
 *
 * @param str
 * @return std::vector<std::string>
 */
std::vector<std::string> SplitByDelimiter(const std::string &str);

/**
 * @brief Remove the first character and the last character
 *
 * @param str
 * @return std::string
 */
std::string RemovePeripheral(const std::string &str);

std::string TrimSpaces(const std::string &str);