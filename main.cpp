//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"

#define TIME 10000
#define PRINTTIME 1000000
#define SERVERS_NUM 100
#define GAMMA 500
#define MU 0.5


/***************************************************
 **************** declerations  ********************
 ***************************************************/

void PrintServerStatus(Server** servers, int t);
void PrintEndSimulation(Server** servers, Dispatcher &dispatcher);

int main(int argc, char *argv[])
{
    /***************************************************
     ****************** initialize *********************
     ***************************************************/

    JsqDispatcher dispatcher = JsqDispatcher(0, SERVERS_NUM, GAMMA);

    Server** servers = new Server*[SERVERS_NUM];
    for(int n=0; n<SERVERS_NUM; n++){
        servers[n] = new Server(n, (double)(1+n)/(1+SERVERS_NUM) );//(double)(n+1)/(SERVERS_NUM+1));
    }

    /***************************************************
     ****************** main loop **********************
     ***************************************************/

    for (int t = 0; t < TIME; t++) {
        int arrivals = dispatcher.get_arrivals();
        for(int a=0; a<arrivals ; a++){
            int destination = dispatcher.get_destination();
            Job job = Job(t);
            servers[destination]->AddJob(job);
        }

        for(int n=0;n<SERVERS_NUM;n++) {
            int finished_jobs = servers[n]->FinishJob(t);
            dispatcher.update_server(n,finished_jobs);
        }

        if( t % PRINTTIME == 0 && t > 0 )
            PrintServerStatus( servers, t);
    }

    /***************************************************
     ***************** conclusions  ********************
     ***************************************************/

    PrintEndSimulation(servers, dispatcher );

}


/***************************************************
 ***************** functions  **********************
 ***************************************************/


void PrintServerStatus(Server** servers, int t){
    cout << "====================" << endl;
    cout << "Time: " << t << endl;
    for (int n = 0; n < SERVERS_NUM; n++) {
        cout << *servers[n] << endl;
    }
}
void PrintEndSimulation(Server** servers, Dispatcher &dispatcher){
    cout << "======================" << endl;
    cout << "=== Simulation End ===" << endl;
    cout << "======================" << endl;
    int total_queued_jobs = 0;
    for (int n = 0; n < SERVERS_NUM; n++) {
        cout << *servers[n] << endl;
        total_queued_jobs += servers[n]->GetQueuedJobs();
    }

    cout << dispatcher << endl;

    assert(total_queued_jobs+Server::total_served_jobs == Dispatcher::total_dispatched_jobs &&
              "-W- Assert, Served Jobs + Queued Jobs != Total Dispatched Jobs" );

    cout << "======================" << endl;
    cout << "Total serving time average: " << (double)Server::total_serving_time/Server::total_served_jobs << endl;
    cout << "======================" << endl;
}

