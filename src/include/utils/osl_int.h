#pragma once

#include <cassert>
#include <gmp.h>
#include <osl/int.h>
#include <osl/macros.h>

int OslIntRead(const osl_int_t &value, const int &precision);

void OslIntWrite(osl_int_t &value, const int &data, const int &precision);