#include <chrono>

#include "components/Comflabulation.h"
#include "systems/ComflabulationSystem.hpp"
#include "systems/EntityTestSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/GravitySystem.hpp"
#include "systems/BoundarySystem.hpp"

using namespace rv;

// Tests Forward declaration
void entityTest();
void performanceTest();

int main(int argc, char** argv)
{
    // performanceTest();
    entityTest();

    return 0;
}

void entityTest()
{
    ISystem* gravitySystem = new GravitySystem();
    ISystem* boundarySystem = new BoundarySystem();
    ISystem* movementSystem = new MovementSystem();
    ISystem* entityTestSystem = new EntityTestSystem();

    EntitiesManager::createEntity<Position>({3, 4});
    EntitiesManager::createEntity<Position, Velocity>();
    EntitiesManager::createEntity(Position(1, 0), Velocity(0, 5.0));
    EntitiesManager::createEntity(Position(2, 0), Velocity(0, 0.5), Comflabulation());
    EntitiesManager::createEntity(Position(3, 10), Velocity(0, 0.25));

    fprintf(stdout, "Press RETURN to STOP, any other key to tick.\n");
    do
    {
        system("cls");
        movementSystem->update(0.016);
        gravitySystem->update(0.16);
        boundarySystem->update(0.016);
        entityTestSystem->update(0.016);
        fprintf(stdout, "\n");
    } while (_getwch() != '\r');
}

void performanceTest()
{
    size_t entityCount = 1'000'000;
    for (int32_t i = 0; i < entityCount; i++)
    {
        if (i > entityCount / 2)
        {
            EntitiesManager::createEntity<Velocity, Position, Comflabulation>();
        }
        else
        {
            EntitiesManager::createEntity(Velocity(0, i / 3.0f), Position((float)i, 0));
        }

        if (i % 1000 == 0)
        {
            float prog = (i / (double)entityCount) * 100.0;
            fprintf(stdout, "Entities Allocation (%f%%)\n", prog);
        }
    }

    ISystem* comflabuSystem = new ComflabulationSystem();
    ISystem* movementSystem = new MovementSystem();
    const size_t testCount = 1'000;
    double acc = 0;
    double times[testCount];
    for (size_t i = 0; i < testCount; i++)
    {
        auto start = std::chrono::system_clock::now();

        comflabuSystem->update(0.016);
        movementSystem->update(0.016);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double deltaTime = elapsed.count() / 1'000'000.0;
        fprintf(stdout, "It time %fms\n", deltaTime);
        times[i] = deltaTime;
        acc += deltaTime;
    }
    double mean = acc / (double)testCount;
    double var = 0;
    for (size_t i = 0; i < testCount; i++)
    {
        var += (times[i] - mean) * (times[i] - mean);
    }
    var /= (double)testCount;
    double stddev = sqrt(var);
    fprintf(stdout, "\n\n");
    fprintf(stdout, "===================================================\n");
    fprintf(stdout, "Test took %fms (mean) %f+/-ms (std-dev)\n", mean, stddev);
    fprintf(stdout, "===================================================\n");
    
    getchar();
}
