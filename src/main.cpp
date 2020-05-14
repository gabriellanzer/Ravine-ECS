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
        (intptr_t)ComponentStorage<char>::getInstance(),
        (intptr_t)ComponentStorage<float>::getInstance(),
        (intptr_t)ComponentStorage<int>::getInstance()
        };
    const char* components = "TestComps";
    storage.addComponent(masks, 2, components, 4);
    storage.addComponent(masks, 3, components, 9);
    storage.addComponent(masks, 1, components, 3);
    intptr_t mask = masks[0];
    ComponentsIterator<char> test = storage.getComponentIterator(mask);
    
    system("pause");
    return 0;
}