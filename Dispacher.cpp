//
// Created by Neria on 07/08/2019.
//

#include "Dispacher.h"

int Dispatcher::total_dispatched_jobs = 0;

int Dispatcher::get_destination()
{
    int destination = rand() % num_servers_;
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

    while ( random_idx.size() < poc)
        random_idx.insert(rand() % num_servers_);

    for (auto elem : random_idx) {
        if( servers[elem]->GetQueuedJobs() < min_queue ){
            min_queue = servers[elem]->GetQueuedJobs();
            min_index = elem;
        }
    }
    routing_map[min_index] ++;
    return min_index;
}

int JsqDispatcher::get_destination()
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
