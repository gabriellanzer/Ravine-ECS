#include "BaseSystem.hpp"
#include "ComflabulationComponent.hpp"
#include "ComponentsIterator.hpp"

using namespace rv;
class ComflabulationSystem : public BaseSystem<ComflabulationComponent>
{
  public:
    void update(double deltaTime, size_t compSize, ComponentsIterator<ComflabulationComponent> comfabIt)
    {
        for (size_t i = 0; i < compSize; i++)
        {
            ComflabulationComponent& comflab = comfabIt[i];
            comflab.thingy *= 1.000001f;
            comflab.mingy = !comflab.mingy;
            comflab.dingy++;
        }
    }
};