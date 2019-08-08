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
    return poisson_distribution_(main_generator_);
}






