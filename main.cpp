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

vector<vector<double>> adj_mat; // dynamic weight matrix used in OTMM
vector<vector<double>> static_adj_mat; //static initial weight for solution processing
vector<vector<double>> adj_mat_copy; // static square weight matrix to recall before each interval computation
vector<vector<double>> square_adj_mat; // dynamic square weight matrix to do work on
vector<vector<int>> matching; // dynamic matching matrix used it OTMM
vector<vector<int>> matching_copy; // static square matching matrix to recall for interval computation
vector<vector<int>> static_matching; // static original matching matrix for solution processing
vector<vector<int>> square_optimal; // dynamic copy of matching matrix as square
vector<vector<int>> discovered_matching; // if more optimal solution is discovered it is stored here
vector<edge> dominating; // dominating edges
vector<int> task_need; // each tasks required agents
vector<int> task_need_copy; // copy of task_needs to hold original tasks required agents after task_needs is initialized to be used in interval function
vector<int> task_have; // current number of agents in task coalition
vector<int> task_cols; // vector holding each subtasks parent task
vector<vector<double>> error; // error for each edge
vector<vector<double>> optimals; // optimal calculated for each edge when determining intervals
int free_agents; // number of agents not yet allocated
int task_count; // number of tasks. this changes when tasks are split into subtasks
int agent_count; // number of agents. this stays constant
int optimal_weight; // will contain optimal weight for initial solution, then all interval solutions
int original_weight; // initial optimal matching weight copied from optimal_weight after initial solution is found
int discovered_weight; // if more optimal solution is discovered its weight is stored here


void print_util_mat(vector<vector<double>>,int,int); // easily prints matrix of doubles
void print_match_mat(vector<vector<int>>,int,int); // easily prints matrix of integers
void generate_system(); // generated random system for the problem
void OTMM(); // runs algorithms one time
void OTMM_iter(); // runs algorithms until all tasks meet required agents
void InitSystem(); // initializes system to pre-interval calculation state for one interval calculation
void MakeSquare(); // square system transformation
void intervals(); // calculates intervals
void PrintIntervals(); // reverses MakeSquare and prints results

int main(int argc, char* argv[]){
    task_count = atoi(argv[2]);
    agent_count = atoi(argv[1]);

    generate_system();
    adj_mat_copy = adj_mat;
    static_adj_mat = adj_mat;

    print_util_mat(adj_mat,task_count,agent_count);

    OTMM_iter();

    static_matching = matching;

    print_match_mat(matching,task_count,agent_count);

    cout<< "Optimal Weight: " << optimal_weight <<endl;

    original_weight = optimal_weight;

    MakeSquare();

    intervals();

    cout<< "-------------optimals------------"<<endl;
    print_util_mat(optimals,agent_count,agent_count);

    cout<< "------------error-----------"<<endl;
    print_util_mat(error,agent_count,agent_count);

    if(discovered_weight > original_weight){
        cout<< "-------------------------------" <<endl;
        cout<< "Higher weighted matching found" <<endl;
        print_match_mat(discovered_matching,agent_count,agent_count);
        cout<< "Weight: " << discovered_weight <<endl;
    }

    PrintIntervals();



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
    int temp_task;
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
    // init free agents
    free_agents = agent_count;
    // randomly assigns needed agents
    for(int i=0;i<task_count;i++){
        task_need.push_back(1);
    }
    for(int i=0;i<agent_count-task_count;i++){
        temp_task = rand()%task_count;
        task_need[temp_task] = task_need[temp_task]+1;
    }
    optimal_weight = 0;

    for(int i=0;i<task_count;i++){
        task_have.push_back(0);
    }


}

void OTMM(){
    int best_match;
    double best_value = 0;
    edge temp;
    int best_for_T = 0;
    vector<int> agent_best_matches;
    vector<int> task_best_matches;
/*    cout<< "*********PHASE1************"<<endl;
    // display what each task needs
    cout<< "task needs" <<endl;
    for(int i=0;i<task_count;i++){
        cout<< task_need[i] << " ";
    }
    cout<<endl;
*/   // finding best matches for agents
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
/*    cout<<"best matches for agents"<<endl;
    for(int i=0;i<agent_count;i++){
        cout<< agent_best_matches[i] << " ";
    }
    cout<<endl;
    cout<<"best matches for tasks"<<endl;
    for(int i=0;i<task_count;i++){
        cout<< task_best_matches[i] << " ";
    }
    cout<<endl;
*/    // finding dominating edges
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(task_best_matches[i] == j && agent_best_matches[j] == i){
                temp.agent = j;
                temp.task = i;
                dominating.push_back(temp);
            }
        }
    }
/*    // displaying initial dominating edges
    cout<< "initial dominating edges" <<endl;
    for(int i=0;i<dominating.size();i++){
        cout << "(" << dominating[i].agent << "," << dominating[i].task << ")" << " ";
    }
    cout<<endl;
*/    // allocating edges
    for(int i=0;i<dominating.size();i++){
        if(task_have[dominating[i].task] != task_need[dominating[i].task]){
            matching[dominating[i].task][dominating[i].agent] = 1;
            optimal_weight = optimal_weight+adj_mat_copy[dominating[i].task][dominating[i].agent];
            for(int j=0;j<task_count;j++){
                adj_mat[j][dominating[i].agent] = -1;
            }
            task_have[dominating[i].task]++;
        }
    }
    // display initial matching
