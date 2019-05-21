#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "skiplist.h"

using namespace std;

int main(int argc, char *argv[]){
    // exit, view, insert, remove    
    string istr;
    skiplist sl;
    int i, iint;
    vector<int> oddarray, evenarray;
    node n;
    sl.init();
    sl.help();
    cout << "\nInput one of the following function.\n"
         << "view(v), find(f), insert(i), remove(r), closestKeyAfter(c), exit(e)\n";           
    while(1){
        cout << ">> ";
        cin >> istr;
        if(istr == "view" || istr == "v"){
            sl.view();
        }
        else if(istr == "closest" || istr == "closestKeyAfter" || istr == "c"){
            cout << "Type an integer you want to find.\n>> ";
            cin >> iint;
            if(sl.closest(iint, i)) cout << "The element " << i << " is the closest key after " << iint << '\n';
            else cout << "Their is no value which is the closest key after " << iint << "\n";     
            sl.view();        
        }
        else if(istr == "find" || istr == "f"){
            cout << "Type an integer you want to find.\n>> ";
            cin >> iint;
            if(sl.find(iint)->val == iint) cout << "The element " << iint << " is found\n";
            else cout << "Their is no value: " << iint <<"\n";      
            sl.view();       
        }
        else if(istr == "insert" || istr == "i"){
            cout << "Type an integer you want to insert.\n>> ";
            cin >> iint;
            srand(time(NULL));
            if(iint == INTMAX || iint == INTMIN) 
                cout << "Invalid insert value\n"; 
            else if(sl.insr(iint)) 
                cout << "Insert " << iint << " succeed\n";
            //else cout << "Already have value: " << iint <<"\n";   
            sl.view();          
        }
        else if(istr == "remove" || istr == "r"){
            cout << "Type an integer you want to remove.\n>> ";
            cin >> iint;
            if(iint == INTMAX || iint == INTMIN) 
                cout << "Invalid remove value\n"; 
            else if(sl.dele(iint)) 
                cout << "Remove " << iint << " succeed\n";
            else cout << "There is no value: " << iint <<"\n"; 
            sl.view();
        }    
        else if(istr == "exit" || istr == "e"){
            cout << "Exit the program.\n";
            break;
        }
        else{
            cout << "Invalid intput, please try again.\n";
        }
    }
    return 0;
}
