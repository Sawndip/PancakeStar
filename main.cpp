#include <iostream>
#include <cstdint> //or <stdint.h>
#include <limits>
#include <limits.h>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/any.hpp>
#include <random> 

#include <boost/container/vector.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/array.hpp>
#include <boost/unordered_map.hpp>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace boost::heap;

struct state{
	int id;
	vector<int> list;
	float g;
    float h;
    float f;
    boost::any handler_open;
	int n = list.size();
	state(int _id, vector<int> _list, double j, double h_, double k) : id(_id) ,list(_list),g(j),h(h_),f(k) {}
	~state(){}
};
struct compare_states {
    bool operator()(const state* s1, const state* s2) const {
        //return n1.id > n2.id;
        return s1->f > s2->f ;
    }
};

int node_id=0;
typedef fibonacci_heap<state*,compare<compare_states> >::handle_type open_handle;
boost::unordered_map<int, open_handle> node_map;

fibonacci_heap<state*, boost::heap::compare<compare_states> > open_list;
boost::container::vector<state> close_list;

set<vector<int>> goal_set;

int generated_nodes;
int expanded_nodes;
vector<int> goal_list;
state *solution;

void printList(state* currentState ){
	for (int& x: currentState->list) cout << ' ' << x;
    std::cout << '\n';
}

	

vector<int> slice(vector<int>::iterator first, vector<int>::iterator last){
	vector<int> aux; 
	for (vector<int>::iterator cur = first; cur != last; ++cur) {
       aux.push_back(*cur);
    }
    return aux;
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

vector<int> flip(vector<int> s1, int i) {
	int back = s1.back();
	s1.pop_back();
	for(unsigned i = 0; i < s1.size()-1; ++i) {
		
	}
	vector<int> previous = slice(s1.begin(),s1.begin()+i);


	vector<int> aux_append = slice(s1.begin()+i,s1.end());
	
	reverse(previous.begin(), previous.end());
	previous.insert( previous.end(), aux_append.begin(), aux_append.end() );
	previous.push_back(back);
	return previous;
}

void getSuccessors(state* _current, double w){
	
	for(size_t i = 2; i < _current->list.size(); ++i) {
		node_id++;
		generated_nodes++;       
		state* succ = new state(node_id,flip(_current->list,i),0,0,0);
		succ->g = _current->g +1;              
		succ->h = hgap(succ->list)*w;
		succ->f = succ->h+succ->g; 
		//check if is open 
		
		auto node_find = node_map.find(node_id);
		if(node_find != node_map.end()){ //if in the open list
			open_list.update(node_find->second,succ);
		}
		else {
			open_list.push(succ);
		}
	}

} 
vector<int> createProblem (int n, int seed){

	vector<int> v0;
	//unsigned seed = chrono::system_clock::now().time_since_epoch().count();

    default_random_engine e(seed);
   	//populate goal and create vector
	for(unsigned i = 1; i < n+1; ++i) {
		goal_list.push_back(i);
		v0.push_back(i);
	}
	int back = v0.back();
	v0.pop_back();



	shuffle(v0.begin(), v0.end(), e);

	v0.push_back(back);


	return v0;
}





int aStar(vector<int> s0, double w) {

	state *initial_state = new state(node_id,s0,0,0,0);

	auto prt_open = open_list.push(initial_state);
	node_map.emplace(node_id,prt_open);
	
	
	while(!open_list.empty()){
		
		//count_close_states++;
		state* current = open_list.top();
		open_list.pop();
		expanded_nodes++;  
		//cout<<"Current state:"<<endl;
		//printList(current);
		//IF ARRAY IS DONE
		if(checkForGoal(current->list)){
			solution = current ; 
			return 1;
		}
		getSuccessors(current,w);
		
	}
	return -1;

}

update_h 

//TODO:
//revisar si la heuristica convergio
//se actualiza (ver paper) la open
//y de ahi se genera
//add loop to be anytime )where is the break_ save all h values in a hash ma


int main(int argc, char const *argv[])
{
	int n_cases = atoi(argv[1]);
	int j =0 ;
	vector<double> average_time;
	vector<int> average_sol;
	vector<int> average_exp_n;
	vector<int> average_gen_n;
	double w = 1;
	while(j < n_cases){
		srand (4*(j+1));
		vector<int> myvector = createProblem(atoi(argv[2]),rand()%100);
		goal_set.insert(goal_list);
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

		int resp = aStar(myvector,1.0);

		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
		if (resp != -1){

			chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end-start);
			cout<< "Problem solved: ";
			for (int& x: myvector) cout << ' ' << x;
    		cout << '\n';
			cout<<"Duration: "<<time_span.count()<<endl;
			cout<<"Solution: "<<solution->g <<endl;
			cout<<"Nodes generated: "<<generated_nodes<<endl;
			cout<<"Nodes expanded:"<<expanded_nodes<<endl;

			average_time.push_back(time_span.count());
			average_sol.push_back(solution->g);
			average_exp_n.push_back(expanded_nodes);
			average_gen_n.push_back(generated_nodes);
		} else {
			cout<<"No solution find for the problem, NO pancakes for you!"<<endl;
		}

		goal_set.clear();
		open_list.clear();
		close_list.clear();
		goal_list.clear();
		node_map.clear();
		expanded_nodes = 0;
		generated_nodes = 0;
		node_id=0;
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