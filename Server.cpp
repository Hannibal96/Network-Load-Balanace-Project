//
// Created by Neria on 06/08/2019.
//

#include "Server.h"

int Server::total_serving_time = 0;
int Server::total_served_jobs = 0;

Server::Server(int id, double mu, int queue_max) : completed_jobs_(mu){
    this->id = id;
    this->mu = mu;
    this->queue_max = queue_max;
    jobs_in_queue=0;
    served_jobs=0;
    dismissed_jobs = 0;
    jobs_queue = queue<Job>();
    generator_.seed((unsigned)id);
}

void Server::AddJob(Job job) {
    if(jobs_in_queue >= queue_max){
        dismissed_jobs ++;
        assert(jobs_in_queue == jobs_queue.size() && "-W- Assert, Server::AddJob jobs_in_queue > queue_max" );
        return;
    }
    jobs_queue.push(job);
    jobs_in_queue ++ ;
}

int Server::GetQueuedJobs(){ return jobs_in_queue; }

void Server::FinishJob(int time) {
    int offered_service = completed_jobs_(generator_);
    int serving_jobs = std::min(offered_service, jobs_in_queue);
    assert(jobs_in_queue == jobs_queue.size() && "-W- Assert, Server::FinishJob queue size mismatch" );
    for(int i=0 ; i< serving_jobs;i++) {
        int job_service_time = jobs_queue.front().FinishJob(time);
        assert(job_service_time >= 0 && "-W- Assert, Server::FinishJob serving job time is negative" );
        total_serving_time += job_service_time;
        serving_time += job_service_time;
        //cout << jobs_queue.front() << endl;
        jobs_queue.pop();
        jobs_in_queue --;
        served_jobs ++;
        total_served_jobs++;
        assert( jobs_in_queue >= 0 && "-W- Assert, Server::FinishJob queue size smaller than zero" );
    }
}

string Server::toString() const {
    return "Server Id: "+::to_string(id)+
        "\n  -mu: "+::to_string(mu)+
        "\n  -jobs_in_queue: "+::to_string(jobs_in_queue)+
        "\n  -served_jobs: "+::to_string(served_jobs)+
        "\n  -serving_time: "+::to_string(serving_time)+
        "\n  -dismissed_jobs: "+::to_string(dismissed_jobs);
}

std::ostream& operator<<(std::ostream& os, const Server& srvr)
{
    os <<  srvr.toString();
    return os;
}
