#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>

using namespace std;

void displayHelp() {
    cout << "Usage: kill_process_using_port [--port <port_number> | <port_number>]" << endl;
    cout << "Options:" << endl;
    cout << "  --port <port_number>    Specify the port number to kill processes using that port." << endl;
    cout << "  --help                  Display this help message." << endl;
}

void killProcessByPort(int port) {
    string command = "netstat -aon | findstr :" + to_string(port);
    string taskkill = "taskkill /F /PID ";

    // Execute netstat command and find the process ID
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Error executing command." << endl;
        return;
    }

    char buffer[128];
    string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }
    _pclose(pipe);


    // Split the result by newline
    stringstream ss(result);
    string line;
    while (getline(ss, line)) {
        // Split each line by whitespace
        istringstream iss(line);
        vector<string> tokens;
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));

        // If the line contains "LISTENING", get the PID
        if (tokens.size() >= 5 && tokens[0] == "TCP") {
            string pid = tokens[4];
            string killCommand = taskkill + pid;
            system(killCommand.c_str());
            cout << "Process using port " << port << " terminated." << endl;
        }
    }

    if (result.empty()) {
        cout << "No process found using port " << port << "." << endl;
    }
}

int main(int argc, char* argv[]) {
    int port = 0;
    

    // Check if there are enough arguments
    if (argc > 2) {
        // Loop through arguments to find "--port" flag
        for (int i = 1; i < argc - 1; ++i) {
            if (string(argv[i]) == "--port") {
                port = stoi(argv[i + 1]);
                break;
            }
        }
    }else if (argc > 1) {
        if (string(argv[1]) == "--help") {
            displayHelp();
            return 0;
        }
        port = stoi(argv[1]);
    }
    else {
        cout << "Enter the port number, or CTRL + C to exit: ";
        cin >> port;
    }    

    killProcessByPort(port);

    return 0;
}
