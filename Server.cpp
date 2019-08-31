//
// Created by Neria on 06/08/2019.
//

#include "Server.h"
#include "utility"

unsigned long long Server::total_serving_time = 0;
unsigned long long Server::total_served_jobs = 0;

Server::Server(int id, double mu, int queue_max) : completed_jobs_(mu){
    this->id = id;
    this->mu = mu;
    this->queue_max = queue_max;
    serving_time = 0;
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
    assert(jobs_in_queue == jobs_queue.size() && "-W- Assert, Server::AddJob jobs_in_queue != queue_max" );
}

int Server::GetQueuedJobs(){ return jobs_in_queue; }

std::pair<int,bool > Server::FinishJob(int time) {
    int offered_service = completed_jobs_(generator_);
    int serving_jobs = std::min(offered_service, jobs_in_queue);

    std::pair<int,bool> finished_jobs = std::pair<int ,bool>();
    finished_jobs.first = serving_jobs;
    if(finished_jobs.first == jobs_in_queue) {
        finished_jobs.second = true;
    } else{
        finished_jobs.second = false;
    }

    assert(jobs_in_queue == jobs_queue.size() && "-W- Assert, Server::FinishJob queue size mismatch" );
    for(int i=0 ; i< serving_jobs;i++) {
        int job_service_time = jobs_queue.front().FinishJob(time);
        assert(job_service_time >= 0 && "-W- Assert, Server::FinishJob serving job time is negative" );

        total_serving_time += job_service_time;
        assert(total_serving_time >= 0 && "-W- Assert, Server::FinishJob -  total_serving_time OERFLOW" );

        serving_time += job_service_time;
        assert(serving_time >= 0 && "-W- Assert, Server::FinishJob - serving_time OERFLOW" );

        jobs_queue.pop();
        jobs_in_queue --;
        served_jobs ++;
        total_served_jobs++;
        assert( jobs_in_queue >= 0 && "-W- Assert, Server::FinishJob queue size smaller than zero" );
    }
    return finished_jobs;
}

double Server::GetMU(){    return mu;}

string Server::toString() const {
    return "Server Id: "+::to_string(id)+
        "\n  -mu: "+::to_string(mu)+
        "\n  -avg serving: "+::to_string(-1 + 1/mu)+
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
