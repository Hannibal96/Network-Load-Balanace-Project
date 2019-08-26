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
void ParseArguments(int argc, char *argv[], map<string, double >& numeric_data, map<string, string>& verbal_data); //ולא בפייתון cpp איך לכתוב פרסר לארגומנטים לא כמו מפגר ואפילו ב 
double InitServersAndGetGamma(string servers_verb, double *server_rate, double load);

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
    double gamma = InitServersAndGetGamma(verbal_data["Servers"], server_rate, numeric_data["Load"]);

    string algo = verbal_data["Algorithm"];
    int POC = -1;
    if ( algo == "POC" )
        POC = (int)numeric_data["POC"];


    int threshold = 100;
    int low_threshold = 10;

    /***************************************************
     ****************** initialize *********************
     ***************************************************/
    Dispatcher *dispatcher;
    if(algo == "Random")          dispatcher = new Dispatcher(0, server_num, gamma, threshold);
    else if(algo == "RoundRobin") dispatcher = new RrDispatcher(0, server_num, gamma, threshold);
    else if(algo=="POC")          dispatcher = new PocDispatcher(0,server_num,gamma, threshold);
    else if(algo == "JSQ")        dispatcher = new JsqDispatcher(0, server_num, gamma, threshold);
    else if(algo == "JIQ")        dispatcher = new JiqDispatcher(0, server_num, gamma, threshold);
    else if (algo == "PI")        dispatcher = new PiDispatcher(0, server_num, gamma, threshold);
    else{
        cout << "-E- Worng algorithm " << endl;
        exit(1);
    }

    Server** servers = new Server*[server_num];
    for(int n=0; n<server_num; n++){
        servers[n] = new Server(n, server_rate[n]);
    }

    JBuffer buffer = JBuffer(1001, server_num, threshold, low_threshold);

    /***************************************************
     ****************** main loop **********************
     ***************************************************/

    for (int t = 0; t < time; t++) {
        int arrivals = dispatcher->get_arrivals();
        for(int a=0; a<arrivals ; a++){
            int destination = -1;
            if(algo == "POC"){
                assert(POC != -1 && "-W- Assert, main loop : POC was not initialized" );
                destination = dynamic_cast<PocDispatcher *>(dispatcher)->get_destination(servers, POC);
            } else{
                destination = dispatcher->get_destination(servers);
            }
            assert(destination != -1 && "-W- Assert, main loop : destination was not initialized" );
            Job job = Job(t);

            servers[destination]->AddJob(job);
        }

        for(int n=0; n<server_num; n++) {
            pair<int, bool> finished_jobs = servers[n]->FinishJob(t);
            if(algo == "JSQ")
                dynamic_cast<JsqDispatcher*>(dispatcher)->update_server(n,finished_jobs.first);
            else if(algo == "JIQ")
                dynamic_cast<JiqDispatcher *>(dispatcher)->update_server(n, finished_jobs.second);
            else if (algo == "PI")
                dynamic_cast<PiDispatcher *>(dispatcher)->update_server(n, finished_jobs.second);
        }

        if( t % print_time == 0 && t > 0 ) {
            PrintServerStatus(servers, server_num, t);
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
    cout << "====================" << endl;
    cout << "Time: " << t << endl;
    for (int n = 0; n < server_num; n++) {
        cout << *servers[n] << endl;
    }
}

void PrintEndSimulation(Server** servers, int server_num , Dispatcher &dispatcher, JBuffer& buffer)
{
    cout << "======================" << endl;
    cout << "=== Simulation End ===" << endl;
    cout << "======================" << endl;
    int total_queued_jobs = 0;
    for (int n = 0; n < server_num; n++) {
        cout << *servers[n] << endl;
        total_queued_jobs += servers[n]->GetQueuedJobs();
    }
    cout << dispatcher << endl;
    cout << buffer << endl;
    assert(total_queued_jobs+Server::total_served_jobs == Dispatcher::total_dispatched_jobs &&
              "-W- Assert, Served Jobs + Queued Jobs != Total Dispatched Jobs" );

    cout << "======================" << endl;
    cout << "Total serving time average: " << (double)Server::total_serving_time/Server::total_served_jobs << endl;
    cout << "======================" << endl;
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
            verbal_data["Buffer"] = argv[++i];
        }
        else if(converted_arg == "--Help"){
            cout << "-Help message:" << endl;
        }
    }
}

double InitServersAndGetGamma(string servers_verb, double *server_rate, double load)
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
        int value = stoi(token_dual.substr(0, sub_pos));
        for(int i=0;i<number;i++) {
            server_rate[offset + i] = 0.5 / ((double) value);
            total_serving_rates += 2 * ((double) value);
        }
        offset += number;
        servers_verb.erase(0, pos + delimiter.length());
    }
    return load * total_serving_rates;
}
