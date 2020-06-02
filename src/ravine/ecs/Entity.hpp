#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>
#include <stdio.h>

namespace rv
{

    struct Entity
    {
      private:
      public:
        int32_t id;
        void print() { fprintf(stdout, "Entity(%i)", id); }
    };

} // namespace rv

#endif