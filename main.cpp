#include <iostream>
#include "string"
#include "Structures.h"
using namespace std;
void Read(string str);
string ToLower(string data);

int main() {
    system("clear");
    bool cicle = true;
    string str;
    while(cicle) {
        str = "";
        getline(cin, str);

        if(ToLower(str) == "exit"){
            cicle = false;
            exit(EXIT_SUCCESS);
            cout << "SEE YOU SOON! =D" << endl;
            continue;
        }

        Read(str);
    }

    return 0;
}
