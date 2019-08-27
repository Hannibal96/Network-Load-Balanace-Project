//
// Created by Neria on 06/08/2019.
//
#ifndef NETWORKSIMULATOR_JOB_H
#define NETWORKSIMULATOR_JOB_H

#include "defs.h"

class Job{
public:
    Job(int time);
    ~Job() = default;

    int FinishJob(int time);
    int GetCreation(){ return creation_time; };
    int GetExiting(){ return  exiting_time;};
    int GetWaiting(){ return  waiting_time;};
    int GetSerial(){ return  job_number;};
    string toString() const ;
    int SetWaiting(int time);

    friend std::ostream& operator<<(std::ostream& os, const Job& s);

    static int number_of_jobs;

private:
    int creation_time;
    int exiting_time;
    int waiting_time;
    int job_number;
};

#endif //NETWORKSIMULATOR_JOB_H
