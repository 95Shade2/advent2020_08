#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <functional>

using namespace std;

map<string, function<void(string, int*, int*)>> COMMAND;

int Int(string num) {
    int num_i = 0;

    while (num.size() > 0) {
        num_i *= 10;
        num_i += num[0] - '0';
        num = num.substr(1);
    }

    return num_i;
}

void Setup_Commands() {
    COMMAND["nop"] = [](string argument, int *IP, int *acc) {
        cout << *IP << "\tnop " << argument << "\n\t\t";
    };
    COMMAND["acc"] = [](string argument, int *IP, int *acc) {
        int amount = Int(argument.substr(1));

        cout << *IP << "\tacc " << argument << "\n\t\t";
        cout << *acc << " " << argument[0] << " " << amount << "\n\t\t";
        cout << *acc << " => ";
        amount = argument[0] == '+' ? amount : amount * -1;
        *acc += amount;
        cout << *acc << endl;
    };
    COMMAND["jmp"] = [](string argument, int *IP, int *acc) {
        int amount = Int(argument.substr(1));

        cout << *IP << "\tjmp " << argument << "\n\t\t";
        cout << *IP << " " << argument[0] << " " << amount << "\n\t\t";
        cout << *IP << " => ";
        amount = argument[0] == '+' ? amount : amount * -1;
        *IP += amount;
        cout << *IP << endl;

        (*IP)--;  //it increases the IP after it returns, so this makes it not off by one
    };
}

///Gets the commands from the file
vector<string> Get_Commands() {
    vector<string> commands;
    ifstream file;
    string line;

    file.open("boot.txt");
    getline(file, line);
    while (file) {
        commands.push_back(line);
        getline(file, line);
    }
    file.close();

    return commands;
}

int Run_Command(string command, int IP, int *acc) {
    string com = command.substr(0, command.find(" "));
    string argument = command.substr(command.find(" ")+1);

    if (COMMAND[com]) {
        COMMAND[com](argument, &IP, acc);
    }

    return IP;
}

vector<string> Change_Command(vector<string> commands, int index) {
    vector<string> edited_command;
    string command;
    string name;    //the command name
    int ii = 0;

    for (int i = 0; i < commands.size(); i++) {
        command = commands[i];
        name = command.substr(0, command.find(" "));

        if (name == "jmp") {
            if (ii == index) {
                command.replace(command.find(name), command.find(name)+3, "nop");
                ii++;
            }
            else {
                ii++;
            }
        }
        else if (name == "nop") {
            if (ii == index) {
                command.replace(command.find(name), command.find(name)+3, "jmp");
                ii++;
            }
            else {
                ii++;
            }
        }

        edited_command.push_back(command);
    }

    return edited_command;
}

void Run_Commands(vector<string> commands) {
    int IP = 0;
    int acc = 0;
    string command;
    map<int, bool> ran; //whether or not a instruction has been ran
    vector<string> edited_command;
    int index = 0;

    //part 1
    while (IP != commands.size() && ran[IP] != true) {
        command = commands[IP];
        ran[IP] = true;
        IP = Run_Command(command, IP, &acc);
        IP++;
    }

    cout << IP << " instruction has already ran (" << commands[IP] << ")\n";
    cout << "Accumulator: " << acc << "\n\n";

    //part 2

    do {
        if (index >= commands.size()) {
            cout << "Could not fix it\n";
            return;
        }

        edited_command = Change_Command(commands, index);

        IP = 0;
        acc = 0;
        ran.clear();
        while (IP != edited_command.size() && IP >= 0) {
            if (ran[IP]) {
                IP = -1;
                index++;
            }
            else {
                command = edited_command[IP];
                ran[IP] = true;
                IP = Run_Command(command, IP, &acc);
                IP++;
            }
        }

        cout << index << endl;
    }
    while (IP == -1);

    cout << "After changing the command, the accumulator is now " << acc << endl;
}

int main()
{
    vector<string> commands;

    Setup_Commands();

    commands = Get_Commands();

    Run_Commands(commands);

    return 0;
}
