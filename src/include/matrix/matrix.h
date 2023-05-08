#pragma once

#include <osl/macros.h>
#include <osl/relation.h>
#include <utils/osl_int.h>
#include <vector>

class Phantom;

class Matrix {
public:
  /**
   * @brief Construct a new Matrix object from osl_relation_p
   *
   * @param ptr
   */
  Matrix(osl_relation_p ptr);

  /**
   * @brief Construct a empty Matrix object
   *
   * @param row
   * @param col
   */
  Matrix(int row, int col);

  /**
   * @brief Create a sub Phantom object in the Matrix
   *
   * @param row_start
   * @param row_end
   * @param col_start
   * @param col_end
   * @return Phantom
   */
  Phantom SubPhantom(int row_start, int row_end, int col_start, int col_end);

  void WriteBack(osl_relation_p ptr);

  int GetData(int i, int j) const { return data[i][j]; }

  void SetData(int i, int j, int value) { data[i][j] = value; }

  int GetRowLast(int i) const { return data[i][col_num - 1]; }

  void SetRowLast(int i, int value) { data[i][col_num - 1] = value; }

  int GetColLast(int j) const { return data[row_num - 1][j]; }

  void SetColLast(int j, int value) { data[row_num - 1][j] = value; }

  int GetRowNum() const { return row_num; }

  int GetColNum() const { return col_num; }

  bool operator==(const Matrix &other) const;

  friend class Phantom;

private:
  int row_num;
  int col_num;
  std::vector<std::vector<int>> data;
};

class ScatteringMatrix : public Matrix {
public:
  ScatteringMatrix(osl_relation_p ptr) : Matrix(ptr) {}
  ScatteringMatrix(int row, int col) : Matrix(row, col) {}

  bool operator<(std::vector<int> stmt_ids);
  bool operator<=(std::vector<int> stmt_ids);
  bool operator==(std::vector<int> stmt_ids);
};

class Phantom : public Matrix {
public:
  /**
   * @brief Construct a new Phantom object
   *
   * @param mat
   */
  Phantom(Matrix *mat, int row_start, int row_end, int col_start, int col_end);

  /**
   * @brief Swap two rows
   *
   * @param row1
   * @param row2
   */
  void SwapRows(int row1, int row2);

  /**
   * @brief Swap two columns
   *
   * @param col1
   * @param col2
   */
  void SwapCols(int col1, int col2);

  /**
   * @brief Write back the Phantom to the original Matrix
   *
   */
  void WriteBack();

private:
  int row_start;
  int col_start;
  Matrix *mat;
};
