# Python argument parser for LoadBalance project

import argparse
import sys
import os

parser = argparse.ArgumentParser()
parser.add_argument('--make', action='store_true')
parser.add_argument('--algorithms', action='store_true')
parser.add_argument('--run', action='store_true')
parser.add_argument('--servers')
parser.add_argument('--dispatchers')
parser.add_argument('--dispatcher_type')
parser.add_argument('--load')
parser.add_argument('--relative_mu')
parser.add_argument('--variance')

args = parser.parse_args()

algorithms = ['random',
              'jsq',
              'poc',
              'jiq',
              'lsq_poc',
              'lsq_jiq',
              'lsq_smart',
              'weighted_random',
              'round_robin',
              'weighted_round_robin',
              'pqm',
              'lsq_smart']


def assert_parameters(servers, dispatchers, dispatcher_type, load, relative_mu, variance):
    if int(servers) <= 0:
        print('Servers number must be positive')
    else:
        print('Server number is OK')
    
    if int(dispatchers) <= 0:
        print('Dispatchers number must be positive')
    else:
        print('Dispatchers number is OK')
    
    if dispatcher_type not in algorithms:
        print('Algorithm not available')
    else:
        print('Algorithm is OK')

    if float(load) >= 1 or float(load) <= 0:
        print('Load is out of range(0,1)')
    else:
        print('Load is OK')

    if float(variance) < 0:
        print('Variance must be positive')
    else:
        print('Variance is OK')

if len(sys.argv) == 2:
    if sys.argv[1] == "--algorithms":
        print("#### Algorithms Menu ####")
        for avail_algo in algorithms:
            print(avail_algo)

    if sys.argv[1] == "--make":
        os.system("cd c_program && make")

else:
    if sys.argv[1] == "--run":

        if args.dispatcher_type not in algorithms:
            raise Exception('Algorithm in not available')

        servers = args.servers
        dispatchers = args.dispatchers
        dispatcher_type = args.dispatcher_type
        load = args.load
        relative_mu = args.relative_mu
        variance = args.variance

        print("Servers: ",  servers)
        print("Dispatchers: " , dispatchers)
        print("Dispatchers Type " + dispatcher_type)
        print("Load " , load)
        print("Relative mu " + relative_mu)
        print("Variance " , variance)

        assert_parameters(servers, dispatchers, dispatcher_type, load, relative_mu, variance)

        c_args_sep = " "
        c_program_args = servers + c_args_sep + dispatchers + c_args_sep + dispatcher_type + c_args_sep + load + c_args_sep + relative_mu
        print(c_program_args)
        print("Passed to c:")
        os.system("cd c_program && ./Parser " + c_program_args)
