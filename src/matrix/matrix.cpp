#include <iostream>
#include <matrix/matrix.h>

Matrix::Matrix(int row, int col) {
  this->row_num = row;
  this->col_num = col;
  data.resize(row);
  for (int i = 0; i < row; i++) {
    data[i].resize(col);
  }
}

void Matrix::SwapRows(int row1, int row2) {
  for (int j = 0; j < GetColNum(); j++) {
    int tmp = GetData(row1, j);
    SetData(row1, j, GetData(row2, j));
    SetData(row2, j, tmp);
  }
}

void Matrix::SwapCols(int col1, int col2) {
  for (int i = 0; i < GetRowNum(); i++) {
    int tmp = GetData(i, col1);
    SetData(i, col1, GetData(i, col2));
    SetData(i, col2, tmp);
  }
}

void Matrix::InsertEmptyRowAt(int pos) {
  data.insert(data.begin() + pos, std::vector<int>(col_num, 0));
  row_num++;
}

void Matrix::InsertRowAt(std::vector<int> row, int pos) {
  data.insert(data.begin() + pos, row);
  row_num++;
}

void Matrix::InsertEmptyColAt(int pos) {
  for (int i = 0; i < row_num; i++) {
    data[i].insert(data[i].begin() + pos, 0);
  }
  col_num++;
}

void Matrix::InsertColAt(std::vector<int> col, int pos) {
  for (int i = 0; i < row_num; i++) {
    data[i].insert(data[i].begin() + pos, col[i]);
  }
  col_num++;
}

bool Matrix::operator==(const Matrix &other) const {
  if (row_num != other.row_num || col_num != other.col_num)
    return false;
  for (int i = 0; i < row_num; i++)
    for (int j = 0; j < col_num; j++)
      if (data[i][j] != other.data[i][j])
        return false;
  return true;
}