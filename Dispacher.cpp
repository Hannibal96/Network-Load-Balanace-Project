//
// Created by Neria on 07/08/2019.
//

#include "Dispacher.h"

int Dispatcher::total_dispatched_jobs = 0;

int Dispatcher::get_arrivals()
{
    int new_arraivals = poisson_distribution_(main_generator_);
    dispatched_jobs += new_arraivals;
    total_dispatched_jobs += new_arraivals;
    return new_arraivals;
}

map<int, int> Dispatcher::get_routing_map() {
    return routing_map;
}

void Dispatcher::update_routing_table(int server_id) {  // id = -1 meaning buffered
    if(server_id == -1)
        bufferd_jobs ++;
    else if(server_id >= 0)
        routing_map[server_id] ++;
    else
        assert("-W- Assert, Dispatcher::update_routing_table - server_id not valid" );
}

string Dispatcher::toString() const
{
    string dispatcher_print = "***************************************************\n"
                              "***************** Dispatchers *********************\n"
                              "***************************************************\n";
    dispatcher_print += "Dispatcher Number: "+to_string(id_)+
                        "\n  -dispached jobs: "+to_string(dispatched_jobs)+"\n";
    for (auto const& x : routing_map)
        dispatcher_print += "   "+to_string(x.first) +": "+to_string(x.second)+"\n";
    dispatcher_print += "Buffered jobs: " + to_string(bufferd_jobs)+"\n";
    return dispatcher_print;
}

std::ostream& operator<<(std::ostream& os, const Dispatcher& dsp)
{
    os << dsp.toString();
    return os;
}

/***************************************************
 ***************** destinations  *******************
 ***************************************************/

int Dispatcher::get_destination()
{
    int destination = rand() % num_servers_;
    return destination;
}

int PocDispatcher::get_destination(Server** servers, int poc )
{
    int min_queue = std::numeric_limits<int>::max();
    int min_index = -1;
    set<int> random_idx;

    while ( random_idx.size() < poc) {
        int r = rand() % num_servers_;
        random_idx.insert(r);
    }
    assert(random_idx.size() == poc &&
         "-W- Assert, PocDispatcher::get_destination : number of random indices dosen't match POC value" );
    for (auto elem : random_idx) {
        if( servers[elem]->GetQueuedJobs() < min_queue ){
            min_queue = servers[elem]->GetQueuedJobs();
            min_index = elem;
        }
    }
    assert(min_index >= 0 && "-W- Assert, PocDispatcher::get_destination : min_index < 0" );
    return min_index;
}

int JsqDispatcher::get_destination()
{
    int min_index = servers_heap_->GetMin();
    assert(servers_heap_->GetVal(min_index) >= 0 && "-W- Assert, JsqDispatcher::get_destination : min_index value < 0" );
    return min_index;
}

int OptDispatcher::get_destination(Server** servers)
{
    int min_index = -1;
    double min_value = std::numeric_limits<double>::max();
    for(int s_idx = 0; s_idx < num_servers_; s_idx++){
        double avg_serving = (1/(servers[s_idx]->GetMU())) - 1;
        double estimated = servers[s_idx]->GetQueuedJobs()/avg_serving;
        if(estimated < min_value)
        {
            min_value = estimated;
            min_index = s_idx;
        }
    }
    assert(min_index >= 0 && "-W- Assert, OptDispatcher::get_destination : min_index was not init" );
    return min_index;
}

int JiqDispatcher::get_destination(){
    int destination;
    if(idle_servers.empty()) {
        destination = rand() % num_servers_;
    }
    else {
        int rand_idle = rand() % idle_servers.size();
        destination = idle_servers[rand_idle];
    }
    assert(destination >= 0 && "-W- Assert, JiqDispatcher::get_destination :   destination == -1" );
    return destination;
}

int PiDispatcher::get_destination(){
    if( idle_servers.empty() ) {
        return last_idle_server;
    }
    int rand_idle = rand() % idle_servers.size();
    return idle_servers[rand_idle];
}

int RrDispatcher::get_destination(){
    int destination = (current) % num_servers_;
    return destination;
}

/***************************************************
 ***************** route update  *******************
 ***************************************************/
void Dispatcher::update_server_route(int destination)
{
}       //  pointless function only for compilation and simplicity of the simulator

void JsqDispatcher::update_server_route(int destination)
{
    int new_queued_number = servers_heap_->GetVal(destination) + 1;
    servers_heap_->UpdateKey(destination, new_queued_number);
    assert(new_queued_number == servers_heap_->GetVal(destination) &&
            "-W- Assert, JsqDispatcher::update_server :   new_queued_number < 0" );
}

void PiDispatcher::update_server_route(int destination){
    idle_servers.erase(std::remove(idle_servers.begin(), idle_servers.end(), destination), idle_servers.end());
    if(idle_servers.empty())
        last_idle_server = destination;
}

void JiqDispatcher::update_server_route(int destination) {
    idle_servers.erase(std::remove(idle_servers.begin(), idle_servers.end(), destination), idle_servers.end());
}

void RrDispatcher::update_server_route() {
    current ++;
}

/***************************************************
 ***************** finish update  ******************
 ***************************************************/

void JsqDispatcher::update_server_finish(int server_num, int finished_jobs)
{
    int new_queued_number = servers_heap_->GetVal(server_num) - finished_jobs;
    assert(new_queued_number >= 0 && "-W- Assert, JsqDispatcher::update_server :   new_queued_number < 0" );
    if(finished_jobs > 0)
        servers_heap_->UpdateKey(server_num, new_queued_number);
    assert(new_queued_number == servers_heap_->GetVal(server_num) &&
           "-W- Assert, JsqDispatcher::update_server :   new_queued_number < 0" );
}

void JiqDispatcher::update_server_finish(int server_num, pair<int, bool> finish_jobs){
    bool is_idle = finish_jobs.second;
    int jobs = finish_jobs.first;
    if( is_idle && jobs )                   // check if the queue is idle and wasnt idle before
        idle_servers.push_back(server_num);
}




