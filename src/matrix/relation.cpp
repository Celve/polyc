#include <matrix/relation.h>
#include <osl/int.h>

Relation::Relation(osl_relation_p ptr)
    : ei(ptr->nb_rows, 1), output(ptr->nb_rows, ptr->nb_output_dims),
      param(ptr->nb_rows, ptr->nb_parameters),
      input(ptr->nb_rows, ptr->nb_input_dims), one(ptr->nb_rows, 1), ptr(ptr),
      row_num(ptr->nb_rows) {
  for (int i = 0; i < ptr->nb_rows; i++) {
    ei.SetData(i, 0, OslIntRead(ptr->m[i][0], ptr->precision));
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_output_dims; j++) {
      output.SetData(i, j, OslIntRead(ptr->m[i][j + 1], ptr->precision));
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_parameters; j++) {
      param.SetData(
          i, j,
          OslIntRead(ptr->m[i][j + 1 + ptr->nb_output_dims], ptr->precision));
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_input_dims; j++) {
      input.SetData(
          i, j,
          OslIntRead(
              ptr->m[i][j + 1 + ptr->nb_output_dims + ptr->nb_parameters],
              ptr->precision));
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    one.SetData(i, 0,
                OslIntRead(ptr->m[i][ptr->nb_columns - 1], ptr->precision));
  }
}

void Relation::WriteBack() {
  // free the original
  // now, here is a memory leak because I don't know how to free it

  // create the new matrix
  auto col_num = ei.GetColNum() + output.GetColNum() + param.GetColNum() +
                 input.GetColNum() + one.GetColNum();
  osl_int_t **p;
  osl_int_t *q;
  OSL_malloc(p, osl_int_t **, (size_t)row_num * sizeof(osl_int_t *));
  OSL_malloc(q, osl_int_t *,
             (size_t)row_num * (size_t)col_num * sizeof(osl_int_t));
  ptr->m = p;
  for (int i = 0; i < row_num; i++) {
    ptr->m[i] = q + i * col_num;
    for (int j = 0; j < col_num; j++)
      osl_int_init_set_si(OSL_PRECISION_MP, &ptr->m[i][j], 0);
  }

  // write back configurations
  ptr->nb_rows = row_num;
  ptr->nb_columns = col_num;
  ptr->nb_output_dims = output.GetColNum();
  ptr->nb_parameters = param.GetColNum();
  ptr->nb_input_dims = input.GetColNum();

  for (int i = 0; i < ptr->nb_rows; i++) {
    OslIntWrite(ptr->m[i][0], ei.GetData(i, 0), ptr->precision);
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_output_dims; j++) {
      OslIntWrite(ptr->m[i][j + 1], output.GetData(i, j), ptr->precision);
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_parameters; j++) {
      OslIntWrite(ptr->m[i][j + 1 + ptr->nb_output_dims], param.GetData(i, j),
                  ptr->precision);
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    for (int j = 0; j < ptr->nb_input_dims; j++) {
      OslIntWrite(ptr->m[i][j + 1 + ptr->nb_output_dims + ptr->nb_parameters],
                  input.GetData(i, j), ptr->precision);
    }
  }

  for (int i = 0; i < ptr->nb_rows; i++) {
    OslIntWrite(ptr->m[i][1 + ptr->nb_output_dims + ptr->nb_parameters +
                          ptr->nb_input_dims],
                one.GetData(i, 0), ptr->precision);
  }
}

int Relation::GetRowNum() { return row_num; }

int Relation::GetColNum() {
  return ei.GetColNum() + output.GetColNum() + param.GetColNum() +
         input.GetColNum() + one.GetColNum();
}

int Relation::GetData(int row, int col) {
  if (col < ei.GetColNum()) {
    return ei.GetData(row, col);
  } else if (col < ei.GetColNum() + output.GetColNum()) {
    return output.GetData(row, col - ei.GetColNum());
  } else if (col < ei.GetColNum() + output.GetColNum() + param.GetColNum()) {
    return param.GetData(row, col - ei.GetColNum() - output.GetColNum());
  } else if (col < ei.GetColNum() + output.GetColNum() + param.GetColNum() +
                       input.GetColNum()) {
    return input.GetData(row, col - ei.GetColNum() - output.GetColNum() -
                                  param.GetColNum());
  } else {
    return one.GetData(row, 0);
  }
}

void Relation::SetData(int row, int col, int value) {
  if (col < ei.GetColNum()) {
    ei.SetData(row, col, value);
  } else if (col < ei.GetColNum() + output.GetColNum()) {
    output.SetData(row, col - ei.GetColNum(), value);
  } else if (col < ei.GetColNum() + output.GetColNum() + param.GetColNum()) {
    param.SetData(row, col - ei.GetColNum() - output.GetColNum(), value);
  } else if (col < ei.GetColNum() + output.GetColNum() + param.GetColNum() +
                       input.GetColNum()) {
    input.SetData(row,
                  col - ei.GetColNum() - output.GetColNum() - param.GetColNum(),
                  value);
  } else {
    one.SetData(row, 0, value);
  }
}

int Relation::GetRowLast(int row) { return one.GetRowLast(row); }

void Relation::SetRowLast(int row, int value) { one.SetRowLast(row, value); }

void Relation::InsertEmptyRowAt(int pos) {
  ei.InsertEmptyRowAt(pos);
  output.InsertEmptyRowAt(pos);
  param.InsertEmptyRowAt(pos);
  input.InsertEmptyRowAt(pos);
  one.InsertEmptyRowAt(pos);
  row_num++;
}

bool Relation::operator==(const std::vector<int> &stmt_id) const {
  // check the branches of the AST
  for (int i = 0; i < stmt_id.size() && i * 2 < one.GetRowNum(); i++) {
    if (one.GetRowLast(i * 2) != stmt_id[i]) {
      return false;
    }
  }

  // if stmt_ids.size() * 2 - 1 > GetRowNum(), there might be some errors

  return true;
}

bool Relation::operator<=(const std::vector<int> &stmt_id) const {
  // check the branches of the AST
  for (int i = 0; i < stmt_id.size() && i * 2 < one.GetRowNum(); i++) {
    if (one.GetRowLast(i * 2) > stmt_id[i]) {
      return false;
    }
  }

  return true;
}

bool Relation::operator<(const std::vector<int> &stmt_id) const {
  // corner cases, which corresponds to impossible
  if (stmt_id.empty()) {
    return false;
  }

  // check the branches of the AST
  for (int i = 0; i < stmt_id.size() && i * 2 < one.GetRowNum(); i++) {
    if (one.GetRowLast(i * 2) != stmt_id[i]) {
      return one.GetRowLast(i * 2) < stmt_id[i];
    }
  }

  return false;
}