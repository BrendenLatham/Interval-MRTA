#include<iostream>
#include "graph.h"
#include "Probabilities.h"
#include "vertex.h"
#include "task.h"
#include "agent.h"
#include <ctime>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

double OTMM(graph G, int run_type);
void set_intervals();
void set_probabilities();
void compare_results();
void output_test_matrix();
void localize_0_agents();
void localize_10_agents();
void localize_20_agents();
void localize_30_agents();
void localize_all_agents();
bool comparePair(pair<int,double> p1, pair<int,double> p2);

graph Initial_System;
graph Initial_Solution;
graph Gamblers_system;
graph Gamblers_solution;
probabilities probability_analysis;
double resource;


int main(int argc, char* argv[]){
    graph Graph;
    probability_analysis.initialize(atoi(argv[1]),round(atoi(argv[1])*atoi(argv[2])/100));
    Graph.randomize(atoi(argv[1]),round(atoi(argv[1])*atoi(argv[2])/100));
    resource = round(atoi(argv[1])*atof(argv[3])/100);
    std::cout<<"-------------INITIAL SYSTEM----------------"<<std::endl;
    Graph.print_graph();
    std::cout<<"-------------------------------------------"<<std::endl;
    Initial_System = Graph;
    auto start1 = std::chrono::high_resolution_clock::now();
    auto start2 = std::chrono::high_resolution_clock::now();
    OTMM(Graph, 0);
    std::cout<<Initial_Solution.get_matching_value()<<" ";
    auto finish1 = std::chrono::high_resolution_clock::now();
    std::cout<<"-------------INITIAL SOLUTION----------------"<<std::endl;
    Initial_Solution.print_graph();
    std::cout<<"---------------------------------------------"<<std::endl;
    set_intervals();
    std::cout<<"-------------ALLOWABLE ERRORS----------------"<<std::endl;
    probability_analysis.print_error();
    std::cout<<"---------------------------------------------"<<std::endl;
    std::cout<<"-------------TOLERENCE INTERVALS----------------"<<std::endl;
    probability_analysis.print_tolerance();
    std::cout<<"------------------------------------------------"<<std::endl;
    Gamblers_system = Initial_System;
    set_probabilities();
    std::cout<<"-------------PROBABILISTIC SYSTEM----------------"<<std::endl;
    Gamblers_system.print_graph();
    std::cout<<"-------------------------------------------------"<<std::endl;

    Gamblers_solution = Initial_System;
    localize_0_agents();
    OTMM(Gamblers_solution, 2);
    compare_results();

//    Gamblers_solution = Initial_System;
//    localize_10_agents();
 //   OTMM(Gamblers_solution, 2);
    auto finish2 = std::chrono::high_resolution_clock::now();
 //   compare_results();

//    Gamblers_solution = Initial_System;
//    localize_20_agents();
//    OTMM(Gamblers_solution, 2);
//    compare_results();

    Gamblers_solution = Initial_System;
    localize_30_agents();
    OTMM(Gamblers_solution, 2);
    compare_results();

    Gamblers_solution = Initial_System;
    localize_all_agents();
//    Gamblers_solution.print_graph();
    OTMM(Gamblers_solution, 2);
//    compare_results();
    std::cout<<std::endl;
    output_test_matrix();


    std::chrono::duration<double> elapsed1 = finish1 - start1;
    std::chrono::duration<double> elapsed2 = finish2 - start2;

//    cout<<Initial_System.get_agent_count()<<","<<elapsed1.count()<<","<<elapsed2.count()<<endl;


    return(0);
}

//run type 0 for initial matching, 1 for resultant matching.
 double OTMM(graph G, int run_type){
    while(G.is_fully_matched() == false){
        // finding dominating edges
        for(int i=0;i<G.get_task_count();i++){
            if(G.Tasks.at(i).is_full() == false){
                int best_a = G.tasks_best_agent(i);
                if(G.agents_best_task(best_a) == i && G.Agents.at(best_a).is_allocated() == false){
                    G.set_matched(i, best_a);
                     /*   std::pair<int,int> dom;
                        dom.first = i;
                        dom.second = best_a;
                        G.dominating.push_back(dom); */
                }
            }
        }

        while(G.dominating.size() != 0){
            if(G.Tasks.at(0).is_full() == false){
                int best_a = G.tasks_best_agent(0);
                if(G.agents_best_task(best_a) == 0 && G.Agents.at(best_a).is_allocated() == false){
                    G.set_matched(G.dominating.at(0).first, best_a);
                    std::pair<int,int> dom;
                    dom.first = G.dominating.at(0).first;
                    dom.second = best_a;
                    G.dominating.push_back(dom);
                    G.dominating.erase(G.dominating.begin());
                }
                else{
                    G.dominating.erase(G.dominating.begin());
                }
            }
            else{
                 G.dominating.erase(G.dominating.begin());
            }
        }

    }
    if(run_type == 0){
        Initial_Solution = G;
        return(0);
    }
    else{
        if(run_type == 2){
            Gamblers_solution = G;
            return(0);
        }
        else{
        return(G.get_matching_value());
        }
    }

}


