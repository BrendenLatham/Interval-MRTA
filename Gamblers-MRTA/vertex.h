#ifndef VERTEX
#define VERTEX

class vertex{
    double x,y;
    double real_x;
    double real_y;
public:
    vertex(double x, double y, double real_x, double real_y);
    double get_x();
    double get_y();
    double get_real_x();
    double get_real_y();
};

#endif
