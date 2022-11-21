#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

struct edge{
    int task;
    int agent;
};

vector<vector<double>> adj_mat; // initial adjacency matrix
vector<vector<double>> adj_mat_copy; // copy of initial matrix for each agent
vector<vector<int>> matching; //solution
vector<edge> dominating; // dominating edges
vector<int> task_need; // each tasks required agents
vector<int> task_have; // current number of agents in task coalition
int task_count;
int agent_count;


void print_util_mat(vector<vector<double>>,int,int);
void print_match_mat(vector<vector<int>>,int,int);
void generate_system();
void OTMM();

int main(int argc, char* argv[]){
    task_count = atoi(argv[2]);
    agent_count = atoi(argv[1]);

    generate_system();
    adj_mat_copy = adj_mat;

    print_util_mat(adj_mat,task_count,agent_count);

    OTMM();


    print_match_mat(matching,task_count,agent_count);

    return 0;
}

void print_util_mat(vector<vector<double>> vec, int dem1, int dem2){
    for(int i=0;i<dem1;i++){
        for(int j=0;j<dem2;j++){
            cout << vec[i][j] << " ";
        }
        cout<<endl;
    }
}

void print_match_mat(vector<vector<int>> vec, int dem1, int dem2){
    for(int i=0;i<dem1;i++){
        for(int j=0;j<dem2;j++){
            cout << vec[i][j] << " ";
        }
        cout<<endl;
    }
}

void generate_system(){
    vector<double> temp;
    vector<int> temp_match;
    // init random utilities
    srand(time(NULL));
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            temp.push_back(rand()%100);
            temp_match.push_back(0);
        }
        adj_mat.push_back(temp);
        temp.clear();
        // init matching matrix
        matching.push_back(temp_match);
    }
    //temporary for testing
    task_need.push_back(2);
    task_need.push_back(2);
    task_need.push_back(2);
    task_need.push_back(2);
    task_need.push_back(2);
    task_have.push_back(0);
    task_have.push_back(0);
    task_have.push_back(0);
    task_have.push_back(0);
    task_have.push_back(0);
}

void OTMM(){
    int best_match;
    double best_value = 0;
    edge temp;
    int best_for_T = 0;
    vector<int> agent_best_matches;
    vector<int> task_best_matches;
    cout<< "*********PHASE1************"<<endl;
    // finding best matches for agents
    for(int i=0;i<agent_count;i++){
        for(int j=0;j<task_count;j++){
            if(adj_mat[j][i] > best_value){
                best_match = j;
                best_value = adj_mat[j][i];
            }
        }
        agent_best_matches.push_back(best_match);
        best_value = 0;
    }
    // finding best matches for tasks
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(adj_mat[i][j] > best_value){
                best_match = j;
                best_value = adj_mat[i][j];
            }
        }
        task_best_matches.push_back(best_match);
        best_value = 0;
    }
    // displaying best matches
    cout<<"best matches for agents"<<endl;
    for(int i=0;i<agent_count;i++){
        cout<< agent_best_matches[i] << " ";
    }
    cout<<endl;
    cout<<"best matches for tasks"<<endl;
    for(int i=0;i<task_count;i++){
        cout<< task_best_matches[i] << " ";
    }
    cout<<endl;
    // finding dominating edges
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(task_best_matches[i] == j && agent_best_matches[j] == i){
                temp.agent = j;
                temp.task = i;
                dominating.push_back(temp);
            }
        }
    }
    // displaying initial dominating edges
    cout<< "initial dominating edges" <<endl;
    for(int i=0;i<dominating.size();i++){
        cout << "(" << dominating[i].agent << "," << dominating[i].task << ")" << " ";
    }
    cout<<endl;
    // allocating edges
    for(int i=0;i<dominating.size();i++){
        matching[dominating[i].task][dominating[i].agent] = 1;
        for(int j=0;j<task_count;j++){
            adj_mat[j][dominating[i].agent] = -1;
        }
        task_have[dominating[i].task]++;
    }
    // display initial matching
    cout<< "initial matching"<<endl;
    print_match_mat(matching,task_count,agent_count);
    // display utilities
    cout<< "current utility matrix" <<endl;
    print_util_mat(adj_mat,task_count,agent_count);
    cout<< "*********PHASE2************"<<endl;
    while(dominating.size() != 0){
    cout << "-------------------------"<<endl;
        temp = dominating[dominating.size()-1];
        dominating.pop_back();
        // finding best agent for task
        for(int i=0;i<agent_count;i++){
            if(adj_mat[temp.task][i] > best_value){
                best_for_T = i;
                best_value = adj_mat[temp.task][i];
            }
        }
        best_value = 0;
        // finding best matches for agents
        agent_best_matches.clear();
        for(int i=0;i<agent_count;i++){
            for(int j=0;j<task_count;j++){
                if(adj_mat[j][i] > best_value){
                    best_match = j;
                    best_value = adj_mat[j][i];
                }
            }
            agent_best_matches.push_back(best_match);
            best_value = 0;
        }
        // looking for dominating edge
        for(int i=0;i<agent_count;i++){
            // adding to dominating and matching
            if(agent_best_matches[i] == temp.task && best_for_T == i && task_have[temp.task] < task_need[temp.task]){
                temp.agent = i;
                dominating.insert(dominating.begin(),temp);
                matching[temp.task][temp.agent] = 1;
                task_have[temp.task]++;
                for(int j=0;j<task_count;j++){
                    adj_mat[j][temp.agent] = -1;
                }
            }
        }
        if(task_need[temp.task] == task_have[temp.task]){
            for(int i=0;i<agent_count;i++){
                adj_mat[temp.task][i] = -1;
            }
        }
        //displaying info
        cout<< "task best match" <<endl;
        cout<< best_for_T <<endl;
        cout<< "agent best matches"<<endl;
        for(int i=0;i<agent_count;i++){
            cout<< agent_best_matches[i] << " ";
        }
        cout<<endl;
        cout<< "dominating edges" <<endl;
        for(int i=0;i<dominating.size();i++){
            cout<< "(" << dominating[i].agent << "," << dominating[i].task << ")";
        }
        cout<<endl;
        cout<< "matching"<<endl;
        print_match_mat(matching,task_count,agent_count);
        cout<< "current utility matrix" <<endl;
        print_util_mat(adj_mat,task_count,agent_count);
    }
    cout << "-------------------------"<<endl;
}
