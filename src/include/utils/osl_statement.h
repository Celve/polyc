#pragma once

#include <matrix/matrix.h>
#include <osl/relation.h>
#include <osl/statement.h>
#include <vector>

osl_statement_p NavigateAfterOslStmt(osl_statement_p curr,
                                     std::vector<int> stmt_ids);

osl_statement_p NavigateToOslStmt(osl_statement_p stmt,
                                  std::vector<int> stmt_id);