#include <iostream>
#include "string"
#include "Structures.h"
using namespace std;
void Read(string str);
string ToLower(string data);

int main() {
    bool cicle = true;
    string str;
    while(cicle) {
        str = "";
        getline(cin, str);

        if(ToLower(str) == "exit"){
            cicle = false;
            cout << "SEE YOU SOON! =D" << endl;
            continue;
        }

        Read(str);
    }

    return 0;
}
