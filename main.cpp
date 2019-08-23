//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"

using namespace std;

#define TIME 100000
#define PRINTTIME 100000
#define SERVERS_NUM 10
#define GAMMA 10
#define MU 0.5


/***************************************************
 **************** declerations  ********************
 ***************************************************/

void PrintServerStatus(Server** servers, int t);
void PrintEndSimulation(Server** servers, Dispatcher &dispatcher);

int main(int argc, char *argv[])
{
    /***************************************************
     ******************* parsing ***********************
     ***************************************************/

    int server_num = SERVERS_NUM;
    int POC = 3;
    string algo = argv[1];
    double server_rate[server_num];
    for (int n =0; n < server_num; n++){
        server_rate[n] = (double)(1+n)/(1+server_num);
    }
    double gamma = GAMMA;

    /***************************************************
     ****************** initialize *********************
     ***************************************************/
    Dispatcher *dispatcher;
    if(algo == "Random")          dispatcher = new Dispatcher(0, server_num, gamma);
    else if(algo == "RoundRobin") dispatcher = new RrDispatcher(0, server_num, gamma);
    else if(algo=="POC")          dispatcher = new PocDispatcher(0,server_num,gamma);
    else if(algo == "JSQ")        dispatcher = new JsqDispatcher(0, server_num, gamma);
    else if(algo == "JIQ")        dispatcher = new JiqDispatcher(0, server_num, gamma);
    else if (algo == "PI")        dispatcher = new PiDispatcher(0, server_num, gamma);
    else{
        cout << "-E- Worng algorithm " << endl;
        exit(1);
    }

    Server** servers = new Server*[server_num];
    for(int n=0; n<server_num; n++){
        servers[n] = new Server(n, server_rate[n]);
    }

    /***************************************************
     ****************** main loop **********************
     ***************************************************/

    for (int t = 0; t < TIME; t++) {
        int arrivals = dispatcher->get_arrivals();
        for(int a=0; a<arrivals ; a++){
            int destination = -1;
            if(algo == "POC"){
                destination = dynamic_cast<PocDispatcher *>(dispatcher)->get_destination(servers, POC);
            } else{
                destination = dispatcher->get_destination();
            }
            assert(destination != -1 && "-W- Assert, main loop : destination was not initialized" );
            Job job = Job(t);
            servers[destination]->AddJob(job);
        }

        for(int n=0;n<SERVERS_NUM;n++) {
            pair<int, bool> finished_jobs = servers[n]->FinishJob(t);
            if(algo == "JSQ")          dynamic_cast<JsqDispatcher*>(dispatcher)->update_server(n,finished_jobs.first);
            else if(algo == "JIQ")     dynamic_cast<JiqDispatcher *>(dispatcher)->update_server(n, finished_jobs.second);
            else if (algo == "PI")     dynamic_cast<PiDispatcher *>(dispatcher)->update_server(n, finished_jobs.second);
        }

        if( t % PRINTTIME == 0 && t > 0 ) {
            PrintServerStatus(servers, t);
        }
    }

    /***************************************************
     ***************** conclusions  ********************
     ***************************************************/

    PrintEndSimulation(servers, *dispatcher );

    /***************************************************
     ******************** Free  ************************
     ***************************************************/

    for(int n=0; n<SERVERS_NUM; n++){
        delete servers[n];
    }
    delete dispatcher;
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

