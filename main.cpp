//
// Created by Neria on 06/08/2019.
//
#include "defs.h"
#include "Server.h"
#include "Dispacher.h"
#include "Job.h"
#include "JBuffer.h"
#include <iomanip>

#define VEC_SIZE 1000000

using namespace std;

/***************************************************
 **************** declerations  ********************
 ***************************************************/

void PrintServerStatus(Server** servers, int server_num, int t);
void PrintEndSimulation(Server** servers, int server_num, Dispatcher &dispatcher, JBuffer &buffer);
void PrintInfo(int sim_time, int servers_num, double gamma, string algo, int HighTH, int LowTH);
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
    int server_num = (int)numeric_data["Servers"], sim_time = (int)numeric_data["Time"];
    int sample_rate = sim_time/(VEC_SIZE);
    if(sample_rate == 0)
        sample_rate = 1;

    int print_time = std::numeric_limits<int>::max();
    if ( numeric_data.find("PrintTime") != numeric_data.end() )
        print_time = (int)numeric_data["PrintTime"];

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

    /***************************************************
    **************** define output *********************
    ***************************************************/

    string name = "./../results/";
    name += "Servers-"+verbal_data["Servers"]+"-"+verbal_data["Algorithm"]+"-"+to_string((int)numeric_data["POC"])+
            "-Load-"+(verbal_data["Load"])+
            "-Buffer-Low="+to_string(low_threshold)+"-High="+to_string(high_threshold)+
            "-Policy="+verbal_data["Policy"]+"-"+to_string((int)numeric_data["Policy"])+
            "-Time-"+to_string(sim_time);

    ofstream sim_print;
    sim_print.open(name+"_print.log");

    ofstream sim_val;
    sim_val.open (name+"_values.log");

    //std::cout.rdbuf(sim_print.rdbuf()); //redirect std::cout to out.txt!

    if (not (sim_val.is_open() && sim_print.is_open())) {
        cout << "-E- output was not correctly set" << endl;
        exit(1);
    }

    vector<double> load;
    double min_load = numeric_data["Load_Min"], max_load = numeric_data["Load_Max"], step_load = numeric_data["Load_Step"];
    for( double i=min_load; i<=max_load+step_load/2; i = i+step_load )
        load.push_back(i);

    for(int i=0; i<load.size() ;i++) {

        double curr_load = load[i];

        /***************************************************
         ***************** initializing ********************
         ***************************************************/

        double server_rate[server_num];
        double gamma = InitServersAndGetGamma(verbal_data["Servers"], server_rate, server_num, curr_load);

        Dispatcher *dispatcher;
        if (algo == "Random") dispatcher = new Dispatcher(0, server_num, gamma);
        else if (algo == "RoundRobin") dispatcher = new RrDispatcher(0, server_num, gamma);
        else if (algo == "POC") dispatcher = new PocDispatcher(0, server_num, gamma);
        else if (algo == "JSQ") dispatcher = new JsqDispatcher(0, server_num, gamma);
        else if (algo == "JIQ") dispatcher = new JiqDispatcher(0, server_num, gamma);
        else if (algo == "PI") dispatcher = new PiDispatcher(0, server_num, gamma);
        else if (algo == "Optimal") dispatcher = new OptDispatcher(0, server_num, gamma);
        else {
            cout << "-E- Worng algorithm " << endl;
            exit(1);
        }

        Server **servers = new Server *[server_num];
        for (int n = 0; n < server_num; n++) {
            servers[n] = new Server(n, server_rate[n]);
        }

        JBuffer buffer;
        bool buffer_exist = not(high_threshold == low_threshold && high_threshold == 0);
        if (buffer_exist) {
            buffer = JBuffer(1001, server_num, high_threshold, low_threshold, verbal_data["Policy"]);
        } else {
            buffer = JBuffer(1001,
                             server_num);                             // deafulting buffer, not affecting simulator
        }

        unsigned long long total_queued_jobs = 0;
        unsigned long long total_buffered_jobs_overtime = 0;


        /***************************************************
         ****************** main loop **********************
         ***************************************************/

        PrintInfo(sim_time, server_num, gamma, algo, high_threshold, low_threshold);

        for (int curr_time = 1; curr_time <= sim_time; curr_time++) {
            int arrivals = dispatcher->get_arrivals();                        // get arrivals
            for (int a = 0; a < arrivals; a++) {                                   // send to destinations
                Job job = Job(curr_time);
                int destination = -1;
                if (algo == "POC") {
                    assert(POC != -1 && "-W- Assert, main loop : POC was not initialized");
                    destination = dynamic_cast<PocDispatcher *>(dispatcher)->get_destination(servers, POC);
                } else if (algo == "Optimal") {
                    destination = dynamic_cast<OptDispatcher *>(dispatcher)->get_destination(servers);
                } else {
                    destination = dispatcher->get_destination();
                }
                assert(destination != -1 && "-W- Assert, main loop : destination was not initialized");

                bool reRoute = buffer.CheckReRoute(*servers[destination]);
                if (reRoute) {                                                     // route to buffer
                    assert(buffer_exist && "-W- Assert, ReRoute : buffer dosen't exist but enter buffer condition");
                    dispatcher->update_routing_table(-1);
                    buffer.AddJob(job);
                }

                else {                                                             // regular routing + reroute from buffer
                    // LowTH
                    bool returnToRoute = buffer.CheckReturnToRoute(*servers[destination]);
                    //LowTH
                    if (returnToRoute) {
                        assert(buffer_exist &&
                               "-W- Assert, returnToRoute : buffer dosen't exist but enter buffer condition");
                        if(buffer.GetPolicy() == "--Victim") {
                            while (buffer.GetQueuedJobs()) {
                                Job returned_job = buffer.SendJob(curr_time, destination);
                                servers[destination]->AddJob(returned_job);
                                if (algo == "RoundRobin") {
                                    dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                                } else {
                                    dispatcher->update_server_route(destination);
                                }
                            }
                        }

                        else if (buffer.GetPolicy() == "--Jester"){
                            int jesters = (int)numeric_data["Policy"];
                            while (buffer.GetQueuedJobs() && jesters > 0 ) {
                                Job returned_job = buffer.SendJob(curr_time, destination);
                                servers[destination]->AddJob(returned_job);
                                if (algo == "RoundRobin") {
                                    dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                                } else {
                                    dispatcher->update_server_route(destination);
                                }
                                jesters --;
                            }
                        }
                        else if(buffer.GetPolicy() == "--WaterBoarding"){
                            int water_boarding_th = (int)numeric_data["Policy"];
                            while (buffer.GetQueuedJobs() && servers[destination]->GetQueuedJobs() < water_boarding_th ) {
                                Job returned_job = buffer.SendJob(curr_time, destination);
                                servers[destination]->AddJob(returned_job);
                                if (algo == "RoundRobin") {
                                    dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                                } else {
                                    dispatcher->update_server_route(destination);
                                }
                            }
                        }
                        else{
                            assert(false && "-W- Assert, ReturnToRoute : buffer's dosen't existing policy");
                        }
                    }

                    dispatcher->update_routing_table(destination);
                    servers[destination]->AddJob(job);
                    if (algo == "RoundRobin") {
                        dynamic_cast<RrDispatcher *>(dispatcher)->update_server_route();
                    } else {
                        dispatcher->update_server_route(destination);
                    }
                }
            }

            for (int n = 0; n < server_num; n++) {                                   // serve jobs and update dispatcher information
                pair<int, bool> finished_jobs = servers[n]->FinishJob(curr_time);
                if (algo == "JSQ") {
                    dynamic_cast<JsqDispatcher *>(dispatcher)->update_server_finish(n, finished_jobs.first);
                } else if (algo == "JIQ" || algo == "PI") {
                    dynamic_cast<JiqDispatcher *>(dispatcher)->update_server_finish(n, finished_jobs);
                }
            }

            /***************************************************
             **************** sample states  *******************
             ***************************************************/
            for (int n = 0; n < server_num; n++) {
                total_queued_jobs += servers[n]->GetQueuedJobs();
                total_buffered_jobs_overtime += buffer.GetQueuedJobs();
                //total_buffered_jobs_overtime += JBuffer::total_buffered_jobs;
            }
            if (curr_time % sample_rate == 0 && load.size() == 1) {
                sim_val <<
                        " " << curr_time <<
                        " " << (double)Server::total_serving_time / (Server::total_served_jobs + !(Server::total_served_jobs)) <<
                        " " << (double)(total_queued_jobs + total_buffered_jobs_overtime) / curr_time <<
                        " " << buffer.GetQueuedJobs() << endl;
            }
            if (curr_time % print_time == 0) {
                PrintServerStatus(servers, server_num, curr_time);
                cout << buffer << endl;
            }
        }

        if ( load.size() > 1 ) {
            sim_val <<
                    " " << curr_load <<
                    " " << (double)Server::total_serving_time / (Server::total_served_jobs + !(Server::total_served_jobs)) <<
                    " " << (double)(total_queued_jobs + total_buffered_jobs_overtime) / sim_time <<
                    " " << buffer.GetMaximalQueue() << endl;
        }

        /***************************************************
         ***************** conclusions  ********************
         ***************************************************/

        PrintEndSimulation(servers, server_num, *dispatcher, buffer);

        /***************************************************
         ******************** Free  ************************
         ***************************************************/

        for (int n = 0; n < server_num; n++) {
            delete servers[n];
        }
        delete dispatcher;

        if ( load.size() > 1){
            Server::total_serving_time = 0;
            Server::total_served_jobs = 0;
            JBuffer::total_buffered_jobs = 0;
            JBuffer::total_waiting = 0;
            Job::number_of_jobs = 0;
            Job::jobs_completion_maps = map<int,pair<int,int>>();
            Dispatcher::total_dispatched_jobs = 0;
        }

    }


    sim_print.close();
    sim_val.close();

    return 0;
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

