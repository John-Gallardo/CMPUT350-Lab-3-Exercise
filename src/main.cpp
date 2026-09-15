#include <cstdlib>
#include <cassert>
#include <iostream>
#include "SharedPtr.h"

int main() {
    // 1. Default constructor test
    SharedPtr<int> uninitialized;
    assert(uninitialized.useCount() == 0);
    SharedPtr<int> sharedPtr{new int(5)};
    assert(sharedPtr.useCount() == 1);


    // 2. Copy semantics test
    SharedPtr<int> sharedPtr2{sharedPtr};
    assert(sharedPtr2.useCount() == 2);
    uninitialized = sharedPtr2;
    assert(uninitialized.useCount() == 3);

    return EXIT_SUCCESS;
}
