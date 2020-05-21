#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H
#include <iostream>

#include "BaseSystem.hpp"
#include "PositionComponent.h"
#include "VelocityComponent.h"
#include "taskflow/taskflow.hpp"

using namespace rv;
using std::cout;
using std::endl;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
  private:
    tf::Taskflow* taskFlow;
    double deltaTime;
    CompIt<VelocityComponent> velIt;
    CompIt<PositionComponent> posIt;

  public:
    MovementSystem(tf::Taskflow* flow) : taskFlow(flow){};

    void update(double deltaTime, size_t size, CompIt<VelocityComponent> vel, CompIt<PositionComponent> pos) override;

    void step(int i);
};

#endif