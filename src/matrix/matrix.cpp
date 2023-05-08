#include <iostream>
#include <matrix/matrix.h>

Matrix::Matrix(osl_relation_p ptr) {
  row_num = ptr->nb_rows;
  col_num = ptr->nb_columns;
  data.resize(row_num);
  for (int i = 0; i < row_num; i++) {
    data[i].resize(col_num);
    for (int j = 0; j < col_num; j++) {
      data[i][j] = OslIntRead(ptr->m[i][j], ptr->precision);
    }
  }
}

Matrix::Matrix(int row, int col) {
  this->row_num = row;
  this->col_num = col;
  data.resize(row);
  for (int i = 0; i < row; i++) {
    data[i].resize(col);
  }
}

Phantom Matrix::SubPhantom(int row_start, int row_end, int col_start,
                           int col_end) {
  return Phantom(this, row_start, row_end, col_start, col_end);
}
void Matrix::WriteBack(osl_relation_p ptr) {
  for (int i = 0; i < row_num; i++) {
    for (int j = 0; j < col_num; j++)
      OslIntWrite(ptr->m[i][j], data[i][j], ptr->precision);
  }
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

bool ScatteringMatrix::operator==(std::vector<int> stmt_ids) {
  // check the branches of the AST
  for (int i = 0; i < stmt_ids.size() && i * 2 < GetRowNum(); i++) {
    if (GetRowLast(i * 2) != stmt_ids[i]) {
      return false;
    }
  }

  // if stmt_ids.size() * 2 - 1 > GetRowNum(), there might be some errors

  return true;
}

bool ScatteringMatrix::operator<=(std::vector<int> stmt_ids) {
  // check the branches of the AST
  for (int i = 0; i < stmt_ids.size() && i * 2 < GetRowNum(); i++) {
    if (GetRowLast(i * 2) > stmt_ids[i]) {
      return false;
    }
  }

  return true;
}

bool ScatteringMatrix::operator<(std::vector<int> stmt_ids) {
  // corner cases, which corresponds to impossible
  if (stmt_ids.empty()) {
    return false;
  }

  // check the branches of the AST
  for (int i = 0; i < stmt_ids.size() && i * 2 < GetRowNum(); i++) {
    if (GetRowLast(i * 2) != stmt_ids[i]) {
      return GetRowLast(i * 2) < stmt_ids[i];
    }
  }

  return false;
}

Phantom::Phantom(Matrix *mat, int row_start, int row_end, int col_start,
                 int col_end)
    : Matrix(row_end - row_start, col_end - col_start), row_start(row_start),
      col_start(col_start), mat(mat) {
  for (int i = row_start; i < row_end; i++) {
    for (int j = col_start; j < col_end; j++) {
      SetData(i - row_start, j - col_start, mat->GetData(i, j));
    }
  }
}

void Phantom::SwapRows(int row1, int row2) {
  for (int j = 0; j < GetColNum(); j++) {
    int tmp = GetData(row1, j);
    SetData(row1, j, GetData(row2, j));
    SetData(row2, j, tmp);
  }
}

void Phantom::SwapCols(int col1, int col2) {
  for (int i = 0; i < GetRowNum(); i++) {
    int tmp = GetData(i, col1);
    SetData(i, col1, GetData(i, col2));
    SetData(i, col2, tmp);
  }
}

void Phantom::WriteBack() {
  for (int i = row_start; i < row_start + row_num; i++) {
    for (int j = col_start; j < col_start + col_num; j++) {
      mat->SetData(i, j, GetData(i - row_start, j - col_start));
    }
  }
}