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
#include <boost/container/flat_set.hpp>

using namespace std;


double inf = std::numeric_limits<double>::infinity();
int generated_nodes;
int expanded_nodes;
set<vector<int>> goal_set;
vector<int> goal_list;


struct State {

	vector<int> list;
	float g;
	int h;
	State *father;
	State(vector<int> _list, double j, int k ,State* _f) : list(_list),g(j), h(k) ,father(_f) {}
	~State(){}

	State & operator=(const State & _state){
		if(this == &_state) return *this;

		this->list = _state.list;
		this->g = _state.g;
		this->h = _state.h;
		this->father = _state.father;
	};


	bool operator==(const State & _state) const{
		return (this==&_state);
	};

};

struct compare_states {
    bool operator()(const State* s1, const State* s2) const {
        //return n1.id > n2.id;
        return s1->h > s2->h ;
    }
};

int hgap(vector<int> numbers) {
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

void clean (){
	expanded_nodes = 0;
	generated_nodes = 0;
	goal_set.clear();
	goal_list.clear();

}	

boost::container::flat_set<State*,compare_states> expand(State* s){
	boost::container::flat_set<State*,compare_states> succs; 
	generated_nodes++;
	for(size_t i = 2; i < s->list.size(); ++i) {
		State* succ = new State(flip(s->list,i),s->g+1,hgap(flip(s->list,i)),s);
		succs.insert(succ);
	}
 
	return succs;

}

void printList(State* currentState ){
	for (int& x: currentState->list) cout << ' ' << x;
    std::cout << '\n';
}

void setPrint(boost::container::flat_set<State*,compare_states> succs){
	cout<<"H SUCCS"<<endl;
	for(auto succ: succs){
		cout<<succ->h<<endl;	}
}

State* DFBnB(State* state, double upperbound) {
		//cout<<"Entro al dfbnb"<<endl;
		if(checkForGoal(state->list)){
			if( state->g < upperbound ) {
				upperbound = state->g; //is it needed ?? 
				cout<<"y llego al goal ??"<<endl;
			}
		}
		else {
			boost::container::flat_set<State*,compare_states> succs  = expand(state);
			setPrint(succs);
			for(auto succ: succs){
				//
				if(succ->g + succ->h < upperbound)
					printList(succ);

					//return DFBnB(succ,upperbound);
					DFBnB(succ,upperbound);
			}
			//
		}
		return state;

}
//set global para no repetir nodos

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

int main(int argc, char const *argv[])
{
	double upperbound = inf;

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

		State* start_n = new State(myvector,0,hgap(myvector),NULL);
		State* sol = DFBnB(start_n,upperbound);

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
			cout<<"Solution: "<<sol->g <<endl;
			cout<<"Nodes generated: "<<generated_nodes<<endl;
			cout<<"Nodes expanded:"<<expanded_nodes<<endl;

			average_time.push_back(time_span.count());
			average_sol.push_back(sol->g);
			average_exp_n.push_back(expanded_nodes);
			average_gen_n.push_back(generated_nodes);
		} else {
			cout<<"No solution find for the problem, NO pancakes for you!"<<endl;
			return 0;
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