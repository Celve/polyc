#include <utils/osl_int.h>

int osl_int_read(const osl_int_t &value, const int &precision) {
  if (precision == OSL_PRECISION_MP) {
    mpz_t *mpz_value = reinterpret_cast<mpz_t *>(value.mp);
    return mpz_get_si(*mpz_value);
  } else {
    // todo
    assert(0);
  }
}

void osl_int_write(osl_int_t &value, const int &data, const int &precision) {
  if (precision == OSL_PRECISION_MP) {
    mpz_t *mpz_value = reinterpret_cast<mpz_t *>(value.mp);
    mpz_set_si(*mpz_value, data);
  } else {
    // todo
    assert(0);
  }
}