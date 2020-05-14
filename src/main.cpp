#include <chrono>
#include <iostream>

#include "MovementSystem.h"
#include "GravitySystem.h"
#include "BoundingSystem.hpp"

using std::cout;
using std::endl;

using namespace rv;

int main(int argc, char** argv)
{
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    ISystem* movementSystem = new MovementSystem();
    ISystem* gravitySystem = new GravitySystem();
    ISystem* boundingSystem = new BoundingSystem();
    for (size_t i = 0; i < 20000000; i++)
    {
        ComponentsManager::createComponents(PositionComponent(i * 0.0001f, 0, 0), VelocityComponent(0, 1, 0));
    }

    while(1)
    {
        auto start = std::chrono::system_clock::now();
        
        double deltaTime = 0.3;
        gravitySystem->update(deltaTime);
        boundingSystem->update(deltaTime);
        movementSystem->update(deltaTime);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        std::cout << elapsed.count() * 0.000001 << "ms\n";
    }
    
    system("pause");
    return 0;
}