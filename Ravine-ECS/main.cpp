#include <chrono>
#include <thread>
#include <iostream>
#include "ComponentsManager.hpp"
#include "MovementSystem.h"
#include "GravitySystem.h"
#include "BoundingSystem.hpp"

int main(int argc, char** argv)
{
	ISystem* gravitySystem = new GravitySystem();
	ISystem* testSystem = new MovementSystem();
	ISystem* boundingSystem = new BoundingSystem();
	cout.setf(std::ios::fixed, std::ios::floatfield);
	cout.precision(4);

	ComponentsManager::createComponents<PositionComponent>({});
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents(PositionComponent{ 2,2,2 }, VelocityComponent{ 0,1,0 });
	ComponentsManager::createComponents<VelocityComponent, PositionComponent>({ 1,2,0 }, { 0,0,3 });
	ComponentsManager::createComponents(PositionComponent{ 0,6,0 }, VelocityComponent{ 1,0,0 });
	ComponentsManager::createComponents<VelocityComponent>({});
	ComponentsManager::createComponents<PositionComponent>({});

	while (true)
	{
		system("cls");

		//Start Delta-time
		static double dt = 0.03;
		static auto mStartTime = std::chrono::system_clock::now();

		//Update Systems
		testSystem->update(dt);
		gravitySystem->update(dt);
		boundingSystem->update(dt);

		//Update Delta-time
		static auto mEndTime = std::chrono::system_clock::now();
		dt = std::chrono::duration_cast<std::chrono::microseconds>(mEndTime - mStartTime).count() * 1e-6;
	}
	system("pause");
	return 0;
}