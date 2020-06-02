#ifndef ISYSTEM_H
#define ISYSTEM_H

class ISystem
{
  public:
    virtual ~ISystem() = default;
    virtual void update(double deltaTime) = 0;
};

#endif