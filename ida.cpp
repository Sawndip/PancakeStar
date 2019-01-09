#include <iostream>
#include <cstdint> //or <stdint.h>
#include <limits>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <bits/stdc++.h> 
#include <random> 
#include <chrono>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>


using namespace std;

double inf = std::numeric_limits<double>::infinity();
double threshold;
int generated_nodes;
int expanded_nodes;
set<vector<int>> goal_set;
vector<int> goal_list;


struct State {

	vector<int> list;
	float g;

	State *father;
	State(vector<int> _list, double j, State* _f) : list(_list),g(j), father(_f) {}
	~State(){}

	State & operator=(const State & _state){
		if(this == &_state) return *this;

		this->list = _state.list;
		this->g = _state.g;
		this->father = _state.father;
	};


	bool operator==(const State & _state) const{
		return (this==&_state);
	};

};

		
	
namespace std
{
    template <>
    struct hash<State>
    {
        size_t operator()(const State& k) const
        {
        	int seed = 0;
        	for(unsigned i = 0; i < k.list.size(); ++i) {
        		seed+= (k.list[i]+i);
        	}
            // Compute individual hash values for two data members and combine them using XOR and bit shifting
            //return ((hash<float>()(k.getM()) ^ (hash<float>()(k.getC()) << 1)) >> 1);
            return hash<int>()(seed);
        }
    };
}


int hgap(vector<int> numbers){
    int gap = 0;
    for(size_t i = 0; i<numbers.size()-1; ++i){

        if (abs(numbers[i]-numbers[i+1]) != 1){
                gap++;
            }

    }
    return gap;
}

vector<int> slicev(vector<int>::iterator first, vector<int>::iterator last){
	vector<int> aux; 
	for (vector<int>::iterator cur = first; cur != last; ++cur) {
       aux.push_back(*cur);
    }
    return aux;
}
vector<int> flip(vector<int> s1, int i) {
	int back = s1.back();
	s1.pop_back();
	for(unsigned i = 0; i < s1.size()-1; ++i) {
		
	}
	vector<int> previous = slicev(s1.begin(),s1.begin()+i);


	vector<int> aux_append = slicev(s1.begin()+i,s1.end());
	
	reverse(previous.begin(), previous.end());
	previous.insert( previous.end(), aux_append.begin(), aux_append.end() );
	previous.push_back(back);
	return previous;
}

bool checkForGoal(vector<int> s_candidate)  {


	int old_size = goal_set.size();
	goal_set.insert(s_candidate);
	int new_size = goal_set.size(); 

	if(old_size == new_size) {
		return 1;
	}
	goal_set.clear();
	goal_set.insert(goal_list);
	return 0;

}

unordered_set<State*> expand(State* s){
	unordered_set<State*> succs; 
	generated_nodes++;
	for(size_t i = 2; i < s->list.size(); ++i) {
		State* succ = new State(flip(s->list,i),0,s);
		succs.emplace(succ);
	}
 
	return succs;

}

State* IDA( State* state, double g , double upperbound ){

	if (checkForGoal(state->list)){
		state->g = g;
		return state;
	}
	unordered_set<State*> succ_set = expand(state); //get sucer
	for(auto succ : succ_set){
		if(g + 1 + hgap(succ->list) > upperbound ){
			if(g + 1 + hgap(succ->list) < threshold){
				threshold = g + 1 + hgap(succ->list);
			}
		}
		else{
			State* p = IDA(succ,g+1,upperbound);
			if(p != NULL){
				expanded_nodes++;
				p->g = g;
				return p;
			}
		}
	}

	return NULL;
}


State* driver(vector<int> s0){

	threshold = hgap(s0);
	State* solution;
	State* start = new State(s0,hgap(s0),NULL);
	while( solution == NULL && threshold != inf ){
		double upperbound = threshold;
		threshold = inf;
		solution = IDA(start,0,upperbound);
	}
	return solution;
}

vector<int> createProblem (int n, int seed){

	vector<int> v0;
	//unsigned seed = chrono::system_clock::now().time_since_epoch().count();

    default_random_engine e(seed);
   	//populate goal and create vector
	for(int i = 1; i < n+1; ++i) {
		goal_list.push_back(i);
		v0.push_back(i);
	}
	int back = v0.back();
	v0.pop_back();

	shuffle(v0.begin(), v0.end(), e);

	v0.push_back(back);
	return v0;
}

int getSolution(State* sol){
	
	int n_flips = 0;
	State* aux = sol;
	while(aux != NULL) {
		aux = aux->father;
		n_flips++;
	}
	return n_flips;
}

void clean (){
	expanded_nodes = 0;
	generated_nodes = 0;
	goal_set.clear();
	goal_list.clear();

}	

int main(int argc, char const *argv[])
{
	int n_cases = atoi(argv[1]);
	int j =0 ;
	vector<double> average_time;
	vector<int> average_sol;
	vector<int> average_exp_n;
	vector<int> average_gen_n;
	while(j < n_cases){
		srand (4*(j+1));
		vector<int> myvector = createProblem(atoi(argv[2]),rand()%100);
		
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

		State* sol = driver(myvector);

		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
		if (sol != NULL){

			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);
			cout<< "Problem solved: ";
			for (int& x: myvector) cout << ' ' << x;
    		cout << '\n';
    		cout<< "Problem solution: ";
    		for (int& x: sol->list) cout << ' ' << x;
    		cout << '\n';
			cout<<"Duration: "<<time_span.count()<<endl;
			cout<<"Solution: "<<getSolution(sol) <<endl;
			cout<<"Nodes generated: "<<generated_nodes<<endl;
			cout<<"Nodes expanded:"<<expanded_nodes<<endl;

			average_time.push_back(time_span.count());
			average_sol.push_back(getSolution(sol));
			average_exp_n.push_back(expanded_nodes);
			average_gen_n.push_back(generated_nodes);
		} else {
			cout<<"No solution find for the problem, NO pancakes for you!"<<endl;
		}
		clean();

		j++;
	}
	double avg_t;
	int avg_sol;
	int avg_exp;
	int avg_gen;
	for(unsigned i = 0; i < average_time.size(); ++i) {
		avg_t+=average_time[i];
		avg_sol+=average_sol[i];
		avg_gen+=average_gen_n[i];
		avg_exp+=average_exp_n[i];
	}
	cout<<"Average Time: "<<avg_t/average_time.size()<<endl;
	cout<<"Average Solution: "<<avg_sol/average_sol.size()<<endl;
	cout<<"Average Expanded Nodes: "<<avg_exp/average_exp_n.size()<<endl;
	cout<<"Average Generated Nodes: "<<avg_gen/average_gen_n.size()<<endl;

	return 0;
}