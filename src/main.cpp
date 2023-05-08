/* poc.cpp A complete C to polyhedra to C compiler */

#include "utils/str.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <osl/extensions/scatnames.h>
#include <osl/strings.h>
#include <vector>

#include <clan/clan.h>
#include <cloog/cloog.h>
#include <cstdlib>
#include <gmp.h>
#include <osl/osl.h>

#include <matrix/matrix.h>
#include <matrix/relation.h>
#include <utils/osl_ext.h>
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
    auto mat = Relation(scattering);
    if (mat == statementID) {
      mat.SetRowLast(target, mat.GetRowLast(target) + 1);
      mat.WriteBack();
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
    auto mat = Relation(scattering);
    if (mat == statementID) {
      mat.SetRowLast(target, neworder[mat.GetRowLast(target)]);
      mat.WriteBack();
    } else {
      break;
    }

    statement = statement->next;
  }

  return 0;
}

/**
 * interchange function:
 * On each statement which belongs to the node, the loops that match the
 * depth_1-th and the depth_2 are interchanged
 * given the inner loop
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth_1, depth_2: >= 1
 * pretty: 1 or 0 : whether update the scatnames
 * return status
 */
int interchange(osl_scop_p scop, std::vector<int> statementID,
                unsigned int depth_1, unsigned int depth_2, int pretty) {
  // find the statement
  auto statement = NavigateToOslStmt(scop->statement, statementID);

  auto scattering = statement->scattering;
  auto mat = Relation(scattering);
  auto row_num = mat.GetRowNum();
  auto output = mat.GetOutput();
  output->SwapRows((depth_1 - 1) * 2 + 1, (depth_2 - 1) * 2 + 1);
  mat.WriteBack();
  return 0;
}

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
      auto mat = Relation(scattering);
      if (mat == statementID) {
        mat.SetRowLast(target, mat.GetRowLast(target) - 1);
        mat.WriteBack();
      } else {
        break;
      }

      statement = statement->next;
    }
  }

  return 0;
}

/**
 * skew function
 * Transform the iteration domain so that the loop at depth depends on the
 * loop iterator at depth_other: in all occurrences, the loop iterator i
 * of the former loop is replaced by (i + coeff*j) where j is the loop iterator
 * of the latter loop.  Adjusts the loop boundaries accordingly.
 * Skewing the loop by its own iterator, i.e. depth == depth_outer, is invalid
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth: 1-based depth of the output loop to modify
 * depth_other: 1-based depth of the loop iterator to add
 * coeff: the coefficient to multiply the dimension by
 * return status
 */
int skew(osl_scop_p scop, std::vector<int> statementID, unsigned int depth,
         unsigned int depth_other, int coeff) {
  ;
}

/**
 * tile function:
 * Do tiling on the loop at depth with size, the outer loop is at depth_outer
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * depth: tiling on loop at depth
 * depth_outer: outer loop depth
 * size: tiling size
 * return status
 */
int tile(osl_scop_p scop, std::vector<int> statementID, unsigned int depth,
         unsigned int depth_outer, unsigned int size) {
  // find the statement
  auto statement = NavigateToOslStmt(scop->statement, statementID);
  auto matrix = Relation(statement->scattering);

  // deal with appended columns
  auto output = matrix.GetOutput();
  output->SwapCols(depth_outer * 2 - 1, depth_outer * 2);
  output->InsertEmptyColAt(depth_outer * 2 - 1);
  output->InsertEmptyColAt(depth_outer * 2 + 2);

  // deal with appended rows
  matrix.InsertEmptyRowAt(depth_outer * 2 + 1);
  matrix.InsertEmptyRowAt(depth_outer * 2 + 2);
  matrix.InsertEmptyRowAt(depth_outer * 2 + 3);

  // make it inequality
  auto ei = matrix.GetEi();
  ei->SetRowLast(depth_outer * 2 + 1, 1);
  ei->SetRowLast(depth_outer * 2 + 2, 1);

  // set its relation with loop in depth
  output->SetData(depth_outer * 2 + 1, depth_outer * 2 - 1, -size);
  output->SetData(depth_outer * 2 + 2, depth_outer * 2 - 1, size);
  output->SetData(depth_outer * 2 + 1, depth * 2 + 1, 1);
  output->SetData(depth_outer * 2 + 2, depth * 2 + 1, -1);
  output->SetData(depth_outer * 2 + 3, depth_outer * 2 + 2, -1);

  auto one = matrix.GetOne();
  one->SetRowLast(depth_outer * 2 + 2, size - 1);

  // write back
  matrix.WriteBack();

  // one more thing about scatname
  auto ext = FindExt(scop, "scatnames");
  auto scatnames = reinterpret_cast<osl_scatnames *>(ext->data);
  auto names = osl::osl_strings_to_cpp(scatnames->names);
  osl_strings_free(scatnames->names);
  auto symbol = names[depth * 2 - 1];
  names.insert(names.begin() + depth_outer * 2 - 1, "b_outer");
  names.insert(names.begin() + depth_outer * 2 - 1, symbol + "_outer");
  scatnames->names = ToOslStrings(names);

  return 0;
}

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
  auto extension = FindExt(scop, "clay");

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
  } else if (op == "interchange") {
    auto statement_id =
        ToVectorInt(split(args[0].substr(1, args[0].size() - 2)));
    auto depth1 = std::stoi(args[1]);
    auto depth2 = std::stoi(args[2]);
    auto pretty = std::stoi(args[3]);

    interchange(scop, statement_id, depth1, depth2, pretty);
  } else if (op == "tile") {
    auto statement_id =
        ToVectorInt(split(args[0].substr(1, args[0].size() - 2)));
    auto depth = std::stoi(args[1]);
    auto depth_outer = std::stoi(args[2]);
    auto size = std::stoi(args[3]);

    tile(scop, statement_id, depth, depth_outer, size);
  }

  print_scop_to_c(stdout, scop);
  osl_scop_free(scop);
  fclose(input);
  return 0;
}