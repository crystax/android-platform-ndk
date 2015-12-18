
#include "lib-static.hpp"

namespace libShared {

int&
getValue()
{
  return libStatic::getValue();
}

} // namespace libShared