void set_intervals(){
    for(int i=0;i<Initial_Solution.get_task_count();i++){
        for(int j=0;j<Initial_Solution.get_agent_count();j++){
            double edge_error;
            graph resultant = Initial_System;
            if(Initial_Solution.get_edge_type(i, j) == 1){
                resultant.hide_edge(i, j, 1);
                edge_error = Initial_Solution.get_matching_value() - OTMM(resultant, 1);
            }
            else{
                resultant.hide_edge(i, j, 0);
                edge_error = Initial_Solution.get_matching_value() - OTMM(resultant, 1);
            }
            probability_analysis.store_error(i, j, edge_error);
            probability_analysis.compute_tolerance(i, j, Initial_Solution.get_edge_type(i, j), Initial_System.get_edge_weight(i, j));
        }
    }
}
void set_probabilities(){
    for(int i=0;i<Initial_System.get_task_count();i++){
        for(int j=0;j<Initial_System.get_agent_count();j++){
            if(Initial_Solution.get_edge_type(i, j) == 0){
                double new_weight = (1-probability_analysis.compute_probability(i, j, Gamblers_system.get_edge_weight(i, j)));
                Gamblers_system.update_weight(i, j, new_weight);
            }
            else{
                double new_weight = (1-probability_analysis.compute_probability(i, j, Gamblers_system.get_edge_weight(i, j)));
                Gamblers_system.update_weight(i, j, new_weight);
            }
        }
    }
}



void compare_results(){
    double initial_value = 0;
    double Gamblers_value = 0;
    double Gambler_estimate_value = 0;
    for(int i=0;i<Gamblers_solution.get_task_count();i++){
        for(int j=0;j<Gamblers_solution.get_agent_count();j++){
            if(Gamblers_solution.get_edge_type(i, j) == 1){
                Gamblers_value = Gamblers_value + Initial_System.reveal_real_weight(i, j);
                Gambler_estimate_value = Gambler_estimate_value + Initial_System.get_edge_weight(i, j);

            }
            if(Initial_Solution.get_edge_type(i, j) == 1){
                initial_value = initial_value+Initial_System.reveal_real_weight(i, j);
            }
        }
    }
    /*
    if(initial_value == Gamblers_value){
        std::cout<<"TIE"<<std::endl;
    }
    else{
        if(Gamblers_value > initial_value){
            std::cout<<"Gambler Won"<<std::endl;
        }
        else{
            std::cout<<"Initial Won"<<std::endl;
        }
    }
    */
    /*
    for(int i=0;i<Gamblers_solution.get_task_count();i++){
        for(int j=0;j<Gamblers_solution.get_agent_count();j++){
            std::cout<<Initial_System.reveal_real_weight(i,j)<<" ";
        }
        std::cout<<std::endl;
    }
    */

    std::cout/*<<initial_value<<" "*/<<Gamblers_value<<" ";
//    std::cout<<Gamblers_value - initial_value<<std::endl;
    //std::cout<<(Gamblers_value-initial_value)/initial_value;

}

void output_test_matrix(){
    for(int i=0;i<Initial_System.get_task_count();i++){
        for(int j=0;j<Initial_System.Tasks.at(i).get_needs();j++){
            for(int k=0;k<Initial_System.get_agent_count();k++){
                std::cout<<Initial_System.reveal_real_weight(i,k)<<" ";
            }
            std::cout<<std::endl;
        }
    }
}

bool comparePair(pair<int,double> p1, pair<int,double> p2){
    return(p1.second > p2.second); // > to sort non-increasing
}

