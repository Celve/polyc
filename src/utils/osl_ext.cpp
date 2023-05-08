#include <string>
#include <utils/osl_ext.h>

osl_generic_p FindExt(osl_scop_p scop, std::string name) {
  auto extension = scop->extension;
  while (extension->interface->URI != name) {
    extension = extension->next;
  }
  return extension;
}