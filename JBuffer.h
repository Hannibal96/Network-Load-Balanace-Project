//
// Created by Neria on 15/08/2019.
//

#ifndef NETWORKSIMULATOR_JBUFFER_H
#define NETWORKSIMULATOR_JBUFFER_H

#include "defs.h"
#include "Job.h"
#include "Server.h"

class JBuffer {

public:

    JBuffer() = default;
    JBuffer(int id, int servers_num,int high_threshold = std::numeric_limits<int>::max() ,int low_threshold = -1,
            int buffer_max = std::numeric_limits<int>::max(), const string& policy = "Victim")
    {
        this->id = id;
        this->jobs_in_buffer = 0;
        jobs_queue = queue<Job>();
        for(int i = 0; i < servers_num ; i++)
            buffer_routing_map[i] = 0;
        this->buffer_max = buffer_max;
        this->high_threshold = high_threshold;
        this->low_threshold = low_threshold;
        this->maximal_queue = 0;
        this->policy = policy;
    };

    ~JBuffer() = default;
    void AddJob(Job job);
    bool CheckReRoute(Server& server);
    bool CheckReturnToRoute(Server& server);

    int GetQueuedJobs();
    int GetMaximalQueue();

    Job SendJob(int time, int server_num);
    string toString() const ;
    friend std::ostream& operator<<(std::ostream& os, const JBuffer& s);
    static unsigned long long total_buffered_jobs;
    static unsigned long long total_waiting;


protected:
    int id, jobs_in_buffer, buffer_max, high_threshold, low_threshold, maximal_queue;
    queue<Job> jobs_queue;
    map<int,int> buffer_routing_map;
    string policy;
};

#endif //NETWORKSIMULATOR_JBUFFER_H
