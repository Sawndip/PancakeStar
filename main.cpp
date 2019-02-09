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
#include <functional>


using namespace std;
using namespace boost::heap;

struct State{
	size_t id;
	vector<int> list;
	float g;
    float h;
    float f;
    boost::any handler_open;
	//int n = list.size();
	State* parent;
	State( size_t _id,vector<int> _list, double j, double h_, double k, State* f_) :  id(_id),list(_list),g(j),h(h_),f(k), parent(f_) {}
	~State(){}

	State & operator=(const State & _state){
		if(this == &_state) return *this;
		this->id = _state.id;
		this->list = _state.list;
		this->g = _state.g;
		this->h = _state.h;
		this->f = _state.f;//(g+h)*1000000 -g
	};

	bool operator==(const State & _state) const{
		//return (this==&_state);
		return this->id==_state.id;
	};

};

struct compare_states {
    bool operator()(const State* s1, const State* s2) const {
        //return n1.id > n2.id;
        return s1->f > s2->f ;
    }
};

struct hash_states{
    std::size_t operator()(const State* & c) const {
        std::size_t result = c->list.size();
        for(unsigned i = 0; i < c->list.size(); ++i) {
        	boost::hash_combine(result, c->list[i]+i);
        }
        return result;
    }
};

/*
struct equal_states {
		bool operator() (Nodo const* t1, Nodo const* t2) const{
    	//std::cout << "Equal called"<<std::endl;
    	//return t1->id==t2->id && t1->x==t2->x && t1->y==t2->y;
		return t1->id==t2->id;
  	}

}
*/




typedef fibonacci_heap<State*,compare<compare_states> >::handle_type open_handle;
boost::unordered_map<State*, open_handle> open_map;
boost::unordered_map<size_t, State*> node_map; //change to hash,int

//boost::unordered_set<State*,hash_states> state_set;

fibonacci_heap<State*, boost::heap::compare<compare_states> > open;
boost::container::vector<State*> closed;

set<vector<int>> goal_set;

int generated_nodes;
int expanded_nodes;
vector<int> goal_list;
State *solution;

vector<double> average_time;
vector<int> average_sol;
vector<int> average_exp_n;
vector<int> average_gen_n;



size_t gen_id(State* state ){ 
    std::size_t result = state->list.size();
    for(unsigned i = 0; i < state->list.size(); ++i) {
        boost::hash_combine(result, state->list[i]+i);
    }
    return result;
} 

void printList(State* currentState ){
	for (int& x: currentState->list) cout << ' ' << x;
    std::cout << '\n';
}

