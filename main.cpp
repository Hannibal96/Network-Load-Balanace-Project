//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"
#include "JBuffer.h"

using namespace std;

/***************************************************
 **************** declerations  ********************
 ***************************************************/

void PrintServerStatus(Server** servers, int server_num, int t);
void PrintEndSimulation(Server** servers, int server_num, Dispatcher &dispatcher, JBuffer &buffer);
void PrintInfo(int time, int servers_num, double gamma, string algo, int HighTH, int LowTH);
void ParseArguments(int argc, char *argv[], map<string, double >& numeric_data, map<string, string>& verbal_data);
double InitServersAndGetGamma(string servers_verb, double *server_rate, int servers_num, double load);

int main(int argc, char *argv[])
{

    /***************************************************
     ******************* parsing ***********************
     ***************************************************/

    map<string, double > numeric_data;
    map<string, string> verbal_data;
    ParseArguments(argc, argv, numeric_data, verbal_data);
    int server_num = (int)numeric_data["Servers"], time = (int)numeric_data["Time"];

    int print_time = std::numeric_limits<int>::max();
    if ( numeric_data.find("PrintTime") != numeric_data.end() )
        print_time = (int)numeric_data["PrintTime"];

    double server_rate[ server_num ];
    double gamma = InitServersAndGetGamma(verbal_data["Servers"], server_rate, server_num, numeric_data["Load"]);

    string algo = verbal_data["Algorithm"];
    int POC = -1;
    if ( algo == "POC" ) {
        POC = (int) numeric_data["POC"];
        if(!POC){
            cout << "-E- POC algo was chosen but no POC parameter was entered" << endl;
            exit(1);
        }
        if(POC >= server_num){
            cout << "-E- POC parameter bigger or equal to number of servers, use smaller POC or JSQ" << endl;
            exit(1);
        }
    }

    int high_threshold = (int)numeric_data["HighTH"];
    int low_threshold = (int)numeric_data["LowTH"];

    PrintInfo(time, server_num, gamma, algo, high_threshold, low_threshold);

    /***************************************************
     ***************** initializing ********************
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

    JBuffer buffer;
    bool buffer_exist = not (high_threshold == low_threshold && high_threshold == 0);
    if(buffer_exist) {
        buffer = JBuffer(1001, server_num, high_threshold, low_threshold);
    }
    else {
        buffer = JBuffer(1001, server_num);                             // deafulting buffer, not affecting simulator
    }

    /***************************************************
     ****************** main loop **********************
     ***************************************************/

    for (int t = 0; t < time; t++) {
        int arrivals = dispatcher->get_arrivals();                        // get arrivals
        for(int a=0; a<arrivals ; a++){                                   // send to destinations
            Job job = Job(t);
            int destination = -1;
            if(algo == "POC"){
                assert(POC != -1 && "-W- Assert, main loop : POC was not initialized" );
                destination = dynamic_cast<PocDispatcher *>(dispatcher)->get_destination(servers, POC);
            } else{
                destination = dispatcher->get_destination();
            }
            assert(destination != -1 && "-W- Assert, main loop : destination was not initialized" );

            bool reRoute = buffer.CheckReRoute( *servers[destination] );
            if( reRoute ){                                                     // buffer related routing
                assert( buffer_exist && "-W- Assert, ReRoute : buffer dosen't exist but enter buffer condition" );
                dispatcher->update_routing_table(-1);
                buffer.AddJob(job);
            }
            else{                                                             // regular routing
                bool returnToRoute = buffer.CheckReturnToRoute( *servers[destination] );
                if( returnToRoute ){
                    assert( buffer_exist && "-W- Assert, returnToRoute : buffer dosen't exist but enter buffer condition" );
                    while ( buffer.GetQueuedJobs() ) {
                        Job returned_job = buffer.SendJob(time, destination);
                        servers[destination]->AddJob(returned_job);
                        if(algo == "RoundRobin") {
                            dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                        }
                        else {
                            dispatcher->update_server_route(destination);
                        }
                    }
                }
                dispatcher->update_routing_table(destination);
                servers[destination]->AddJob(job);
                if(algo == "RoundRobin") {
                    dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                }
                else {
                    dispatcher->update_server_route(destination);
                }
            }
        }

        for(int n=0; n<server_num; n++) {                                   // serve jobs and update dispatcher information
            pair<int, bool> finished_jobs = servers[n]->FinishJob(t);
            if(algo == "JSQ") {
                dynamic_cast<JsqDispatcher *>(dispatcher)->update_server_finish(n, finished_jobs.first);
            }
            else if(algo == "JIQ" || algo == "PI") {
                dynamic_cast<JiqDispatcher *>(dispatcher)->update_server_finish(n, finished_jobs);
            }
        }

        if( t % print_time == 0 && t > 0 ) {
            PrintServerStatus(servers, server_num, t);
            cout << buffer << endl;
        }
    }

    /***************************************************
     ***************** conclusions  ********************
     ***************************************************/

    PrintEndSimulation(servers, server_num, *dispatcher , buffer);

    /***************************************************
     ******************** Free  ************************
     ***************************************************/

    for(int n=0; n<server_num; n++){
        delete servers[n];
    }
    delete dispatcher;
}


/***************************************************
 ***************** functions  **********************
 ***************************************************/

