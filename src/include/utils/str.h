#pragma once

#include <iostream>
#include <osl/strings.h>
#include <vector>

std::vector<int> ToVectorInt(std::vector<std::string> from);

osl_strings_p ToOslStrings(std::vector<std::string> from);