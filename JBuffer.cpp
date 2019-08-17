//
// Created by Neria on 15/08/2019.
//

#include "JBuffer.h"

int JBuffer::total_buffered_jobs = 0;

void JBuffer::AddJob(Job job)
{
    if(jobs_in_buffer >= buffer_max){
        // TODO: check this part //dismissed_jobs ++;
        assert(jobs_in_buffer == jobs_queue.size() && "-W- Assert, Server::AddJob jobs_in_queue > queue_max" );
        return;
    }
    jobs_queue.push(job);
    jobs_in_buffer ++ ;
}


bool JBuffer::CheckReRoute(Server& server)
{
    //TODO: set condition
    if(server.GetQueuedJobs() > 100)
        return true;
    return false;
}

Job JBuffer::SendJob(int time, int server_num)
{
    Job re_routed_job = jobs_queue.front();
    re_routed_job.SetWaiting(time);
    routing_map[server_num] ++ ;
    return re_routed_job;
}


string JBuffer::toString() const
{
    string buffer_print = "Buffer Id: "+::to_string(id)+
                          "\n  -jobs_in_buffer: "+::to_string(jobs_in_buffer)+"\n";
    for (auto const& x : routing_map)
        buffer_print += "   "+::to_string(x.first) +": "+::std::to_string(x.second)+"\n";
    return buffer_print;
}

std::ostream& operator<<(std::ostream& os, const JBuffer& buffer)
{
    os <<  buffer.toString();
    return os;
}
