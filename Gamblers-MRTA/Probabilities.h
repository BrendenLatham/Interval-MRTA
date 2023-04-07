#ifndef PROBABILITIES
#define PROBABILITIES
#include <vector>

class probabilities{
int task_count;
int agent_count;
std::vector<std::vector<double>> error;
std::vector<std::vector<std::pair<double,int>>> Intervals;
public:
    void initialize(int t, int a);
    void store_error(int t, int a, double new_error);
    void print_error();
    void compute_tolerance(int t, int a, int type, double weight);
    void print_tolerance();
    double compute_probability(int t, int a, double weight);
};

#endif
