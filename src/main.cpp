#include <chrono>
#include <stack>

#include "Systems/ComflabulationSystem.hpp"
#include "Systems/EntityTestSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/GravitySystem.hpp"
#include "Systems/BoundarySystem.hpp"

using std::stack;
using namespace rv;

// Tests Forward declaration
void entitiesTest();
void performanceTest();

int main(int argc, char** argv)
{
	entitiesTest();
	//performanceTest();

    return 0;
}

const char* options = R"(
Options:
-> RETURN to STOP
-> RIGHT to create new entity (Velocity=%f)
-> LEFT to remove last entity
-> Any other key to tick
)";

void entitiesTest()
{
	ISystem* movementSystem = new MovementSystem();
	ISystem* gravitySystem = new GravitySystem();
	ISystem* boundarySystem = new BoundarySystem();
	ISystem* entityTestSystem = new EntityTestSystem();

	while (true)
	{
		system("cls");

		entityTestSystem->update(0.016);
		gravitySystem->update(0.016);
		movementSystem->update(0.016);
		boundarySystem->update(0.016);

		static float velocity = 1.0f;
		fprintf(stdout, "\n");
		fprintf(stdout, options, velocity);
		char c = _getwch();
		if(c == '\r') break;
		if(c == 72) // Up Arrow
		{
			velocity += 1.0f;
		}
		if(c == 80) // Down Arrow
		{
			velocity -= 1.0f;
		}
		if(c == 77) // Right Arrow
		{
			Entity entity = EntitiesManager::createEntity<Position, Velocity>({0, 1}, {0, velocity});
			EntitiesManager::lateRemoveEntity(entity);
		}
		if(c == 75) // Left Arrow
		{
			EntitiesManager::flushEntityOperations();
		}
	}
}

void performanceTest()
{
	size_t entityCount = 10'000'000;
	for (int32_t i = 0; i < entityCount; i++)
	{
		if (i < entityCount/2)
		{
			EntitiesManager::createEntity<Velocity, Position>();
		}
		else
		{
			EntitiesManager::createEntity<Velocity, Position, Comflabulation>();
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

		movementSystem->update(0.016);
		comflabuSystem->update(0.016);

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