void PrintServerStatus(Server** servers, int server_num, int t)
{
    cout << "********************" << endl;
    cout << "**** Time:" << t << " *******" << endl;
    cout << "********************" << endl;
    cout << "Time: " << t << endl;
    for (int n = 0; n < server_num; n++) {
        cout << *servers[n] << endl;
    }
}

void PrintEndSimulation(Server** servers, int server_num , Dispatcher &dispatcher, JBuffer& buffer)
{
    cout << "***************************************************" << endl
         << "*************** Simulation End ********************" << endl
         << "***************************************************" << endl;
    int total_queued_jobs = 0;
    for (int n = 0; n < server_num; n++) {
        cout << *servers[n] << endl;
        total_queued_jobs += servers[n]->GetQueuedJobs();
    }
    cout << dispatcher << endl;
    cout << buffer << endl;
    assert(total_queued_jobs+Server::total_served_jobs+buffer.GetQueuedJobs() == Dispatcher::total_dispatched_jobs &&
           "-W- Assert, Served Jobs + Queued Jobs != Total Dispatched Jobs" );

    cout << "======================" << endl;
    cout << "Total serving time average: " << (double)Server::total_serving_time/Server::total_served_jobs << endl;
    cout << "======================" << endl;
}

void PrintInfo(int time, int servers_num, double gamma, string algo, int HighTH, int LowTH)
{
    cout << "***************************************************" << endl
         << "*********** Stating simulation with ***************" << endl
         << "***************************************************" << endl;
    cout << "--- " << servers_num << " servers" << endl;
    cout << "--- incoming rate of " << gamma << endl;
    cout << "--- under load balancing of " <<  algo << endl;
    if(HighTH == LowTH && HighTH == 0)
        cout << "--- no buffer in usage" << endl;
    else
        cout << "--- with buffer configurations of: " << HighTH << " high threshold, and " << LowTH << " low threshold" << endl;
    cout << "--- for " << time << " time units" << endl;
}

void ParseArguments(int argc, char *argv[], map<string, double >& numeric_data, map<string, string>& verbal_data)
{
    for(int i=1; i<argc; i++){
        string converted_arg = argv[i];
        if(converted_arg == "--Time") {
            numeric_data["Time"] = atoi(argv[++i]);
        }
        else if(converted_arg == "--PrintTime") {
            numeric_data["PrintTime"] = atoi(argv[++i]);
        }
        else if(converted_arg == "--Servers") {
            numeric_data["Servers"] = atoi(argv[++i]);
            verbal_data["Servers"] = argv[++i];
        }
        else if(converted_arg == "--Dispatchers") {         // Currently testing only one, to expand to multiple need
            numeric_data["Dispatchers"] = atoi(argv[++i]);  // to make small adjustment from the server parsing
            verbal_data["Algorithm"] = argv[++i];
            if( verbal_data["Algorithm"] == "POC" )
                numeric_data["POC"] = atoi(argv[++i]);
        }
        else if(converted_arg == "--Load") {
            numeric_data["Load"] = stod(argv[++i]);
        }
        else if(converted_arg == "--Buffer") {              // Likewise
            numeric_data["Buffer"] = atoi(argv[++i]);
            //verbal_data["Buffer"] = argv[++i];
            converted_arg = argv[++i];
            if( converted_arg == "--LowTH" ){
                numeric_data["LowTH"] = atoi(argv[++i]);
            }
            else {
                cout << "-E- Wrong Buffer argument and/or order" << endl;
                exit(1);
            }
            converted_arg = argv[++i];
            if(converted_arg == "--HighTH"){
                numeric_data["HighTH"] = atoi(argv[++i]);
            }
            else {
                cout << "-E- Wrong Buffer argument and/or order" << endl;
                exit(1);
            }
        }
        else if(converted_arg == "--Help"){
            cout << "-Help message:" << endl;
            cout << "--Example: \n\t--Servers 10 5,1;5,10; --Dispatchers 1 JSQ --Time 100000 --Load 0.8 --Buffer 1 --LowTH 10 --HighTH 1000" << endl;
            exit(0);
        }
        else {
            cout << "-E- Wrong argument inserted" << endl;
            exit(1);
        }
    }
}

double InitServersAndGetGamma(string servers_verb, double *server_rate, int servers_num, double load)
{
    double total_serving_rates = 0.0;
    int offset = 0 ;
    string delimiter = ";", inner_delimiter = ",";
    size_t pos = 0;
    size_t sub_pos = 0;
    std::string token_dual;
    while ((pos = servers_verb.find(delimiter)) != std::string::npos) {
        token_dual = servers_verb.substr(0, pos);
        sub_pos = token_dual.find(inner_delimiter);
        int number = stoi(token_dual.substr(0, sub_pos));
        token_dual.erase(0, sub_pos + delimiter.length());
        sub_pos = token_dual.find(inner_delimiter);
        double value = stod(token_dual.substr(0, sub_pos));
        for(int i=0;i<number;i++) {
            server_rate[offset + i] = 1/(1-value+(value/0.5));
            total_serving_rates += (1/server_rate[offset + i]) - 1;
        }
        offset += number;
        servers_verb.erase(0, pos + delimiter.length());
    }
    if(offset != servers_num) {
        cout << "-E- number of servers dosen't consist with server rate info" << endl;
        exit(1);
    }
    return load * total_serving_rates;
}
