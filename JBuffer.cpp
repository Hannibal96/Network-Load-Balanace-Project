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
    if(server.GetQueuedJobs() >= threshold)
        return true;
    return false;
}

bool JBuffer::CheckReturnToRoute(Server& server)
{
    if(server.GetQueuedJobs() <= low_threshold)
        return true;
    return false;
}

Job* JBuffer::SendJob(int time, int server_num)
{
    if(jobs_queue.empty())
        return nullptr;
    Job* re_routed_job = &jobs_queue.front();
    jobs_queue.pop();
    re_routed_job->SetWaiting(time);
    jobs_in_buffer --;
    assert(jobs_in_buffer >= 0  && "-W- Assert, JBuffer::SendJob jobs_in_buffer < 0" );
    assert(jobs_in_buffer == jobs_queue.size()  && "-W- Assert, JBuffer::SendJob jobs_in_buffer != jobs_queue.size()" );
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
