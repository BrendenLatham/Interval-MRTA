#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "vertex.h"
#include "graph.h"
#include "task.h"
#include "agent.h"
#include <ctime>
#include <chrono>
#include <bits/stdc++.h>
#include <random>
#include<sys/types.h>
#include <unistd.h>
#include <math.h>

double est_stdv = 30;
int env_size = 100;

using namespace std;

int main(int argc, char* argv[]){
    int rank,size,message_int;
    double message_double;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank == 0){
        graph Graph;
        for(int i=1;i<size;i++){
            MPI_Recv(&message_double,1,MPI_DOUBLE,i,100+i,MPI_COMM_WORLD,&status);
            double real_x = message_double;
            MPI_Recv(&message_double,1,MPI_DOUBLE,i,200+i,MPI_COMM_WORLD,&status);
            double real_y = message_double;
            MPI_Recv(&message_double,1,MPI_DOUBLE,i,300+i,MPI_COMM_WORLD,&status);
            double est_x = message_double;
            MPI_Recv(&message_double,1,MPI_DOUBLE,i,400+i,MPI_COMM_WORLD,&status);
            double est_y = message_double;
            agent Temp(real_x,real_y,est_x,est_y);
            Temp.set_unallocated();
            Graph.Agents.push_back(Temp);
        }
        Graph.initialize_system(3,env_size);
        graph initial_system = Graph;
        initial_system.print_graph();


        while(Graph.is_fully_matched() == false){
            for(int i=0;i<Graph.get_task_count();i++){
                if(Graph.Tasks.at(i).is_full() == false){
                    int best_a = Graph.tasks_best_agent(i);
                    if(Graph.agents_best_task(best_a) == i && Graph.Agents.at(best_a).is_allocated() == false){
                        Graph.set_matched(i, best_a);
                    }
                }
            }

            while(Graph.dominating.size() != 0){
                if(Graph.Tasks.at(0).is_full() == false){
                    int best_a = Graph.tasks_best_agent(0);
                    if(Graph.agents_best_task(best_a) == 0 && Graph.Agents.at(best_a).is_allocated() == false){
                        Graph.set_matched(Graph.dominating.at(0).first, best_a);
                        std::pair<int,int> dom;
                        dom.first = Graph.dominating.at(0).first;
                        dom.second = best_a;
                        Graph.dominating.push_back(dom);
                        Graph.dominating.erase(Graph.dominating.begin());
                    }
                    else{
                        Graph.dominating.erase(Graph.dominating.begin());
                    }
                }
                else{
                    Graph.dominating.erase(Graph.dominating.begin());
                }
            }
        }
        cout<<endl;
        Graph.print_graph();

        vector<int> temp_needs;
        vector<vector<double>> temp_weights;
        vector<double> temp_vec;
        vector<int> temp_assign;
        for(int i=0;i<initial_system.get_agent_count();i++){
            temp_assign.push_back(Graph.get_agent_assignment(i));
        }
        for(int i=0;i<initial_system.get_task_count();i++){
            temp_needs.push_back(initial_system.Tasks.at(i).get_needs());
        }
        for(int i=0;i<initial_system.get_task_count();i++){
            for(int j=0;j<initial_system.get_agent_count();j++){
                temp_vec.push_back(initial_system.get_edge_weight(i,j));
            }
            temp_weights.push_back(temp_vec);
            temp_vec.clear();
        }


        for(int i=1;i<size;i++){
            for(int j=0;j<initial_system.get_task_count();j++){
                MPI_Send(&temp_needs.at(j),1,MPI_INT,i,((j+1)*1000)+i,MPI_COMM_WORLD);
            }

            for(int j=0;j<initial_system.get_task_count();j++){
                for(int k=0;k<initial_system.get_agent_count();k++){
                    MPI_Send(&temp_weights.at(j).at(k),1,MPI_DOUBLE,i,(j*1000)+(k*100)+i,MPI_COMM_WORLD);
                }
            }
            MPI_Send(&temp_assign.at(i-1),1,MPI_INT,i,i,MPI_COMM_WORLD);
        }

        double initial_value = Graph.get_matching_value();
        for(int i=1;i<size;i++){
            MPI_Send(&initial_value,1,MPI_DOUBLE,i,2100+i,MPI_COMM_WORLD);
        }

	}
    else{
        pid_t pid = getpid();
        srand(pid);
        std::default_random_engine generator;
        generator.seed(pid);
        double real_x = rand()%env_size;
        double real_y = rand()%env_size;
        std::normal_distribution<double> x_distribution(real_x,est_stdv);
        std::normal_distribution<double> y_distribution(real_y,est_stdv);
        double est_x = x_distribution(generator);
        double est_y = y_distribution(generator);
        MPI_Send(&real_x,1,MPI_DOUBLE,0,100+rank,MPI_COMM_WORLD);
        MPI_Send(&real_y,1,MPI_DOUBLE,0,200+rank,MPI_COMM_WORLD);
        MPI_Send(&est_x,1,MPI_DOUBLE,0,300+rank,MPI_COMM_WORLD);
        MPI_Send(&est_y,1,MPI_DOUBLE,0,400+rank,MPI_COMM_WORLD);

        graph Graph;
        for(int i=1;i<size;i++){
            agent Temp(0,0,0,0);
            Temp.set_unallocated();
            Graph.Agents.push_back(Temp);
        }
        Graph.initialize_partial_system(3,env_size);

        for(int i=0;i<Graph.get_task_count();i++){
            MPI_Recv(&message_int,1,MPI_INT,0,((i+1)*1000)+rank,MPI_COMM_WORLD,&status); //needs sent from 0
            cout<<message_int<<endl;
            for(int j=1;j<message_int;j++){
                Graph.Tasks.at(i).increase_needs();
            }
        }

        for(int i=0;i<Graph.get_task_count();i++){
            for(int j=0;j<Graph.get_agent_count();j++){
                MPI_Recv(&message_double,1,MPI_DOUBLE,0,(i*1000)+(j*100)+rank,MPI_COMM_WORLD,&status); // weights sent from 0
                Graph.update_weight(i,j,message_double);
            }
        }

        MPI_Recv(&message_int,1,MPI_INT,0,rank,MPI_COMM_WORLD,&status);
        int assigned = message_int;

        graph initial_system = Graph;

        vector<double> agent_values;
        vector<double> agent_intervals;
        vector<double> agent_probabilities;
        vector<double> agent_zscores;

        for(int l=0;l<Graph.get_task_count();l++){
            Graph = initial_system;
            if(l == assigned){
                Graph.update_weight(l,rank-1,0);
            }
            else{
                Graph.set_matched(l,rank-1);
            }
            while(Graph.is_fully_matched() == false){
                for(int i=0;i<Graph.get_task_count();i++){
                    if(Graph.Tasks.at(i).is_full() == false){
                        int best_a = Graph.tasks_best_agent(i);
                        if(Graph.agents_best_task(best_a) == i && Graph.Agents.at(best_a).is_allocated() == false){
                            Graph.set_matched(i, best_a);
                        }
                    }
                }

                while(Graph.dominating.size() != 0){
                    if(Graph.Tasks.at(0).is_full() == false){
                        int best_a = Graph.tasks_best_agent(0);
                        if(Graph.agents_best_task(best_a) == 0 && Graph.Agents.at(best_a).is_allocated() == false){
                            Graph.set_matched(Graph.dominating.at(0).first, best_a);
                            std::pair<int,int> dom;
                            dom.first = Graph.dominating.at(0).first;
                            dom.second = best_a;
                            Graph.dominating.push_back(dom);
                            Graph.dominating.erase(Graph.dominating.begin());
                        }
                        else{
                            Graph.dominating.erase(Graph.dominating.begin());
                        }
                    }
                    else{
                        Graph.dominating.erase(Graph.dominating.begin());
                    }
                }
            }
            agent_values.push_back(Graph.get_matching_value());
        }

        MPI_Recv(&message_double,1,MPI_DOUBLE,0,2100+rank,MPI_COMM_WORLD,&status);
        double initial_value = message_double;

        for(int i=0;i<initial_system.get_task_count();i++){
            agent_values.at(i) = initial_value-agent_values.at(i);
            if(i == assigned){
                agent_intervals.push_back(initial_system.get_edge_weight(i,rank-1)-agent_values.at(i));
                if(initial_system.get_edge_weight(i,rank-1) > agent_intervals.at(i)){
                    agent_zscores.push_back((initial_system.get_edge_weight(i,rank-1)-agent_intervals.at(i))/est_stdv);
                }
                else{
                    agent_zscores.push_back(-1*(initial_system.get_edge_weight(i,rank-1)-agent_intervals.at(i))/est_stdv);
                }
            }
            else{
                agent_intervals.push_back(initial_system.get_edge_weight(i,rank-1)+agent_values.at(i));
                if(initial_system.get_edge_weight(i,rank-1) < agent_intervals.at(i)){
                    agent_zscores.push_back((agent_intervals.at(i)-initial_system.get_edge_weight(i,rank-1))/est_stdv);
                }
                else{
                    agent_zscores.push_back(-1*(agent_intervals.at(i)-initial_system.get_edge_weight(i,rank-1))/est_stdv);
                }
            }
            agent_probabilities.push_back(1-(0.5*std::erfc(-1*agent_zscores.at(i) * M_SQRT1_2)));
        }
        double relative_crit = 0;
        for(int i=0;i<initial_system.get_task_count();i++){
            relative_crit = relative_crit+agent_probabilities.at(i);
        }
        cout<<"process "<<rank<<" crit "<<relative_crit<<endl;
    }

	MPI_Finalize();
    return(0);
}
