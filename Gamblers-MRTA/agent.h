#ifndef AGENT
#define AGENT
#include "vertex.h"

class agent: public vertex{
    bool allocated;
public:
    using vertex::vertex;
    bool is_allocated();
    void set_allocated();
    void set_unallocated();
};

#endif // AGENT
