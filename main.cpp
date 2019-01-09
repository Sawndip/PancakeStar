#include <iostream>
#include <cstdint> //or <stdint.h>
#include <limits>
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

struct State{
	int id;
	vector<int> list;
	float g;
    float h;
    float f;
    boost::any handler_open;
	int n = list.size();
	State(int _id, vector<int> _list, double j, double h_, double k) : id(_id) ,list(_list),g(j),h(h_),f(k) {}
	~State(){}

	State & operator=(const State & _state){
		if(this == &_state) return *this;
		this->id = _state.id;
		this->list = _state.list;
		this->g = _state.g;
		this->h = _state.h;
		this->f = _state.f;
	};

	bool operator==(const State & _state) const{
		return (this==&_state);
	};

};

struct compare_states {
    bool operator()(const State* s1, const State* s2) const {
        //return n1.id > n2.id;
        return s1->f > s2->f ;
    }
};


struct hash_states<State>
{
    std::size_t operator()(const State& c) const
    {
        std::size_t result = c.list.size();
        for(unsigned i = 0; i < c.list.size(); ++i) {
        	boost::hash_combine(result, c.list[i]+i);
        }
        return result;
    }
};


typedef fibonacci_heap<State*,compare<compare_states> >::handle_type open_handle;
boost::unordered_map<int, open_handle> node_map;

fibonacci_heap<State*, boost::heap::compare<compare_states> > open;
boost::container::vector<State*> closed;

set<vector<int>> goal_set;

int generated_nodes;
int expanded_nodes;
vector<int> goal_list;
state *solution;

void printList(State* currentState ){
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

void getSuccessors(State* _current, double w){
	
	for(size_t i = 2; i < _current->list.size(); ++i) {
		node_id++;
		generated_nodes++;       
		state* succ = new state(node_id,flip(_current->list,i),0,0,0);
		succ->g = _current->g +1;              
		succ->h = hgap(succ->list)*w;
		succ->f = succ->h+1;//+succ->g; 

		//check if is open 
		
		auto node_find = node_map.find(node_id);
		auto state_f = find(closed.begin(),closed.end(),succ);
		if(node_find != node_map.end()){ //if in the open list
			open.update(node_find->second,succ);
		}

		else if(state_f != closed.end()){ //in closed?
			closed.erase(state_f);
			open.push(succ);
		}
		else {
			auto prt_open = open.push(succ);
			node_map.emplace(node_id,prt_open);

		}
	}

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



int aStar(vector<int> s0, double w) {

	State *initial_state = new state(node_id,s0,0,hgap(s0),0);

	auto prt_open = open.push(initial_state);
	node_map.emplace(node_id,prt_open);
	
	
	while(!open.empty()){
		
		//count_close_states++;
		State* current = open.top();

		closed.push_back(current);

		open.pop();
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

void update_h (){
	for(unsigned i = 0; i <closed.size() ; ++i) {
		closed[i]->h = numeric_limits<unsigned short int>::max();
		
	}
	
	while(!open.empty()) {
		State* open_b = open.top();

		for(size_t i = 2; i < open_b->n; ++i) {   
			cout<<"PCHDISHFCSDIJFCISD"<<endl;    
			State* succ = new state(node_id,flip(open_b->list,i),open_b->g +1,hgap(succ->list),0);
			succ->f = succ->h;
			auto succ_f = find(closed.begin(),closed.end(),succ); 
			if(succ_f != closed.end()){
				if((*succ_f)->h > open_b->h +succ->g /*Es el g? */ ){
					(*succ_f)->h = open_b->h + succ->g;
					open.push(succ);
				} 
			}		

		}
	}
}

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
		//update_h();
		//resp = aStar(myvector,1.0);
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
		open.clear();
		closed.clear();
		goal_list.clear();
		node_map.clear();
		expanded_nodes = 0;
		generated_nodes = 0;

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