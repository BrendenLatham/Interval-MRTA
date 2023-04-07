#include "vertex.h"
#include <random>
#include <sys/types.h>



vertex::vertex(double x, double y){
    this->x = x;
    this->y = y;
    pid_t pid = getpid();
    std::default_random_engine x_generator;
    std::default_random_engine y_generator;
    x_generator.seed(pid);
    y_generator.seed(pid+1);
    std::normal_distribution<double> x_distribution(x,10.0);
    std::normal_distribution<double> y_distribution(y,10.0);

    actual_x = x_distribution(x_generator);
    actual_y = y_distribution(y_generator);
}
double vertex::get_x(){
    return(x);
}
double vertex::get_y(){
    return(y);
}
double vertex::get_actual_x(){
    return(actual_x);
}
double vertex::get_actual_y(){
    return(actual_y);
}
