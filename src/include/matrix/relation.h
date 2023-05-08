#include "matrix/matrix.h"
#include <osl/relation.h>
#include <vector>

class Relation {
public:
  Relation(osl_relation_p ptr);

  void WriteBack();

  int GetRowNum();
  int GetColNum();
  int GetData(int row, int col);
  void SetData(int row, int col, int value);
  int GetRowLast(int row);
  void SetRowLast(int row, int value);

  void InsertEmptyRowAt(int pos);

  Matrix *GetEi() { return &ei; }
  Matrix *GetOutput() { return &output; }
  Matrix *GetParam() { return &param; }
  Matrix *GetInput() { return &input; }
  Matrix *GetOne() { return &one; }

  bool operator==(const std::vector<int> &stmt_id) const;
  bool operator<=(const std::vector<int> &stmt_id) const;
  bool operator<(const std::vector<int> &stmt_id) const;

private:
  Matrix ei;
  Matrix output;
  Matrix param;
  Matrix input;
  Matrix one;
  osl_relation_p ptr;
  int row_num;
};
