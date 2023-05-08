/* poc.cpp A complete C to polyhedra to C compiler */

#include "utils/str.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include <clan/clan.h>
#include <cloog/cloog.h>
#include <cstdlib>
#include <gmp.h>
#include <osl/osl.h>

#include <matrix/matrix.h>
#include <utils/osl_int.h>
#include <utils/osl_statement.h>

/**
 * split function:
 * Split the loop into two parts at the depth-th level from the statement
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth
 * return status
 */
int split(osl_scop_p scop, std::vector<int> statementID, unsigned int depth) {
  // find the statement
  auto statement = NavigateAfterOslStmt(scop->statement, statementID);
  statementID.resize(depth - 1);
  auto target = (depth - 1) * 2;

  while (statement != nullptr) {
    auto scattering = statement->scattering;
    auto mat = ScatteringMatrix(scattering);
    if (mat == statementID) {
      mat.SetRowLast(target, mat.GetRowLast(target) + 1);
      mat.WriteBack(scattering);
    } else {
      break;
    }

    statement = statement->next;
  }

  return 0;
}

/**
 * reorder function:
 * Reorders the statements in the loop
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * neworder: the new order of the statements
 * return status
 */
int reorder(osl_scop_p scop, std::vector<int> statementID,
            std::vector<int> neworder) {
  // find the statment
  auto statement = NavigateToOslStmt(scop->statement, statementID);
  auto target = statementID.size() * 2;

  while (statement != nullptr) {
    auto scattering = statement->scattering;
    auto mat = ScatteringMatrix(scattering);
    if (mat == statementID) {
      mat.SetRowLast(target, neworder[mat.GetRowLast(target)]);
      mat.WriteBack(scattering);
    } else {
      break;
    }

    statement = statement->next;
  }

  return 0;
}

// int interchange(osl_scop_p scop, std::vector<int> statementID,
//                 unsigned int depth_1, unsigned int depth_2, int pretty);

/**
 * fuse function:
 * Fuse loop with the first loop after
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * return status
 */
int fuse(osl_scop_p scop, std::vector<int> statementID) {
  // find the statement
  auto statement = NavigateAfterOslStmt(scop->statement, statementID);
  statementID.pop_back();
  auto target = statementID.size() * 2;

  // do only when the next instruction is a loop
  if (statement != nullptr && statement->domain->nb_rows > 1) {
    while (statement != nullptr) {
      auto scattering = statement->scattering;
      auto mat = ScatteringMatrix(scattering);
      if (mat == statementID) {
        mat.SetRowLast(target, mat.GetRowLast(target) - 1);
        mat.WriteBack(scattering);
      } else {
        break;
      }

      statement = statement->next;
    }
  }

  return 0;
}

// int skew(osl_scop_p scop,
//          std::vector<int> statementID,
//          unsigned int depth,
//          unsigned int depth_other,
//          int coeff) ;
// int tile(osl_scop_p scop,
//          std::vector<int> statementID, unsigned int depth, unsigned int
//          depth_outer, unsigned int size) ;
// int unroll(osl_scop_p scop, std::vector<int> statementID, unsigned int
// factor) ;

/* Use the Clan library to convert a SCoP from C to OpenScop */
osl_scop_p read_scop_from_c(FILE *input, char *input_name) {
  clan_options_p clanoptions;
  osl_scop_p scop;
  clanoptions = clan_options_malloc();
  clanoptions->precision = OSL_PRECISION_MP;
  CLAN_strdup(clanoptions->name, input_name);
  scop = clan_scop_extract(input, clanoptions);
  clan_options_free(clanoptions);
  return scop;
}

/* Use the CLooG library to output a SCoP from OpenScop to C */
void print_scop_to_c(FILE *output, osl_scop_p scop) {
  CloogState *state;
  CloogOptions *options;
  CloogInput *input;
  struct clast_stmt *clast;
  state = cloog_state_malloc();
  options = cloog_options_malloc(state);
  options->openscop = 1;
  cloog_options_copy_from_osl_scop(scop, options);
  input = cloog_input_from_osl_scop(options->state, scop);
  clast = cloog_clast_create_from_input(input, options);
  clast_pprint(output, clast, 0, options);
  cloog_clast_free(clast);
  options->scop = NULL; // don’t free the scop
  cloog_options_free(options);
  cloog_state_free(state); // the input is freed inside
}

// Remove leading zeros and trailing zeros
std::string trim(std::string str) {
  str.erase(0, str.find_first_not_of(' '));
  str.erase(str.find_last_not_of(' ') + 1);
  return str;
}

std::vector<std::string> split(const std::string &str) {
  std::vector<std::string> result;
  std::string curr;
  auto inside_brackets = 0;
  auto inside_parenthesis = 0;
  for (auto c : str) {
    if (c == '[') {
      inside_brackets++;
    } else if (c == '(') {
      inside_parenthesis++;
    } else if (c == ']') {
      inside_brackets--;
    } else if (c == ')') {
      inside_parenthesis--;
    }

    if (c == ',' && inside_parenthesis == 0 && inside_brackets == 0) {
      result.push_back(curr);
      curr.clear();
    } else if (c != ' ') {
      curr += c;
    }
  }

  if (!curr.empty()) {
    result.push_back(curr);
  }

  return result;
}

/// Script is organized during processing.
void parse(std::string &script, std::string &op,
           std::vector<std::string> &args) {
  script.erase(0, script.find_first_not_of(' '));
  int begin = script.find_first_of('(');
  int end = script.find_last_of(')');
  op = script.substr(0, begin);
  args = split(script.substr(begin + 1, end - begin - 1));
}

int main(int argc, char *argv[]) {
  osl_scop_p scop;
  FILE *input;
  if ((argc < 2) || (argc > 2)) {
    fprintf(stderr, "usage: %s file.c\n", argv[0]);
    exit(0);
  }
  if (argc == 1)
    input = stdin;
  else
    input = fopen(argv[1], "r");
  if (input == NULL) {
    fprintf(stderr, "cannot open input file\n");
    exit(0);
  }
  scop = read_scop_from_c(input, argv[1]);
  osl_scop_print(stdout, scop);

  // find the correct location
  auto extension = scop->extension;
  while (strcmp(extension->interface->URI, "clay")) {
    extension = extension->next;
  }

  // extract the script
  auto clay = reinterpret_cast<osl_clay *>(extension->data);
  auto script = std::string(clay->script);

  // parse the operation
  std::string op;
  std::vector<std::string> args;
  parse(script, op, args);

  // debug
  std::cout << op << std::endl;
  for (auto arg : args) {
    std::cout << arg << std::endl;
  }

  if (op == "fuse") {
    // need further splitting
    auto statement_id =
        ToVectorInt(split(args[0].substr(1, args[0].size() - 2)));

    fuse(scop, statement_id);
  } else if (op == "reorder") {
    auto statement_id =
        ToVectorInt(split(args[0].substr(1, args[0].size() - 2)));
    auto neworder = ToVectorInt(split(args[1].substr(1, args[1].size() - 2)));

    reorder(scop, statement_id, neworder);
  } else if (op == "split") {
    auto statement_id =
        ToVectorInt(split(args[0].substr(1, args[0].size() - 2)));
    auto depth = std::stoi(args[1]);

    split(scop, statement_id, depth);
  }

  print_scop_to_c(stdout, scop);
  osl_scop_free(scop);
  fclose(input);
  return 0;
}