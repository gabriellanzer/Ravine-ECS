#include <chrono>
#include <iostream>

#include "ComponentStorage.hpp"

using std::cout;
using std::endl;

using namespace rv;

int main(int argc, char** argv)
{
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    ComponentStorage<char> storage;
    intptr_t masks[] = {
        (intptr_t)0,
        (intptr_t)1,
        (intptr_t)2
    };
    const char* components = "TestComps";
    storage.addComponent(masks, 2, components, 4);
    storage.addComponent(masks, 3, components, 9);

    system("pause");
    return 0;
}