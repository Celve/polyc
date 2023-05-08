#pragma once

#include <matrix/matrix.h>
#include <osl/relation.h>
#include <osl/statement.h>
#include <vector>

osl_statement_p NavigateOslStmt(osl_statement_p curr,
                                std::vector<int> stmt_ids);