#include <cassert>
#include <gmp.h>
#include <osl/int.h>
#include <osl/macros.h>

int osl_int_read(const osl_int_t &value, const int &precision);

void osl_int_write(osl_int_t &value, const int &data, const int &precision);