#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <algorithm>
#include "skiplist.h"

using namespace std;

int main(int argc, char *argv[]){
    // exit, view, insert, remove    
    string istr;
    skiplist sl;
    int i, iint, tt, ttt;
    double iut, dut, fsut, ffut, cut, it, dt, fst, fft, ct;
    vector<int> oddarray, evenarray;
    node n;
    struct timeval start_t, end_t;
    sl.init();          
    while(1){
        cout << "\nInput iteration times and test times below:\n";
        cout << "iteration times >> ";
        cin >> iint;
        cout << "test times >> ";
        cin >> tt;
        ttt = tt;
    
        sl.init();
        oddarray.clear();
        evenarray.clear();
        for(int j = 0; j != iint; j++){
            oddarray.push_back(j*2+1);
            evenarray.push_back(j*2);
        }    
        it = 0.0;
        dt = 0.0;
        fst = 0.0;
        fft = 0.0;
        ct = 0.0;
        iut = 0.0;
        dut = 0.0;
        fsut = 0.0;
        ffut = 0.0;
        cut = 0.0;      
        while(ttt != 0){  
            srand(time(NULL));
            //insert
            std::random_shuffle(oddarray.begin(), oddarray.end());
            gettimeofday(&start_t, NULL);
            for(i = 0; i != iint; i++){
                sl.insr(oddarray[i]);
            }
            gettimeofday(&end_t, NULL);
            iut += double(end_t.tv_usec - start_t.tv_usec)/tt;
            it += double(end_t.tv_sec - start_t.tv_sec)/tt;
            //ff
            std::random_shuffle(evenarray.begin(), evenarray.end());
            gettimeofday(&start_t, NULL);
            for(int j = 0; j != iint; j++){
                if(sl.find(evenarray[j])->val == evenarray[j]) cout<<"wrong\n";
            }
            gettimeofday(&end_t, NULL);
            ffut += double(end_t.tv_usec - start_t.tv_usec)/tt;
            fft += double(end_t.tv_sec - start_t.tv_sec)/tt;
            //fs
            std::random_shuffle(oddarray.begin(), oddarray.end());
            gettimeofday(&start_t, NULL);
            for(int j = 0; j != iint; j++){
                if(sl.find(oddarray[j])->val != oddarray[j]) cout<<"wrong\n";
            }
            gettimeofday(&end_t, NULL);
            fsut += double(end_t.tv_usec - start_t.tv_usec)/tt;
            fst += double(end_t.tv_sec - start_t.tv_sec)/tt;
            //c
            std::random_shuffle(oddarray.begin(), oddarray.end());
            gettimeofday(&start_t, NULL);
            for(int j = 0; j != iint; j++){
                sl.closest(oddarray[j], i);
            }
            gettimeofday(&end_t, NULL);
            cut += double(end_t.tv_usec - start_t.tv_usec)/tt;
            ct += double(end_t.tv_sec - start_t.tv_sec)/tt;
            //d
            std::random_shuffle(oddarray.begin(), oddarray.end());
            gettimeofday(&start_t, NULL);
            for(i = 0; i != iint; i++){
                sl.dele(oddarray[i]);
            }
            gettimeofday(&end_t, NULL);
            dut += double(end_t.tv_usec - start_t.tv_usec)/tt;
            dt += double(end_t.tv_sec - start_t.tv_sec)/tt;

            ttt--;
        }
       
        cout << '\n' << tt << " times average:\n";
        cout << "Insert time : " << it << " (s) " << iut << " (us)\n";
        cout << "\t=" << it * 1000000 + iut << " (us)\n";
        cout << "Remove time : " << dt << " (s) " << dut << " (us)\n";
        cout << "\t=" << dt * 1000000 + dut << " (us)\n";
        cout << "Find success time : " << fst << " (s) " << fsut << " (us)\n";
        cout << "\t=" << fst * 1000000 + fsut << " (us)\n";
        cout << "Find fail time : " << fft << " (s) " << ffut << " (us)\n";
        cout << "\t=" << fft * 1000000 + ffut << " (us)\n";            
        cout << "Closest element time : " << ct << " (s) " << cut << " (us)\n";
        cout << "\t=" << ct * 1000000 + cut << " (us)\n";
    }
    return 0;
}
