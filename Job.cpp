//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Job.h"

int Job::number_of_jobs = 0;
map<int,pair<int,int>> Job::jobs_completion_maps = map<int,pair<int,int>>();

Job::Job(int time)
{
    creation_time = time;
    waiting_time = 0;
    exiting_time = -1;
    served_time = -1;
    number_of_jobs++;
    job_number = number_of_jobs;
}

int Job::SetWaiting(int time) {
    waiting_time = time - creation_time;
    assert(waiting_time >= 0 && "-W- Assert, Job::SetWaiting job waiting time is negative" );
    return waiting_time;
};

int Job::FinishJob(int time)
{
    exiting_time = time;
    served_time = exiting_time - creation_time;
    //jobs_completion_maps[job_number] = pair<int,int>(waiting_time,served_time);
    assert(waiting_time >= 0 && "-W- Assert, Job::SetWaiting job waiting time is negative" );
    assert(served_time >= 0 && "-W- Assert, Job::SetWaiting job serving time is negative" );
    assert(served_time >= waiting_time && "-W- Assert, Job::SetWaiting job waiting time is bigger than serving" );
    return served_time;
}

string Job::toString() const
{
    return "Job Number: "+::to_string(job_number)+
        "\n -creation_time: "+::to_string(creation_time)+
        "\n -waiting_time: "+::to_string(waiting_time)+
        "\n -exiting_time: "+::to_string(exiting_time)+
        "\n -serving_time: "+::to_string(exiting_time - creation_time);
}

std::ostream& operator<<(std::ostream& os, const Job& job)
{
    os <<  job.toString();
    return os;
}