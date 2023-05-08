#pragma once

#include <matrix/matrix.h>
#include <osl/relation.h>
#include <utils/osl_int.h>
#include <vector>

class Phantom {
public:
  /**
   * @brief Construct a new Phantom object
   *
   * @param mat
   */
  Phantom(Matrix mat);

private:
  int row;
  int col;
  std::vector<std::vector<int *>> data;
};
