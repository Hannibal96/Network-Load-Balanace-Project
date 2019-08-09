//
// Created by Neria on 06/08/2019.
//
#ifndef NETWORKSIMULATOR_SERVER_H
#define NETWORKSIMULATOR_SERVER_H

#include "defs.h"
#include "Job.h"

class Server{

public:
Server(int id, double mu, int queue = 100 );   //std::numeric_limits<int>::max()
~Server() = default;
void AddJob(Job job);
void FinishJob(int time);
string toString() const ;

friend std::ostream& operator<<(std::ostream& os, const Server& s);

protected:
    int id, jobs_in_queue, served_jobs, queue_max;
    double mu;
    queue<Job> jobs_queue;
    // random numbers engine
    default_random_engine generator_;
    // generate the number of jobs completed at each ts.
    geometric_distribution<int> completed_jobs_;

};


#endif //NETWORKSIMULATOR_SERVER_H
