#include "agent.h"

bool agent::is_allocated(){
    return(allocated);
}
void agent::set_allocated(){
    allocated = true;
}
void agent::set_unallocated(){
    allocated = false;
}