void printMap(){
	auto it = node_map.begin();
	for(unsigned i = 1; i < 5; ++i) {
		cout<<"nodo id:"<<it->first<<" h:"<<it->second->h<<endl;
		advance(it,i);
	}
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

		generated_nodes++;       
		State* succ = new State(0,flip(_current->list,i),0,0,0,_current);
		succ->id = gen_id(succ);
		succ->g = _current->g +1;              
		
		

		//check if already is generated
		auto in_map = node_map.find(succ->id);
		if(in_map != node_map.end()){
			//in_map->second = succ;
			succ->h = in_map->second->h;		
			
		} else{		
			succ->h = hgap(succ->list)*w;
			
			node_map.emplace(succ->id,succ);

		}
		succ->f = (succ->h+ succ->g)*100000 - succ->g; 

		//check if is open 
		
		auto node_find = open_map.find(succ);
		auto state_f = find(closed.begin(),closed.end(),succ);

		if(node_find != open_map.end()){ //if in the open list

			open.update(node_find->second);
			
		}

		else if(state_f != closed.end()){ //in closed?

			closed.erase(state_f);
			auto prt_open = open.push(succ);
			open_map.emplace(succ,prt_open);
			
		}
		else {

			auto prt_open = open.push(succ);
			open_map.emplace(succ,prt_open);
			

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



State* aStar(vector<int> s0, double w, int lookahead) {
	cout<<"Doing A* search"<<endl;
	int iter = 0;
	State* current;

	State *initial_state = new State(0,s0,0,hgap(s0),0,NULL);
	initial_state->id = gen_id(initial_state);
	//check if is in the transposition map
	auto it_node = node_map.find(initial_state->id);
	if ( it_node != node_map.end() ){
		initial_state->h = it_node->second->h;
	} else {
		node_map.emplace(initial_state->id,initial_state);
	}

	auto prt_open = open.push(initial_state);
	open_map.emplace(initial_state,prt_open);
	
	
	
	while(!open.empty()  && iter < lookahead ){
		
		//count_close_states++;
		current = open.top();

		closed.push_back(current);

		open.pop();
		//update open map
		open_map.erase(open_map.find(current));
		expanded_nodes++; 

		//printList(current); 

		if( checkForGoal(current->list) ) {
			//solution = current ; 
			return current;
		}
		getSuccessors(current,w);
		iter++;
	}
	return current;

}
boost::container::vector<State*>  children(State* s) {
	boost::container::vector<State*> childrens;

	//checking in closed list 
	for (auto &state : closed) {
		
		if(state->parent == s) {
			childrens.push_back(state);
		}
	}
	//checking in open list 
	for (auto &state : open) {
		if(state->parent == s) {
			childrens.push_back(state);
		}
	}
	//imprimir el mejor de la open
	return childrens;

}


int update_h(State* s){
	

	//actualizar mapa, no las estructuras
	boost::container::vector<State*> childrens = children(s);
	auto node_find = open_map.find(s);
	if( node_find != open_map.end() ) {

		return s->h;
	}
	int min = numeric_limits<unsigned short int>::max();
	
	if ( childrens.empty() ){ 
		//cout<<"1"<<endl;
		return min;
	} else {
		//cout<<"2"<<endl;
		for (auto &state : childrens ){
			int h = update_h(state);
			//cout<<"4"<<endl;

			if(min > 1+h){
				min = 1+h;
			}
		}
		auto it_node = node_map.find(s->id);
		if ( it_node != node_map.end() ){
			//cout<<"3"<<endl;
			it_node->second->h = min;
		} else {
			cout<<"State not found"<<endl;
		}

	}

	return min;

}

/*
void update_h (){
	for(unsigned i = 0; i <closed.size() ; ++i) {
		closed[i]->h = numeric_limits<unsigned short int>::max();
		
	}
	
	while(!open.empty()) {
		State* open_b = open.top();

		for(size_t i = 2; i < open_b->list.size(); ++i) {   

			State* succ = new State(0,flip(open_b->list,i),0,0,0,open_b);
			succ->id = gen_id(succ);
			
			auto in_map = node_map.find(succ->id);
			if(in_map != node_map.end()){
				succ = in_map->second;
				succ->f = succ->h;


			} else {
				succ->f = succ->h;
				succ->g = open_b->g +1;
				succ->h = hgap(succ->list);
			}
			
			auto succ_f = find(closed.begin(),closed.end(),succ); 
			if(succ_f != closed.end()){
				cout<<"Entro aqui uwu?"<<endl;
				if((*succ_f)->h > open_b->h + 1/*succ->g )
				{
					(*succ_f)->h = open_b->h + succ->g;
					auto prt_open = open.push(succ);
					open_map.emplace(succ,prt_open);
					cout<<"Entro aqui owo?"<<endl;
				} 
			}		

		}
	}
}

*/


int driver(vector<int> myvector, int lookahead) {
	
	int iter = 0;

	//verificar si llego al objetivo
	
	while(1) {
		State *prop_sol = aStar(myvector,1.5,lookahead);//numero + grande que el numero de operaciones optimas
		cout<<"nodos expandidos dsps astar: "<<expanded_nodes<<endl;
		//get start node
		
		//printList(prop_sol);

		if (checkForGoal(prop_sol->list)) {
			solution = prop_sol; 
			return 1; 

		}
		if(iter > 100000) {
			return -1;	
		}


		State* first = closed[0];
		cout<<"h antes update: "<<first->h<<endl;
		auto it_node= node_map.find(first->id);
		if ( it_node != node_map.end() ){
			cout<<"Doing update_heuristic"<<endl;
			it_node->second->h = update_h(first); 
		} else {
			cout<<"State not found"<<endl;
			return -1;
		}
		cout<<"First h: "<<it_node->second->h<<endl;
		cin.get();
		printMap();

		iter++;
		cout<<node_map.size()<<endl;
		open.clear();
		closed.clear();
		open_map.clear();
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

	double w = 1;
	while(j < n_cases){
		srand (4*(j+1));
		vector<int> myvector = createProblem(atoi(argv[2]),rand()%100);
		goal_set.insert(goal_list);
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

		int resp = driver(myvector,atoi(argv[3]));
		node_map.clear();
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
		
		goal_list.clear();
		goal_set.clear();


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