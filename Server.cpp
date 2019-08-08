//
// Created by Neria on 06/08/2019.
//

#include "Server.h"

Server::Server(int id, double mu, int queue_max) : completed_jobs_(mu){
    this->id = id;
    this->mu = mu;
    this->queue_max = queue_max;
    jobs_in_queue=0;
    served_jobs=0;
    jobs_queue = queue<Job>();
    generator_.seed(id);
}

Server::~Server(){}

void Server::AddJob(Job job) {
    jobs_queue.push(job);               // TODO: add condition for maxed queue
    jobs_in_queue ++ ;
}

void Server::FinishJob(int time) {
    int offered_service = completed_jobs_(generator_);
    int serving_jobs = std::min(offered_service, jobs_in_queue);
    assert(jobs_in_queue == jobs_queue.size() && "-W- Assert, Server::FinishJob queue size mismatch" );
    for(int i=0 ; i< serving_jobs;i++) {
        jobs_queue.front().FinishJob(time);
        jobs_queue.pop();
        jobs_in_queue --;
        assert( jobs_in_queue >= 0 && "-W- Assert, Server::FinishJob queue size smaller than zero" );
    }

}

string Server::toString() const {
    return "Id: "+::to_string(id)+"\n -jobs_in_queue: "+::to_string(jobs_in_queue)+"\n -served_jobs: "+::to_string(served_jobs);
}

/*
ofstream& operator<<(ofstream& os, const Server& srvr)
{
    os << "Hello Server";
    return os;
}
 */