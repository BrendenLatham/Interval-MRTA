#ifndef VERTEX
#define VERTEX

class vertex{
    double est_x,est_y;
    double real_x;
    double real_y;
public:
    vertex(double real_x, double real_y, double est_x, double est_y);
    double get_x();
    double get_y();
    double get_real_x();
    double get_real_y();
};

#endif
