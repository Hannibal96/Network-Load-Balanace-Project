//
// Created by Neria on 06/08/2019.
//
#include "defs.h"

#include "Job.h"

Job::Job(int time) {
    creation_time = time;
    waiting_time = 0;
    exiting_time = 0;
}

Job::~Job() {}

int Job::GetCreation() {
    return creation_time;
}
int Job::GetExiting() {
    return exiting_time;
}
int Job::GetWaiting() {
    return waiting_time;
}

void Job::FinishJob(int time) {
    exiting_time = time;
}