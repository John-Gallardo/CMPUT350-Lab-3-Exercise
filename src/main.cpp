#include <cstdlib>
#include <cassert>
#include "SharedPtr.h"

int main() {
    // 1. Default constructor test
    SharedPtr<int> sharedPtr1;
    assert(sharedPtr1.useCount() == 0);
    SharedPtr<int> sharedPtr{new int(5)};
    assert(sharedPtr.useCount() == 1);

    return EXIT_SUCCESS;
}
