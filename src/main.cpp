#include <chrono>
#include <iostream>

// #include "BoundingSystem.hpp"
// #include "GravitySystem.h"
// #include "MovementSystem.h"

#include "ComponentStorage.hpp"
#include "ComponentGroup.hpp"

using std::cout;
using std::endl;

using namespace rv;

template<typename Type>
Type* GetUniquePtr()
{
    static Type* ptr = new Type;
    return ptr;
}

int main(int argc, char** argv)
{
    // ISystem *gravitySystem = new GravitySystem();
    // ISystem *movementSystem = new MovementSystem();
    // ISystem *boundingSystem = new BoundingSystem();
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(9);

    ComponentStorage<char> storage;
    intptr_t masks[] = {
        (intptr_t)GetUniquePtr<char>(),
        (intptr_t)GetUniquePtr<int>(),
        (intptr_t)GetUniquePtr<float>()
    };
    storage.getComponentGroup(masks, 2);
    for(auto maskGroupPair : storage.groups) { cout << maskGroupPair.first.typePtr << "|" << maskGroupPair.first.typesCount << endl; }
    cout << "END" << endl;
    intptr_t temp = masks[0];
    masks[0] = masks[1];
    masks[1] = temp;
    storage.getComponentGroup(masks, 2);
    for(auto maskGroupPair : storage.groups) { cout << maskGroupPair.first.typePtr << "|" << maskGroupPair.first.typesCount << endl; }
    cout << "END" << endl;
    masks[1] = (intptr_t)GetUniquePtr<double>();
    storage.getComponentGroup(masks, 1);
    for(auto maskGroupPair : storage.groups) { cout << maskGroupPair.first.typePtr << "|" << maskGroupPair.first.typesCount << endl; }
    cout << "END" << endl;
    masks[1] = (intptr_t)GetUniquePtr<short>();
    storage.getComponentGroup(masks, 3);
    for(auto maskGroupPair : storage.groups) { cout << maskGroupPair.first.typePtr << "|" << maskGroupPair.first.typesCount << endl; }
    cout << "END" << endl;

    masks[0] = 0;
    masks[1] = 1;
    masks[2] = 2;

    int32_t combCount;
    getMaskCombinations(masks, 3, combCount);

    // ComponentsManager::createComponents<PositionComponent>({});
    // ComponentsManager::createComponents<VelocityComponent>({});
    // ComponentsManager::createComponents<VelocityComponent>({});
    // ComponentsManager::createComponents(PositionComponent{1, 1, 1}, VelocityComponent{0, 0, 0});
    // ComponentsManager::createComponents<VelocityComponent, PositionComponent>({0, 0, 0}, {2, 2, 2});
    // ComponentsManager::createComponents(PositionComponent{3, 3, 3}, VelocityComponent{0, 0, 0});
    // ComponentsManager::createComponents<VelocityComponent>({});
    // ComponentsManager::createComponents<PositionComponent>({});

    // Creation Test
    ComponentGroup<char> testCyclic;
    cout << testCyclic.getDebugStr() << endl;

    // Initial Addition Test
    const char* str = "testBemGrande";
    testCyclic.addComponent(str, 13);
    cout << testCyclic.getDebugStr() << endl;

    // Base Operations Test
    testCyclic.rollClockwise(4);
    cout << testCyclic.getDebugStr() << endl;
    testCyclic.rollCounterClockwise(4);
    cout << testCyclic.getDebugStr() << endl;
    testCyclic.rollClockwise(4);
    cout << testCyclic.getDebugStr() << endl;
    testCyclic.shiftClockwise(2);
    cout << testCyclic.getDebugStr() << endl;

    // Addition Test
    testCyclic.addComponent("XZ", 2);
    cout << testCyclic.getDebugStr() << endl;

    // Removal Test
    int* compRem = new int[]{8,9};
    testCyclic.remComponent(compRem, 2);
    cout << testCyclic.getDebugStr() << endl;
    compRem[0] = 0; compRem[1] = 12;
    testCyclic.remComponent(compRem, 2);
    cout << testCyclic.getDebugStr() << endl;
    compRem[0] = 0; compRem[1] = 1;
    testCyclic.remComponent(compRem, 2);
    cout << testCyclic.getDebugStr() << endl;
    delete[] compRem;

    // while (true)
    // {
    //     // system("cls");
    //
    //     // Start Delta-time
    //     static double dt = 0.03;
    //     static auto mStartTime = std::chrono::system_clock::now();
    //
    //     // Update Systems
    //     movementSystem->update(dt);
    //     gravitySystem->update(dt);
    //     boundingSystem->update(dt);
    //
    //     // Update Delta-time
    //     static auto mEndTime = std::chrono::system_clock::now();
    //     dt = std::chrono::duration_cast<std::chrono::nanoseconds>(mEndTime - mStartTime).count() * 1e-9;
    //     cout << "Delta-time:" << dt << endl;
    // }

    system("pause");
    return 0;
}