//
// Created by Neria on 07/08/2019.
//

#include "Dispacher.h"

int Dispatcher::get_destination()
{
    return rand() % num_servers_;
}

int Dispatcher::get_arrivals()
{
    int new_arraivals = poisson_distribution_(main_generator_);
    dispatched_jobs += new_arraivals;
    return new_arraivals;
}

string Dispatcher::toString() const {
    return "Dispatcher Number: "+::to_string(id_)+
        "\n  -dispached jobs: "+::to_string(dispatched_jobs);
}

std::ostream& operator<<(std::ostream& os, const Dispatcher& dsp)
{
    os << dsp.toString();
    return os;
}




