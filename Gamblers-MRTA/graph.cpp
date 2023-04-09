#include <iostream>
#include <random>
#include<sys/types.h>
#include <unistd.h>
#include <math.h>
#include "graph.h"


void graph::randomize(int agent_count, int task_count){
    this->agent_count = agent_count;
    this->task_count = task_count;
    free_agents = agent_count;
    Matching_Value = 0;

    // starting random location generator
    pid_t pid = getpid();
    srand(pid);
    std::default_random_engine generator;
    generator.seed(pid);
    std::normal_distribution<double> distribution(50.0,10.0);
    // random generator up and running

    // populate agents and tasks
    for(int i=0;i<task_count;i++){
        task New_Task(distribution(generator),distribution(generator));
        New_Task.set_unfull();
        New_Task.init_needs();
        Tasks.push_back(New_Task);
    }
    for(int i=0;i<agent_count-task_count;i++){
        Tasks.at(rand()%task_count).increase_needs();
    }
    for(int i=0;i<agent_count;i++){
        agent New_Agent(distribution(generator),distribution(generator));
        New_Agent.set_unallocated();
        Agents.push_back(New_Agent);
    // agents and tasks done populating

    }
    // initializing matrices
    std::vector<double> init_vec1;
    std::vector<int> init_vec2;
    for(int i=0;i<agent_count;i++){
        init_vec1.push_back(0);
        init_vec2.push_back(0);
    }
    for(int i=0;i<task_count;i++){
        Weights.push_back(init_vec1);
        Matching.push_back(init_vec2);
    }
    // matrices initialized

    // calculating weight matrix
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            double cost;
            double x = Tasks.at(i).get_x() - Agents.at(j).get_x();
            double y = Tasks.at(i).get_y() - Agents.at(j).get_y();
            x = std::pow(x,2);
            y = std::pow(y,2);
            cost = std::sqrt(x+y);
            Weights.at(i).at(j) = 100 - cost;
        }
    }
    // weight matrix calculated
}

void graph::print_graph(){
    std::cout<<"Agents: "<<agent_count<<std::endl;
    std::cout<<"Tasks: "<<task_count<<std::endl;
    for(int i=0;i<task_count;i++){
        std::cout<<"Task " << i << " needs " <<Tasks.at(i).get_needs()<<std::endl;
    }
    std::cout<<"Free Agents: "<<free_agents<<std::endl;
    std::cout<<"Matching Value: "<<Matching_Value<<std::endl;
    std::cout<<"Weights: "<<std::endl;
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            std::cout<<Weights.at(i).at(j)<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<"Matching: "<<std::endl;
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            std::cout<<Matching.at(i).at(j)<<" ";
        }
        std::cout<<std::endl;
    }
}

int graph::get_agent_count(){
    return(agent_count);
}
int graph::get_task_count(){
    return(task_count);
}
void graph::update_fully_matched(){
    if(free_agents == 0){
        fully_matched = true;
    }
    else{
        fully_matched = false;
    }
}
bool graph::is_fully_matched(){
    return(fully_matched);
}
void graph::decrease_free_agents(){
    free_agents = free_agents-1;
}
double graph::get_matching_value(){
    return(Matching_Value);
}
void graph::increase_matching_value(double n){
    Matching_Value = Matching_Value+n;                               //value also here
}
void graph::set_matched(int t, int a){
    increase_matching_value(Weights.at(t).at(a));
    hide_agent(a);
    Matching.at(t).at(a) = 1;
    decrease_free_agents();
    Agents.at(a).set_allocated();
    Tasks.at(t).decrease_needs();
    Tasks.at(t).update_full();
    if(Tasks.at(t).is_full() == true){
        hide_task(t);
    }
    update_fully_matched();
}
void graph::hide_edge(int t, int a, int type){    //type 1 is matched, type 0 in unmatched
    if(type == 1){
        Weights.at(t).at(a) = 0;
    }
    else{
        if(type == 0){
            set_matched(t, a);
        }
    }
}
void graph::hide_task(int t){
    for(int i=0;i<agent_count;i++){
        Weights.at(t).at(i) = -1;
    }
}
void graph::hide_agent(int a){
    for(int i=0;i<task_count;i++){
        Weights.at(i).at(a) = -1;
    }
}

int graph::tasks_best_agent(int t){
    int best_agent;
    double best_value = -1;
    double current_value;
    for(int i=0;i<agent_count;i++){
        current_value = Weights.at(t).at(i);                     // value of agent to task here
        if(best_value < current_value){
            best_agent = i;
            best_value = current_value;
        }
        else{
            best_agent = best_agent;
            best_value = best_value;
            // do nothing
        }
    }
    return(best_agent);
}

int graph::agents_best_task(int a){
    int best_task;
    double best_value = -1;
    double current_value;
    for(int i=0;i<task_count;i++){
        current_value = Weights.at(i).at(a);                   // value of task to agent here
        if(best_value < current_value){
            best_task = i;
            best_value = current_value;
        }
        else{
            best_task = best_task;
            best_value = best_value;
            // do nothing
        }
    }
    return(best_task);
}

int graph::get_edge_type(int t, int a){
    return(Matching.at(t).at(a));
}
double graph::get_edge_weight(int t, int a){
    return(Weights.at(t).at(a));
}

void graph::update_weight(int t, int a, double weight){
    Weights.at(t).at(a) = weight;
}

double graph::reveal_real_weight(int t, int a){
    double ax = Agents.at(a).get_actual_x();
    double ay = Agents.at(a).get_actual_y();
    double tx = Tasks.at(t).get_actual_x();
    double ty = Tasks.at(t).get_actual_y();
    double x = ax - tx;
    double y = ay = ty;
    x = std::pow(x,2);
    y = std::pow(y,2);
    return(std::sqrt(x+y));
}
