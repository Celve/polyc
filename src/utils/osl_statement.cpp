#include <iostream>
#include <matrix/relation.h>
#include <osl/statement.h>
#include <utils/osl_statement.h>

osl_statement_p NavigateAfterOslStmt(osl_statement_p stmt,
                                     std::vector<int> stmt_id) {
  while (stmt != nullptr && Relation(stmt->scattering) <= stmt_id) {
    stmt = stmt->next;
  }
  return stmt;
}

osl_statement_p NavigateToOslStmt(osl_statement_p stmt,
                                  std::vector<int> stmt_id) {
  while (stmt != nullptr && Relation(stmt->scattering) < stmt_id) {
    stmt = stmt->next;
  }
  return stmt;
}
