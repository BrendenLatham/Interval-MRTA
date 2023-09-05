#include "vertex.h"
#include <random>
#include <sys/types.h>
#include <unistd.h>



vertex::vertex(double real_x, double real_y, double est_x, double est_y){
    this->real_x = real_x;
    this->real_y = real_y;
    this->est_x = est_x;
    this->est_y = est_y;
}
double vertex::get_x(){
    return(est_x);
}
double vertex::get_y(){
    return(est_y);
}
double vertex::get_real_x(){
    return(real_x);
}
double vertex::get_real_y(){
    return(real_y);
}
