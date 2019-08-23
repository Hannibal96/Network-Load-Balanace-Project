#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {

    	vector<string > commandLineArguments;
    	commandLineArguments.assign(argv + 1, argv + argc);
    	for (string inputCommand : commandLineArguments)
    	    cout << inputCommand << endl;

    return 0;
}

