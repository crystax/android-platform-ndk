
#include "lib-static.hpp"

namespace libStatic {

int&
getValue()
{
  static int value = 1;
  return value;
}

} // namespace libStatic
