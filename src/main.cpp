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
    storage.addComponent(masks, 3, components, sizeof(components));

    system("pause");
    return 0;
}