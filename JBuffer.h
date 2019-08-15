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
    JBuffer(int id, int servers_num, int buffer_max = std::numeric_limits<int>::max() )
    {
        this->id = id;
        this->jobs_in_buffer = 0;
        jobs_queue = queue<Job>();
        for(int i = 0; i < servers_num ; i++)
            routing_map[i] = 0;
        this->buffer_max = buffer_max;
    };

    ~JBuffer() = default;
    void AddJob(Job job);
    bool CheckReRoute(Server& server);
    Job SendJob(int time, int server_num);
    string toString() const ;
    friend std::ostream& operator<<(std::ostream& os, const JBuffer& s);
    static int total_buffered_jobs;

protected:
    int id, jobs_in_buffer, buffer_max;
    queue<Job> jobs_queue;
    map<int,int> routing_map;
};

#endif //NETWORKSIMULATOR_JBUFFER_H
