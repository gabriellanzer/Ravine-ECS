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
    ComponentsManager::createComponents(PositionComponent(0, 0, 0), VelocityComponent(0, 5, 0));

    while(1)
    {
        double deltaTime = 0.3;
        gravitySystem->update(deltaTime);
        boundingSystem->update(deltaTime);
        movementSystem->update(deltaTime);
    }
    
    system("pause");
    return 0;
}