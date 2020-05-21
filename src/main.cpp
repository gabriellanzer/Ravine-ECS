#include <chrono>
#include <iostream>

#include "taskflow/core/taskflow.hpp"
#include "taskflow/taskflow.hpp"

#include "ComflabulationSystem.h"
#include "MovementSystem.h"

using std::cout;
using std::endl;

using namespace rv;
using namespace tf;

int main(int argc, char** argv)
{
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    Executor executor;
    Taskflow* taskflow = new Taskflow();

    //ISystem* comflabulationSystem = new ComflabulationSystem(taskflow);
    ISystem* movementSystem = new MovementSystem(taskflow);
    for (size_t i = 0; i < 10'000'000; i++)
    {
        // if (i >= 5'000'000)
        // {
        //     ComponentsManager::createComponents(PositionComponent(), VelocityComponent(), ComflabulationComponent());
        // }
        // else
        {
            ComponentsManager::createComponents(PositionComponent(), VelocityComponent());
        }
        if (i % 1000 == 0)
        {
            float prog = (i / 10'000'000.0) * 100.0;
            fprintf(stdout, "Entities Allocation (%f%%)\n", prog);
        }
    }

    double deltaTime = 0;
    int32_t i = 0;
    double acc = 0;

    movementSystem->update(deltaTime);
    //comflabulationSystem->update(deltaTime);

    while (1)
    {
        auto start = std::chrono::system_clock::now();

        executor.run(*taskflow).wait();

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        deltaTime = elapsed.count() / 1'000.0;

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