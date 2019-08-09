//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"

#define TIME 1000000
#define SERVERS_NUM 3
#define GAMMA 5
#define MU 0.3

void PrintServerStatus(Server** servers, int t);
void PrintEndSimulation(Server** servers, Dispatcher &dispatcher);

int main(int argc, char *argv[]) {

    Dispatcher dispatcher = Dispatcher(0, SERVERS_NUM, GAMMA);

    Server** servers = new Server*[SERVERS_NUM];
    for(int n=0; n<SERVERS_NUM; n++){
        servers[n] = new Server(n, (double)(n+1)/(SERVERS_NUM+1) );
    }

    for (int t = 0; t < TIME; t++) {
        int arrivals = dispatcher.get_arrivals();
        for(int a=0; a<arrivals ; a++){
            int destination = dispatcher.get_destination();
            Job job = Job(t);
            servers[destination]->AddJob(job);
        }

        for(int n=0;n<SERVERS_NUM;n++){
            servers[n]->FinishJob(t);
        }

        if(t%100000 == 0) {
            PrintServerStatus( servers, t);
        }
    }
    PrintEndSimulation(servers, dispatcher );

}




void PrintServerStatus(Server** servers, int t){
    cout << "====================" << endl;
    cout << "Time: " << t << endl;
    for (int n = 0; n < SERVERS_NUM; n++) {
        cout << *servers[n] << endl;
    }
}
void PrintEndSimulation(Server** servers, Dispatcher &dispatcher){
    cout << "====================" << endl;
    cout << "=== Simulation End ===" << endl;
    cout << "====================" << endl;
    for (int n = 0; n < SERVERS_NUM; n++) {
        cout << *servers[n] << endl;
    }
    cout << dispatcher << endl;
}
