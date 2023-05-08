#include <matrix/phantom.h>

Phantom::Phantom(Matrix mat) {
  row = mat.row;
  col = mat.col;
  data = std::vector<std::vector<int *>>(row, std::vector<int *>(col));
  for (int i = 0; i < row; i++) {
    data[i] = std::vector<int *>(col);
    for (int j = 0; j < col; j++) {
      data[i][j] = &mat.data[i][j];
    }
  }
}