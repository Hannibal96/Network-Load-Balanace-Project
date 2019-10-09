//
// Created by Neria on 15/08/2019.
//

#include "JBuffer.h"

unsigned long long JBuffer::total_buffered_jobs = 0;
unsigned long long JBuffer::total_waiting = 0;

void JBuffer::AddJob(Job job)
{
    if(jobs_in_buffer >= buffer_max){
        // TODO: check this part //dismissed_jobs ++;
        assert(jobs_in_buffer == jobs_queue.size() && "-W- Assert, Server::AddJob jobs_in_queue > queue_max" );
        return;
    }
    jobs_queue.push(job);
    jobs_in_buffer ++ ;
    total_buffered_jobs ++;
    if(jobs_in_buffer > maximal_queue)
        maximal_queue = jobs_in_buffer;
}


bool JBuffer::CheckReRoute(Server& server)
{
    if(server.GetQueuedJobs() >= high_threshold)
        return true;
    return false;
}

bool JBuffer::CheckReturnToRoute(Server& server)
{
    if(server.GetQueuedJobs() <= low_threshold)
        return true;
    return false;
}

Job JBuffer::SendJob(int time, int server_num)
{
    if(jobs_queue.empty()) {
        assert("-W- Assert, JBuffer::SendJob try to send Job from buffer with buffer queue empty");
    }
    Job re_routed_job = jobs_queue.front();
    jobs_queue.pop();
    total_waiting += re_routed_job.SetWaiting(time);
    assert(total_waiting >= 0 && "-W- Assert, JBuffer::SendJob total_waiting_time OVERFLOW" );

    jobs_in_buffer --;
    assert(jobs_in_buffer >= 0  && "-W- Assert, JBuffer::SendJob jobs_in_buffer < 0" );
    assert(jobs_in_buffer == jobs_queue.size()  && "-W- Assert, JBuffer::SendJob jobs_in_buffer != jobs_queue.size()" );
    buffer_routing_map[server_num] ++ ;
    return re_routed_job;
}

int JBuffer::GetQueuedJobs(){    return jobs_in_buffer;  }

int JBuffer::GetMaximalQueue(){    return maximal_queue;  }

string JBuffer::toString() const
{
    string buffer_print = "***************************************************\n"
                          "******************** Buffers **********************\n"
                          "***************************************************\n";
    buffer_print += "Buffer Id: "+::to_string(id)+
                          "\n  -jobs_in_buffer: "+::to_string(jobs_in_buffer)+"\n";
    for (auto const& x : buffer_routing_map)
        buffer_print += "   "+::to_string(x.first) +": "+::std::to_string(x.second)+"\n";
    buffer_print += "  -total buffered jobs: " + ::to_string(total_buffered_jobs);
    buffer_print += "\n  -total waiting time: " + ::to_string(total_waiting);
    buffer_print += "\n  -average waiting time: " + ::to_string(((double)total_waiting)/total_buffered_jobs);
    return buffer_print;
}

std::ostream& operator<<(std::ostream& os, const JBuffer& buffer)
{
    os <<  buffer.toString();
    return os;
}
