#include <chrono>
#include <iostream>

#include "ComflabulationSystem.hpp"
#include "MovementSystem.h"

using std::cout;
using std::endl;

using namespace rv;

int main(int argc, char** argv)
{
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    ISystem* comflabulationSystem = new ComflabulationSystem();
    ISystem* movementSystem = new MovementSystem();
    for (size_t i = 0; i < 10'000'000; i++)
    {
        if (i % 2 != 0)
        {
            ComponentsManager::createComponents(PositionComponent(), VelocityComponent(), ComflabulationComponent());
        }
        else
        {
            ComponentsManager::createComponents(PositionComponent(), VelocityComponent());
        }
        if(i%1000 == 0)
        {
            float prog = (i/10'000'000.0)*100.0;
            fprintf(stdout, "Entities Allocation (%f%%)\n", prog);
        }
    }

    double deltaTime = 0;
    int32_t i = 0;
    double acc = 0;
    while (1)
    {
        auto start = std::chrono::system_clock::now();

        movementSystem->update(deltaTime);
        comflabulationSystem->update(deltaTime);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        deltaTime = elapsed.count() * 0.000000001;

        acc += deltaTime;
        i++;
        if (i == 50)
        {
            fprintf(stdout, "%f seconds\n", acc / 50.0);
            acc = 0;
            i = 0;
        }
    }

    system("pause");
    return 0;
}