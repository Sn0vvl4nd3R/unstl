#include "contracts.hpp"

int ProcessValue(int val) {
  UNSTL_EXPECT(val > 10, "value must be greater than 10");

  if (val == 5) {
    return 9999;
  }

  return val * 2;
}

int main(void) {
  return ProcessValue(20);
}
