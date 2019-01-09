#include <iostream>
#include <cstdint> //or <stdint.h>
#include <limits>
#include <string>
#include <vector>
#include <set>
#include <boost/any.hpp>
#include <random> 
#include <chrono>
#include <boost/unordered_map.hpp>
#include <boost/any.hpp>

boost::unordered_map<string, int> h_map;
int generated_nodes;
int expanded_nodes;
vector<int> goal_list;


struct State{
	int id
	vector<int> list;
	float g;
    float h;
    float f;
    boost::any handler_open;
	int n = list.size();
	State father;
	State(int _id, vector<int> _list, double j, double h_, double k, State _f) : id(_id) ,list(_list),g(j),h(h_),f(k) father(_f) {}
	~State(){}

	State & operator=(const State & _state){
		if(this == &_state) return *this;
		this->id = _state.id;
		this->list = _state.list;
		this->g = _state.g;
		this->h = _state.h;
		this->f = _state.f;
		this->father = _state.father;
	};

	bool operator==(const State & _state) const{
		return (this==&_state);
	};

	bool operator<( const State & a,  const State & b) {
    	return(a->f > b->f);

	};

};

string id_gen(vector<int> _v){
	string _id = "";
	for(unsigned i = 0; i <_v.size() ; ++i) {
	 	_id+= to_string(_v[i]);
	 } 
	return _id;
}


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
		return 0;
	}
	goal_set.clear();
	goal_set.insert(goal_list);
	return 1;

}


State* lookahead(State* current){

	State* min = new State(,current->list,0,0,0) ;
	for(size_t i = 2; i < _current->list.size(); ++i) {
		
		     
		State* succ = new State(id_gen(),flip(_current->list,i),0,0,0,current);
		generated_nodes++;  

		//Already visited ? 
		auto s_find = h_map.find(succ);

		if(s_find != h_map.end()){ 
			succ->g = succ->g+1;
			succ->h = s_find->h;
			succ->f = succ->f = succ->h + succ->g;  
		} else {
			succ->h = hgap(succ->list);
			succ->f = succ->h + 1;
			h_map.emplace(succ->id,succ->h);
		}
		//get the min 
		if(min < succ) {
			min = succ;
		}
	}
	return min;       

}

void update(State* _curr&, State* _succ&) {
	if(_curr->h < _succ->f){
		_curr->h = _succ->f;
		auto it = map.find()
		if(it != map.end(_curr->id)) 
    	it->second = _curr->h;
	}
}



State LRTA(State start){

	State *current = start //reset to start

	while(checkForGoal(current->list)){

		State *succ = lookahead(current);
		update(current, succ);

		current = state;
		expanded_nodes++;

	}
	return current;

}

void driver(vector<int> s0){

	State solution = 
	State * start = new state(id_gen(s0),s0,0,hgap(s0),0)
	h_map.emplace(succ->id,succ->h);
	while( solution->g != start->h ){
		solution = LRTA(start);
	}

}		

void clean(){

}


int main(int argc, char const *argv[])
{


	
	return 0;
}