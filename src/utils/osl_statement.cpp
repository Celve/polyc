#include <iostream>
#include <utils/osl_statement.h>

osl_statement_p NavigateOslStmt(osl_statement_p stmt,
                                std::vector<int> stmt_id) {
  while (stmt != nullptr && ScatteringMatrix(stmt->scattering) <= stmt_id) {
    stmt = stmt->next;
    std::cout << "next" << std::endl;
  }
  return stmt;
}