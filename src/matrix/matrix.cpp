#include <iostream>
#include <matrix/matrix.h>

Matrix::Matrix(osl_relation_p ptr) {
  row = ptr->nb_rows;
  col = ptr->nb_columns;
  data.resize(row);
  for (int i = 0; i < row; i++) {
    data[i].resize(col);
    for (int j = 0; j < col; j++) {
      data[i][j] = OslIntRead(ptr->m[i][j], ptr->precision);
    }
  }
}

Matrix::Matrix(int row, int col) {
  this->row = row;
  this->col = col;
  data.resize(row);
  for (int i = 0; i < row; i++) {
    data[i].resize(col);
  }
}

Matrix Matrix::SubMatrix(int row_start, int row_end, int col_start,
                         int col_end) {
  Matrix sub(row_end - row_start + 1, col_end - col_start + 1);
  for (int i = row_start; i <= row_end; i++) {
    for (int j = col_start; j <= col_end; j++) {
      sub.data[i - row_start][j - col_start] = data[i][j];
    }
  }
  return sub;
}
void Matrix::WriteBack(osl_relation_p ptr) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++)
      OslIntWrite(ptr->m[i][j], data[i][j], ptr->precision);
  }
}

bool Matrix::operator==(const Matrix &other) const {
  if (row != other.row || col != other.col)
    return false;
  for (int i = 0; i < row; i++)
    for (int j = 0; j < col; j++)
      if (data[i][j] != other.data[i][j])
        return false;
  return true;
}