void PrintInfo(int sim_time, int servers_num, double gamma, string algo, int HighTH, int LowTH)
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
    cout << "--- for " << sim_time << " time units" << endl;
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
            verbal_data["Load"] = argv[++i];

            string a = verbal_data["Load"];
            a.erase(0,1);
            size_t pos = a.find(",");
            string min = a.substr(0, pos);
            a.erase(0, pos + 1);

            pos = a.find(",");
            string max = a.substr(0, pos);
            a.erase(0, pos + 1);

            pos = a.find("}");
            string step = a.substr(0, pos);
            a.erase(0, pos + 1);

            numeric_data["Load_Min"] = stod(min);
            numeric_data["Load_Max"] = stod(max);
            numeric_data["Load_Step"] = stod(step);
        }
        else if(converted_arg == "--Buffer") {              // Likewise
            numeric_data["Buffers"] = atoi(argv[++i]);
            //verbal_data["Buffer"] = argv[++i];
            converted_arg = argv[++i];

            if( converted_arg == "--LowTH" ){
                numeric_data["LowTH"] = atoi(argv[++i]);
            }
            else {
                cout << "-E- Wrong LowTH Buffer argument and/or order" << endl;
                exit(1);
            }
            converted_arg = argv[++i];
            if(converted_arg == "--HighTH"){
                numeric_data["HighTH"] = atoi(argv[++i]);
            }
            else {
                cout << "-E- Wrong HighTH Buffer argument and/or order" << endl;
                exit(1);
            }
            converted_arg = argv[++i];
            if(converted_arg == "--Jester" || converted_arg == "--WaterBoarding" ||
                converted_arg == "--Victim"){
                verbal_data["Policy"] = converted_arg;
                if(converted_arg == "--WaterBoarding" || converted_arg == "--Jester"){
                    numeric_data["Policy"] = atoi(argv[++i]);
                }
            }
            else if(numeric_data["HighTH"] != 0) {
                cout << "-E- Wrong Policy Buffer argument and/or order" << endl;
                exit(1);
            } else{
                verbal_data["Policy"] = "Deafault";
            }

        }
        else if(converted_arg == "--Help"){
            cout << "-Help message:" << endl;
            cout << "--Example: \n\t"
                    "--Servers 25 10x0.1,10x0.2,5x0.5, --Dispatchers 1 POC 2 --Time 1000000 --Load {0.5,1.0,0.025}"
                    << endl;
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
    string delimiter = ",", inner_delimiter = "x";
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
