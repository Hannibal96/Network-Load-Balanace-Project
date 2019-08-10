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

    int GetCreation();
    int GetExiting();
    int GetWaiting();
    string toString() const ;

    friend std::ostream& operator<<(std::ostream& os, const Job& s);

    static int number_of_jobs;

private:
    int creation_time;
    int exiting_time;
    int waiting_time;
    int job_number;
};

#endif //NETWORKSIMULATOR_JOB_H
