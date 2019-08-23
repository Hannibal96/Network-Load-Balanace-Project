//
// Created by Neria on 07/08/2019.
//

#include "Dispacher.h"

int Dispatcher::total_dispatched_jobs = 0;

int Dispatcher::get_destination(Server** servers)
{
    int destination = rand() % num_servers_;

    if(buffer.CheckReRoute( *servers[destination] )) {
        cout << "ReRouting" << endl;
        exit(1);
    }

    routing_map[destination] ++;
    return destination;
}

int Dispatcher::get_arrivals()
{
    int new_arraivals = poisson_distribution_(main_generator_);
    dispatched_jobs += new_arraivals;
    total_dispatched_jobs += new_arraivals;
    return new_arraivals;
}

string Dispatcher::toString() const {

    string dispatcher_print = "Dispatcher Number: "+::to_string(id_)+
                              "\n  -dispached jobs: "+::to_string(dispatched_jobs)+"\n";
    for (auto const& x : routing_map)
        dispatcher_print += "   "+::to_string(x.first) +": "+::std::to_string(x.second)+"\n";

    dispatcher_print += buffer.toString();

    return dispatcher_print;
}

std::ostream& operator<<(std::ostream& os, const Dispatcher& dsp)
{
    os << dsp.toString();
    return os;
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
    routing_map[min_index] ++;
    return min_index;
}

int JsqDispatcher::get_destination(Server** servers)
{
    int min_index = servers_heap_->GetMin();
    int new_queued_number = servers_heap_->GetVal(min_index) + 1;
    servers_heap_->UpdateKey(min_index, new_queued_number);
    routing_map[min_index] ++;
    return min_index;
}

void JsqDispatcher::update_server(int server_num, int finished_jobs)
{
    int new_queued_number = servers_heap_->GetVal(server_num) - finished_jobs;
    assert(new_queued_number >= 0 && "-W- Assert, JsqDispatcher::update_server :   new_queued_number < 0" );
    servers_heap_->UpdateKey(server_num, new_queued_number);
}

int JiqDispatcher::get_destination(Server** servers){
    int destination = -1;
    if(idle_servers.size() == 0)
        destination = rand() % num_servers_;
    else {
        int rand_idle = rand() % idle_servers.size();
        destination = idle_servers[rand_idle];
        idle_servers.erase(std::remove(idle_servers.begin(), idle_servers.end(), destination), idle_servers.end());
    }
    assert(destination != -1 && "-W- Assert, JiqDispatcher::get_destination :   destination == -1" );
    routing_map[destination] ++;
    return destination;
}

void JiqDispatcher::update_server(int server_num, bool is_idle){
    if(is_idle)
        idle_servers.push_back(server_num);
}


int PiDispatcher::get_destination(Server** servers){
    int destination = -1;
    if(idle_servers.size() == 0)
        destination = last_idle_server ;
    else {
        int rand_idle = rand() % idle_servers.size();
        destination = idle_servers[rand_idle];
        idle_servers.erase(std::remove(idle_servers.begin(), idle_servers.end(), destination), idle_servers.end());
        if(idle_servers.size() == 0)
            last_idle_server = destination;
    }
    assert(destination != -1 && "-W- Assert, PiDispatcher::get_destination :   destination == -1" );
    routing_map[destination] ++;
    return destination;
}

void PiDispatcher::update_server(int server_num, bool is_idle){
    if(is_idle)
        idle_servers.push_back(server_num);
}

int RrDispatcher::get_destination(Server** servers){
    int destination = (current ++) % num_servers_;
    routing_map[destination] ++ ;
    return destination;
}
