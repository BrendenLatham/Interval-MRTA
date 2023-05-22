#include "vertex.h"
#include <random>
#include <sys/types.h>
#include <unistd.h>



vertex::vertex(double x, double y, double real_x, double real_y){
    this->x = x;
    this->y = y;
    this->real_x = real_x;
    this->real_y = real_y;
}
double vertex::get_x(){
    return(x);
}
double vertex::get_y(){
    return(y);
}
double vertex::get_real_x(){
    return(real_x);
}
double vertex::get_real_y(){
    return(real_y);
}
