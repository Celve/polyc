/* poc.cpp A complete C to polyhedra to C compiler */

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

// int split(osl_scop_p scop, std::vector<int> statementID, unsigned int depth);
// int reorder(osl_scop_p scop, std::vector<int> statementID, std::vector<int>
// neworder) ; int interchange(osl_scop_p scop,
//                 std::vector<int> statementID,
//                 unsigned int depth_1, unsigned int depth_2,
//                 int pretty) ;

/**
 * fuse function:
 * Fuse loop with the first loop after
 * scop: the SCoP to be transformed
 * statementID: the statement scattering ID on AST
 * return status
 */
int fuse(osl_scop_p scop, std::vector<int> statementID) {
  // find the statement
  auto stmt_id = statementID[0];
  auto statement = scop->statement;
  while (statement != nullptr &&
         Matrix(statement->scattering).GetRowLast(0) <= stmt_id) {
    statement = statement->next;
  }
  if (statement != nullptr && statement->domain->nb_rows > 1) {
    while (statement != nullptr) {
      auto scattering = statement->scattering;
      auto row = scattering->nb_rows;
      auto column = scattering->nb_columns;
      Matrix mat = Matrix(scattering);
      if (mat.GetRowLast(0) != stmt_id) {
        mat.SetRowLast(0, mat.GetData(0, column - 1) - 1);
      }
      mat.WriteBack(scattering);

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

std::vector<std::string> split(const std::string &str, char end) {
  std::vector<std::string> result;
  int curr = 1;
  int next;
  while ((next = str.find(',', curr)) != std::string::npos) {
    result.push_back(trim(str.substr(curr, next - curr)));
    curr = next + 1;
  }
  result.push_back(trim(str.substr(curr, str.find(end) - curr)));
  return result;
}

/// Script is organized during processing.
void parse(std::string &script, std::string &op,
           std::vector<std::string> &args) {
  script.erase(0, script.find_first_not_of(' '));
  int curr = script.find('(');
  op = script.substr(0, curr);
  args = split(script.substr(curr, script.length() - curr - 2), ')');
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
  for (int i = 0; i < 3; i++) {
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
    std::vector<std::string> fuse_strs = split(args[0], ']');
    std::vector<int> fuse_args;
    for (auto fuse_str : fuse_strs) {
      fuse_args.push_back(std::stoi(fuse_str));
    }

    // debug
    for (auto arg : fuse_args) {
      std::cout << arg << std::endl;
    }

    fuse(scop, fuse_args);
  }

  print_scop_to_c(stdout, scop);
  osl_scop_free(scop);
  fclose(input);
  return 0;
}