#include <chrono>
#include <stack>

#include "Systems/BoundarySystem.hpp"
#include "Systems/ComflabulationSystem.hpp"
#include "Systems/EntityTestSystem.hpp"
#include "Systems/GravitySystem.hpp"
#include "Systems/MovementSystem.hpp"

using namespace rv;

// Goals:
// - High-performance low cache-miss storages for an Entity Component System implementation
// - Pay for what you use implementation of Entity Handles and Archetype Groups Registry
// - Serialize/Deserialize Entity Handles and Components Array raw data into the disk
// - Have a Rendering Abstraction API for a 2D Rendering Framework

// TODOs:
// Remove Entity Storage Implementation and create a class representing entity operations
// Then apply these operations to the Entity IDs directly (only for tracked Entities)
// Change Registry to Work with a Graph representation and only store existing Groups
// Ensure that Groups are storage agnostic and operate on template type pointers only
// Create compile-time checks for empty-structs entity flags, so they never have a storage

// Tests Forward declaration
void entitiesTest();
void performanceTest();

std::set<float> vectorSet;

int main(int argc, char** argv)
{
	//entitiesTest();
	performanceTest();
<<<<<<< Updated upstream
	vectorSet.clear();
=======
>>>>>>> Stashed changes

	return 0;
}

const char* options = R"(
Options:
-> RETURN to STOP
-> UP increase initial Velocity
-> Down decrease initial Velocity
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
		if (c == '\r')
			break;
		if (c == 72) // Up Arrow
		{
			velocity += 1.0f;
		}
		if (c == 80) // Down Arrow
		{
			velocity -= 1.0f;
		}
		if (c == 77) // Right Arrow
		{
			Entity entity = EntityRegistry::createEntity<Position, Velocity>({0, 1}, {0, velocity});
			EntityRegistry::removeEntity(entity);
		}
		if (c == 75) // Left Arrow
		{
			EntityRegistry::flushEntityOperations();
		}
	}
}

void performanceTest()
{
	size_t entityCount = 100'000;
	for (int32_t i = 0; i < entityCount; i++)
	{
		if (i < entityCount / 2)
		{
			EntityRegistry::createEntity<Velocity, Position>();
		}
		else
		{
			EntityRegistry::createEntity<Velocity, Position, Comflabulation>();
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
		if (i % 50 == 0)
		{
			fprintf(stdout, "It time %fms\n", deltaTime);
		}
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
