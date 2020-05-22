#include <chrono>
#include <iostream>

#include "taskflow/core/taskflow.hpp"
#include "taskflow/taskflow.hpp"

#include "MovementSystem.h"

using std::cout;
using std::endl;

using namespace rv;
using namespace tf;

int main(int argc, char** argv)
{
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    size_t entityCount = 2'000'000;
    for (size_t i = 0; i < entityCount; i++)
    {
        if(i < entityCount/2)
        {
            ComponentsManager::createComponents(PositionComponent(), VelocityComponent());
        }
        else    
        {
            ComponentsManager::createComponents(PositionComponent(), VelocityComponent(), OtherComponent());
        }
        
        if (i % 1000 == 0)
        {
            float prog = (i / (double)entityCount) * 100.0;
            fprintf(stdout, "Entities Allocation (%f%%)\n", prog);
        }
    }

    ISystem* movementSystem = new MovementSystem();
    const size_t testCount = 1'000;
	double acc;
	double times[testCount];
    for (size_t i = 0; i < testCount; i++)
    {
        auto start = std::chrono::system_clock::now();

        movementSystem->update(0.016);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double deltaTime = elapsed.count() / 1'000'000.0;
	    fprintf(stdout, "It time %fms\n", deltaTime);
        acc += deltaTime;
    }
	fprintf(stdout, "\n\n");
	fprintf(stdout, "==============\n");
	fprintf(stdout, "Test took %fms\n", acc / (double)testCount);
	fprintf(stdout, "==============\n");

    getchar();
    return 0;
}