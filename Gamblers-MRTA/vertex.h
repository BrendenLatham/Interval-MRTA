#ifndef VERTEX
#define VERTEX

class vertex{
    double x,y;
    double actual_x;
    double actual_y;
public:
    vertex(double x, double y);
    double get_x();
    double get_y();
    double get_actual_x();
    double get_actual_y();
};

#endif
