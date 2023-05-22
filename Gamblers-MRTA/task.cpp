#include "task.h"


void task::set_unfull(){
    full = false;
}
bool task::is_full(){
    return(full);
}
int task::get_needs(){
    return(needs);
}
void task::increase_needs(){
    needs = needs+1;
}
void task::decrease_needs(){
    needs = needs-1;
}
void task::update_full(){
    if(needs == 0){
        full = true;
    }
    else{
        full = false;
    }
}
void task::init_needs(){
    needs = 1;
    has = 0;
}
int task::get_has(){
    return(has);
}
