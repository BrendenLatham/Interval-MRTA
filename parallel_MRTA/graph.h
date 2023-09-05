#ifndef GRAPH
#define GRAPH
#include "agent.h"
#include "task.h"
#include <vector>

class graph{
    int agent_count;
    int task_count;
    int free_agents;
    double env_size;
    double Matching_Value;
    bool fully_matched;
    std::vector<std::vector<double>> Weights;
    std::vector<std::vector<int>> Matching;
    void decrease_free_agents();
    void increase_matching_value(double n);

public:
    std::vector<agent> Agents;
    std::vector<task> Tasks;
    std::vector<std::pair<int,int>> dominating;
    void initialize_system(int agent_count, int env_size);
    void initialize_partial_system(int task_count, int env_size);
    void print_graph();
    int get_agent_count();
    int get_task_count();
    double get_matching_value();
    void set_matched(int t, int a);
    void hide_edge(int t, int a, int type);
    void hide_agent(int a);
    void hide_task(int t);
    bool is_fully_matched();
    void update_fully_matched();
    int get_agent_assignment(int a);


    int tasks_best_agent(int t);
    int agents_best_task(int a);

    int get_edge_type(int t, int a);
    double get_edge_weight(int t, int a);

    void update_weight(int t, int a, double weight);

    double reveal_real_weight(int t, int a);

    void localize(int a);

    void find_max_cost();


};




#endif