void localize_0_agents(){
    std::vector<std::pair<int,double>> crits;
    for(int i=0;i<Initial_System.get_agent_count();i++){
        pair<int,double> temp_pair;
        temp_pair.first = i;
        temp_pair.second = 0;
        crits.push_back(temp_pair);
    }
    for(int i=0;i<Initial_System.get_agent_count();i++){
        for(int j=0;j<Initial_System.get_task_count();j++){
            crits.at(i).second = crits.at(i).second+Gamblers_system.get_edge_weight(j,i);
        }
    }
    sort(crits.begin(), crits.end(), comparePair); //comment out to localize agents randomly
    /*
    for(int i=0;i<Initial_System.get_agent_count();i++){
        std::cout<<"Agent "<<crits.at(i).first<<" : "<<crits.at(i).second<<std::endl;
    }
    */

    for(int i=0;i<0;i++){
        Gamblers_solution.localize(crits.at(i).first);
    }
}

void localize_10_agents(){
    std::vector<std::pair<int,double>> crits;
    for(int i=0;i<Initial_System.get_agent_count();i++){
        pair<int,double> temp_pair;
        temp_pair.first = i;
        temp_pair.second = 0;
        crits.push_back(temp_pair);
    }
    for(int i=0;i<Initial_System.get_agent_count();i++){
        for(int j=0;j<Initial_System.get_task_count();j++){
            crits.at(i).second = crits.at(i).second+Gamblers_system.get_edge_weight(j,i);
        }
    }
    sort(crits.begin(), crits.end(), comparePair); //comment out to localize agents randomly
    /*
    for(int i=0;i<Initial_System.get_agent_count();i++){
        std::cout<<"Agent "<<crits.at(i).first<<" : "<<crits.at(i).second<<std::endl;
    }
    */

    for(int i=0;i<Initial_System.get_agent_count()*.1;i++){
        Gamblers_solution.localize(crits.at(i).first);
    }
}

void localize_20_agents(){
    std::vector<std::pair<int,double>> crits;
    for(int i=0;i<Initial_System.get_agent_count();i++){
        pair<int,double> temp_pair;
        temp_pair.first = i;
        temp_pair.second = 0;
        crits.push_back(temp_pair);
    }
    for(int i=0;i<Initial_System.get_agent_count();i++){
        for(int j=0;j<Initial_System.get_task_count();j++){
            crits.at(i).second = crits.at(i).second+Gamblers_system.get_edge_weight(j,i);
        }
    }
    sort(crits.begin(), crits.end(), comparePair); //comment out to localize agents randomly
    /*
    for(int i=0;i<Initial_System.get_agent_count();i++){
        std::cout<<"Agent "<<crits.at(i).first<<" : "<<crits.at(i).second<<std::endl;
    }
    */

    for(int i=0;i<Initial_System.get_agent_count()*.2;i++){
        Gamblers_solution.localize(crits.at(i).first);
    }
}

void localize_30_agents(){
    std::vector<std::pair<int,double>> crits;
    for(int i=0;i<Initial_System.get_agent_count();i++){
        pair<int,double> temp_pair;
        temp_pair.first = i;
        temp_pair.second = 0;
        crits.push_back(temp_pair);
    }
    for(int i=0;i<Initial_System.get_agent_count();i++){
        for(int j=0;j<Initial_System.get_task_count();j++){
            crits.at(i).second = crits.at(i).second+Gamblers_system.get_edge_weight(j,i);
        }
    }
    sort(crits.begin(), crits.end(), comparePair); //comment out to localize agents randomly

    for(int i=0;i<Initial_System.get_agent_count();i++){
        std::cout<<"Agent "<<crits.at(i).first<<" : "<<crits.at(i).second<<std::endl;
    }


    for(int i=0;i<Initial_System.get_agent_count()*.4;i++){
        Gamblers_solution.localize(crits.at(i).first);
    }
}


void localize_all_agents(){
    std::vector<std::pair<int,double>> crits;
    for(int i=0;i<Initial_System.get_agent_count();i++){
        pair<int,double> temp_pair;
        temp_pair.first = i;
        temp_pair.second = 0;
        crits.push_back(temp_pair);
    }
    for(int i=0;i<Initial_System.get_agent_count();i++){
        for(int j=0;j<Initial_System.get_task_count();j++){
            crits.at(i).second = crits.at(i).second+Gamblers_system.get_edge_weight(j,i);
        }
    }
    /*
    for(int i=0;i<Initial_System.get_agent_count();i++){
        std::cout<<"Agent "<<crits.at(i).first<<" : "<<crits.at(i).second<<std::endl;
    }
    */
    for(int i=0;i<Initial_System.get_agent_count();i++){
        Gamblers_solution.localize(crits.at(i).first);
    }
}
