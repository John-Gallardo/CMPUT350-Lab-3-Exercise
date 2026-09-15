#include <cstdlib>
#include <cassert>
#include <iostream>
#include "SharedPtr.h"

struct Foo{
    public:
        void print() {
            std::cout << "Foo\n";
        }
};

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

    // 3. Move semantics test

    SharedPtr<int> sharedPtr3{std::move(uninitialized)};
    assert(sharedPtr3.useCount() == 3);

    SharedPtr<int> moveAssignment{};
    moveAssignment = std::move(sharedPtr3);
    assert(sharedPtr3 == false);
    assert(moveAssignment.useCount() == 3);
    
    // 4. dereferences operators + .get() test
    SharedPtr<int> sharedPtr4{new int(20)};
    assert(*sharedPtr4 == 20);
    SharedPtr<Foo> fooTest{new Foo()};
    fooTest->print();  // terminal should print Foo
    assert(sharedPtr4.get() != nullptr);

    // 5. equality & boolean coversion test
    SharedPtr<int> sharedPtr5{sharedPtr4};
    SharedPtr<int> sharedPtr6{new int(30)};
    assert(sharedPtr5 == sharedPtr4);
    assert(sharedPtr6 != sharedPtr5);
    assert(sharedPtr5 == true);

    SharedPtr<int> uninitialized2{};
    assert(uninitialized2 == false);

    // 6. swap and reset test
    uninitialized2.swap(sharedPtr6);
    assert(uninitialized2 == true);
    assert(sharedPtr6 == false);

    uninitialized2.reset();
    assert(uninitialized2 == false);

    sharedPtr6.reset(new int(50));
    assert(*sharedPtr6 == 50);
    assert(sharedPtr6.useCount() == 1);

    // 7. use count test
    {
        SharedPtr<int> sharedPtr7{sharedPtr6};
        assert(sharedPtr7.useCount() == 2);
    }
    assert(sharedPtr6.useCount() == 1);

    // 8. makeShared test
    SharedPtr<int> makeSharedTest{makeSharedBasic<int>(100)};
    assert(*makeSharedTest == 100);
    auto makeSharedTest2{makeSharedBasic<int>(1000)};
    assert(*makeSharedTest2 == 1000);

    return EXIT_SUCCESS;
}
