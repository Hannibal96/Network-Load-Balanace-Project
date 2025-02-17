//
// Created by Neria on 06/08/2019.
//
#ifndef NETWORKSIMULATOR_SERVER_H
#define NETWORKSIMULATOR_SERVER_H

#include "defs.h"
#include "Job.h"

class Server{

public:
    Server(int id, double mu, int queue_max = std::numeric_limits<int>::max() );   //std::numeric_limits<int>::max()
    ~Server() = default;
    void AddJob(Job job);
    std::pair<int,bool > FinishJob(int time);

    int GetQueuedJobs();
    double GetMU();

    string toString() const ;

    friend std::ostream& operator<<(std::ostream& os, const Server& s);
    static unsigned long long total_serving_time;
    static unsigned long long total_served_jobs;


protected:
    int id, jobs_in_queue, served_jobs, queue_max, dismissed_jobs;
    unsigned long long serving_time;
    double mu;
    queue<Job> jobs_queue;
    // random numbers engine
    default_random_engine generator_;
    // generate the number of jobs completed at each ts.
    geometric_distribution<int> completed_jobs_;

};


#endif //NETWORKSIMULATOR_SERVER_H
