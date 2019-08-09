//
// Created by Neria on 07/08/2019.
//

#ifndef NETWORKSIMULATOR_DISPACHER_H
#define NETWORKSIMULATOR_DISPACHER_H

#include "defs.h"
#include "MinHeap.h"

class Dispatcher
{

public:

    Dispatcher(int id, int num_servers, double load) : poisson_distribution_(load)
    {
        id_ = id;
        num_servers_ = num_servers;
        main_generator_.seed(id);
        dispatched_jobs = 0;
    }
    ~Dispatcher() = default;
    // arrivals at the dispatcher
    int get_arrivals();
    // random by default. Overridden by derived classes.
    virtual int get_destination();

    string toString() const ;
    friend std::ostream& operator<<(std::ostream& os, const Dispatcher& s);

protected:
    int id_;
    int num_servers_;
    int dispatched_jobs;
    // Random numbers engine
    default_random_engine main_generator_;
    // Poisson arrivals
    poisson_distribution<int> poisson_distribution_;
};



#endif //NETWORKSIMULATOR_DISPACHER_H
