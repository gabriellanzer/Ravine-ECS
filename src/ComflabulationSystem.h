#ifndef COMFLABULATIONSYSTEM_H
#define COMFLABULATIONSYSTEM_H

#include "BaseSystem.hpp"
#include "ComflabulationComponent.h"
#include "taskflow/core/taskflow.hpp"

using namespace rv;

class ComflabulationSystem : public BaseSystem<ComflabulationComponent>
{
  private:
    tf::Taskflow* taskFlow;

  public:
    ComflabulationSystem(tf::Taskflow* flow) : taskFlow(flow){};

    void update(double deltaTime, size_t compSize, CompIt<ComflabulationComponent> comfabIt) override;
};

#endif