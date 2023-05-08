#include <osl/macros.h>
#include <osl/relation.h>
#include <utils/osl_int.h>
#include <vector>

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
   * @brief Create a sub Matrix object
   *
   * @param row_start
   * @param row_end
   * @param col_start
   * @param col_end
   * @return Matrix
   */
  Matrix SubMatrix(int row_start, int row_end, int col_start, int col_end);

  void WriteBack(osl_relation_p ptr);

  int GetData(int i, int j) const { return data[i][j]; }

  void SetData(int i, int j, int value) { data[i][j] = value; }

  int GetRowLast(int i) const { return data[i][col - 1]; }

  void SetRowLast(int i, int value) { data[i][col - 1] = value; }

  int GetColLast(int j) const { return data[row - 1][j]; }

  void SetColLast(int j, int value) { data[row - 1][j] = value; }

  int GetRow() const { return row; }

  int GetCol() const { return col; }

  bool operator==(const Matrix &other) const;

  friend class Phantom;

private:
  int row;
  int col;
  std::vector<std::vector<int>> data;
};
