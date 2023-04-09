#include<iostream>
#include <math.h>
#include "Probabilities.h"

void probabilities::initialize(int agent_count, int task_count){
    this->task_count = task_count;
    this->agent_count = agent_count;
    std::vector<double> temp1;
    std::vector<std::pair<double,int>> temp_pairs;
    std::pair<double, int> temp_pair;
    for(int i=0;i<agent_count;i++){
        temp1.push_back(0);
        temp_pairs.push_back(temp_pair);
    }
    for(int i=0;i<task_count;i++){
        error.push_back(temp1);
        Intervals.push_back(temp_pairs);
    }
}
void probabilities::store_error(int t, int a, double new_error){
    error.at(t).at(a) = new_error;
}
void probabilities::print_error(){
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            std::cout<<error.at(i).at(j)<<" ";
        }
        std::cout<<std::endl;
    }
}
void probabilities::compute_tolerance(int t, int a, int type, double weight){
    Intervals.at(t).at(a).second = type;
    if(type == 1){
        Intervals.at(t).at(a).first = weight - error.at(t).at(a);
    }
    else{
        Intervals.at(t).at(a).first = weight + error.at(t).at(a);
    }
}
void probabilities::print_tolerance(){
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(Intervals.at(i).at(j).second == 1){
                std::cout<<"["<<Intervals.at(i).at(j).first<<",+00)"<<" ";
            }
            else{
                std::cout<<"(-00,"<<Intervals.at(i).at(j).first<<"]"<<" ";
            }
        }
        std::cout<<std::endl;
    }
}

double probabilities::compute_probability(int t, int a, double weight){
    double z_score;
    if(Intervals.at(t).at(a).second == 1){
        if(weight > Intervals.at(t).at(a).first){
            z_score = -1*(weight-Intervals.at(t).at(a).first)/10;             // double check later
        }
        else{
            z_score = (weight-Intervals.at(t).at(a).first)/10;
        }
    }
    else{
        if(Intervals.at(t).at(a).first > weight){
            z_score = -1*(Intervals.at(t).at(a).first-weight)/10;
        }
        else{
            z_score = (weight-Intervals.at(t).at(a).first)/10;
        }
    }
    return (0.5*std::erfc(-z_score * M_SQRT1_2));
}

