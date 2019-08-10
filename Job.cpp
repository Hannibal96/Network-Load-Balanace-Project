//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Job.h"

int Job::number_of_jobs = 0;

Job::Job(int time)
{
    creation_time = time;
    waiting_time = 0;
    exiting_time = 0;
    number_of_jobs++;
    job_number = number_of_jobs;
}

int Job::GetCreation() {   return creation_time; }
int Job::GetExiting() {    return exiting_time;  }
int Job::GetWaiting() {    return waiting_time;  }

int Job::FinishJob(int time)
{
    exiting_time = time;
    return exiting_time - creation_time;
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