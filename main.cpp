//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"

#define TIME 100000
#define SERVERS_NUM 3
#define GAMMA 10
#define MU 0.3

int main(int argc, char *argv[]) {

    Dispatcher disp = Dispatcher(0, SERVERS_NUM, GAMMA);

    Server** servers = new Server*[SERVERS_NUM];
    for(int n=0; n<SERVERS_NUM; n++){
        servers[n] = new Server(n, MU);
    }

    for (int t = 0; t < TIME; t++) {
        int arrivals = disp.get_arrivals();
        for(int a=0; a<arrivals ; a++){
            int dest = disp.get_destination();
            Job job = Job(t);
            servers[dest]->AddJob(job);
        }

        for(int n=0;n<SERVERS_NUM;n++){
            servers[n]->FinishJob(t);
        }

        //cout << "====================" << endl;
        //cout << "Time: " << t << endl;
        for(int n=0; n<SERVERS_NUM; n++){
            //cout << servers[n]->toString() << endl;
        }
    }
    for(int n=0; n<SERVERS_NUM; n++)
        cout << servers[n]->toString() << endl;
}