/*    cout<< "initial matching"<<endl;
    print_match_mat(matching,task_count,agent_count);
    // display utilities
    cout<< "current utility matrix" <<endl;
    print_util_mat(adj_mat,task_count,agent_count);
    cout<< "*********PHASE2************"<<endl;
*/    while(dominating.size() != 0){
//    cout << "-------------------------"<<endl;
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
                optimal_weight = optimal_weight+adj_mat_copy[temp.task][temp.agent];
                // hiding agent
                for(int j=0;j<task_count;j++){
                    adj_mat[j][temp.agent] = -1;
                }
            }
        }
        // checking if task has reached needed agents and hiding if true
        if(task_need[temp.task] == task_have[temp.task]){
            for(int i=0;i<agent_count;i++){
                adj_mat[temp.task][i] = -1;
            }
        }
        //displaying info
/*        cout<< "task best match" <<endl;
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
*/        // adjust allocated
        free_agents = free_agents-1;
    }
//    cout << "-------------------------"<<endl;
}

void OTMM_iter(){
    while(free_agents > 0){
//        cout << "free agents: " << free_agents <<endl;
        OTMM();
    }
}

void InitSystem(){
    vector<double> temp;
    vector<int> temp2;
    adj_mat = square_adj_mat;
    adj_mat_copy = square_adj_mat;
    task_count = agent_count;
    matching_copy = square_optimal;
    matching.clear();
    dominating.clear();
    task_need.clear();
    task_have.clear();
    free_agents = agent_count;
    optimal_weight = 0;
    for(int i=0;i<task_count;i++){
        task_need.push_back(1);
        task_have.push_back(0);
    }
    for(int i=0;i<agent_count;i++){
        for(int j=0;j<agent_count;j++){
            temp.push_back(0);
        }
        error.push_back(temp);
        optimals.push_back(temp);
        temp.clear();
    }
    for(int i=0;i<agent_count;i++){
        for(int j=0;j<agent_count;j++){
            temp2.push_back(0);
        }
        matching.push_back(temp2);
        temp2.clear();
    }
}

void intervals(){
//    cout<< "***********interval*************" <<endl;
    discovered_weight = original_weight;
    for(int i=0;i<agent_count;i<i++){
        for(int j=0;j<agent_count;j++){
            InitSystem();
            if(matching_copy[i][j] == 1){
                for(int k=0;k<agent_count;k++){
                    if(task_cols[k] == i){
                        adj_mat[k][i] = -1;
                    }
                }
            }
            else{
                for(int k=0;k<agent_count;k++){
                    adj_mat[i][k] = -1;
                    adj_mat[k][j] = -1;
                    matching[i][j] = 1;
                    task_have[i] = 1;
                }
            }
//            cout<< "-----------current utils----------"<<endl;
//            print_util_mat(adj_mat,agent_count,agent_count);

            OTMM_iter();
            optimals[i][j] = optimal_weight;
            error[i][j] = original_weight - optimal_weight;
            if(optimal_weight > discovered_weight){
                discovered_weight = optimal_weight;
                discovered_matching = matching;
            }
        }
    }
}

void MakeSquare(){
    task_need_copy = task_need;
    vector<int> temp;
    vector<int> index;
    int counter = 0;
    for(int i=0;i<task_count;i++){
        for(int j=0;j<task_need[i];j++){
            task_cols.push_back(i);
        }
    }

    for(int i=0;i<agent_count;i++){
        for(int j=0;j<agent_count;j++){
            temp.push_back(0);
        }
        square_optimal.push_back(temp);
        temp.clear();
    }
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(matching[i][j] == 1){
                index.push_back(j);
            }
        }
    }
    for(int i=0;i<agent_count;i++){
        square_optimal[i][index[i]] = 1;
    }
    index.clear();
    for(int i=0;i<task_count;i++){
        for(int j=0;j<agent_count;j++){
            if(matching[i][j] == 1){
                counter = counter+1;
            }
        }
        index[i] = counter;
        counter = 0;
    }
    for(int i=0;i<task_count;i++){
        for(int j=0;j<index[i];j++){
            square_adj_mat.push_back(adj_mat_copy[i]);
        }
    }
//    cout << "**********Square System*************" <<endl;
//    print_match_mat(square_optimal,agent_count,agent_count);
//    cout<<endl;
//    print_util_mat(square_adj_mat,agent_count,agent_count);
}

void PrintIntervals(){
    vector<vector<double>> error_collapsed;
    vector<double> matched_errors;
    vector<double> temp;

    for(int i=1;i<task_need_copy.size();i++){
        task_need_copy[i] = task_need_copy[i]+task_need_copy[i-1];
    }
    for(int i=0;i<task_need_copy.size();i++){
        task_need_copy[i] = task_need_copy[i]-1;
    }
    for(int i=0;i<task_need_copy.size();i++){
        error_collapsed.push_back(error[task_need_copy[i]]);
    }
    for(int i=0;i<agent_count;i++){
        for(int j=0;j<agent_count;j++){
            if(matching_copy[i][j] == 1){
                matched_errors.push_back(error[i][j]);
            }
        }
    }

    for(int i=0;i<error_collapsed.size();i++){
        for(int j=0;j<agent_count;j++){
            if(static_matching[i][j] == 1){
                error_collapsed[i][j] = matched_errors[0];
                for(int k=1;k<matched_errors.size();k++){
                    temp.push_back(matched_errors[k]);
                }
                matched_errors = temp;
                temp.clear();
            }
        }
    }

    cout<<"------Intervals-------"<<endl;
    for(int i=0;i<error_collapsed.size();i++){
        for(int j=0;j<agent_count;j++){
            if(static_matching[i][j] == 1){
                cout << "[" << static_adj_mat[i][j] - error_collapsed[i][j] << ",oo)" << "  ";
            }
            else{
                cout << "(-oo," << static_adj_mat[i][j] + error_collapsed[i][j] << "]" << "  ";
            }
        }
        cout<<endl;
    }

}
