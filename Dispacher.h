//
// Created by Neria on 07/08/2019.
//

#ifndef NETWORKSIMULATOR_DISPACHER_H
#define NETWORKSIMULATOR_DISPACHER_H

#include "defs.h"
#include "MinHeap.h"
#include "Server.h"
#include "JBuffer.h"

class Dispatcher
{

public:

    explicit Dispatcher(int id, int num_servers, double load) : poisson_distribution_(load)
    {
        id_ = id;
        num_servers_ = num_servers;
        main_generator_.seed(id);
        dispatched_jobs = 0;
        for(int i=0;i<num_servers; i++)
            routing_map[i] = 0;
        buffer = JBuffer(1000+id,num_servers);
    }
    ~Dispatcher() = default;
    // arrivals at the dispatcher
    virtual int get_arrivals();
    // random by default. Overridden by derived classes.
    virtual int get_destination();

    string toString() const ;
    friend std::ostream& operator<<(std::ostream& os, const Dispatcher& s);

    static int total_dispatched_jobs;

protected:
    int id_, num_servers_, dispatched_jobs;
    map<int,int> routing_map;
    // Random numbers engine
    default_random_engine main_generator_;
    // Poisson arrivals
    poisson_distribution<int> poisson_distribution_;
    JBuffer buffer;
};


class PocDispatcher: public Dispatcher{
public:
    explicit PocDispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) { }
    virtual int get_destination(Server** servers, int poc);
};

class JsqDispatcher : public Dispatcher
{
public:
    JsqDispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load)
    {        servers_heap_ = new MinHeap(num_servers);    }
    ~JsqDispatcher()  {  delete servers_heap_;    }
    int get_destination() override;
    void update_server(int server_num, int finished_jobs);

protected:
    MinHeap* servers_heap_;
};

class JiqDispatcher: public Dispatcher{
public:
    JiqDispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) {
        idle_servers = vector<int>();
        for(int i=0; i<num_servers; i++)
            idle_servers.push_back(i);
    };
    ~JiqDispatcher() = default;
    int get_destination() override;
    void update_server(int server_num, bool is_idle);

protected:
    vector<int> idle_servers;

};


class PiDispatcher: public JiqDispatcher{
public:
    PiDispatcher(int id, int num_servers, double load) : JiqDispatcher(id, num_servers, load) {
        this->last_idle_server = -1;
    };
    ~PiDispatcher() = default;
    int get_destination() override;
    void update_server(int server_num, bool is_idle);

protected:
    int last_idle_server;

};

class RrDispatcher: public Dispatcher{
public:
    RrDispatcher(int id, int num_servers, double load) : Dispatcher(id, num_servers, load) {
        this->current = rand() % num_servers;
    };
    ~RrDispatcher() = default;
    int get_destination() override;

protected:
    int current;

};




#endif //NETWORKSIMULATOR_DISPACHER_H
