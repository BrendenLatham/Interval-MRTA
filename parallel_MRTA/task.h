#ifndef TASK
#define TASK
#include "vertex.h"

class task: public vertex{
    bool full;
    int needs;
    int has;
public:
    using vertex::vertex;
    void set_unfull();
    bool is_full();
    int get_needs();
    void increase_needs();
    void decrease_needs();
    void update_full();
    void init_needs();
    int get_has();
};

#endif //TASK
