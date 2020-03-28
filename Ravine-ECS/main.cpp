#include <chrono>
#include <iostream>
#include "ComponentsManager.hpp"
#include "MovementSystem.h"
#include "GravitySystem.h"
#include "BoundingSystem.hpp"

int main(int argc, char** argv)
{
	ISystem* gravitySystem = new GravitySystem();
	ISystem* movementSystem = new MovementSystem();
	ISystem* boundingSystem = new BoundingSystem();
	cout.setf(std::ios::fixed, std::ios::floatfield);
	cout.precision(9);

	ComponentsManager::createComponents<PositionComponent>({});
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents(PositionComponent{ 1,1,1 }, VelocityComponent{ 0,0,0 });
	ComponentsManager::createComponents<VelocityComponent, PositionComponent>({ 0,0,0 }, { 2,2,2 });
	ComponentsManager::createComponents(PositionComponent{ 3,3,3 }, VelocityComponent{ 0,0,0 });
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents<PositionComponent>({});

	while (true)
	{
		//system("cls");

		//Start Delta-time
		static double dt = 0.03;
		static auto mStartTime = std::chrono::system_clock::now();

		//Update Systems
		movementSystem->update(dt);
		gravitySystem->update(dt);
		boundingSystem->update(dt);

		//Update Delta-time
		static auto mEndTime = std::chrono::system_clock::now();
		dt = std::chrono::duration_cast<std::chrono::nanoseconds>(mEndTime - mStartTime).count() * 1e-9;
		cout << "Delta-time:" << dt << endl;
	}
	system("pause");
	return 0;
}