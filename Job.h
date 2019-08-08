//
// Created by Neria on 06/08/2019.
//
#ifndef NETWORKSIMULATOR_JOB_H
#define NETWORKSIMULATOR_JOB_H

#include "defs.h"

class Job{
public:
    Job(int time);
    ~Job();

    void FinishJob(int time);

    int GetCreation();
    int GetExiting();
    int GetWaiting();

private:
    int creation_time;
    int exiting_time;
    int waiting_time;
};

#endif //NETWORKSIMULATOR_JOB_H
