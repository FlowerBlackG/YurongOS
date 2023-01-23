#include <iostream>
#include <iomanip>
using namespace std;

int main() {

    while (true) {
        cout << "address in hex: ";
        unsigned long long x;
        
        cin >> hex >> x;
        unsigned long long t = x;
        
        int xb[5];
        const char* xbName[] = {
            "B", "K", "M", "G", "T"
        };

        for (int i = 0; i < 5; i++) {
            xb[i] = t % 1024;
            t /= 1024;
        }

        for (int i = 4; i >= 0; i--) {
            cout << xb[i] << xbName[i] << " ";
        }

        cout << endl;

        cout << "lv4: " << ((x >> (12 + 27)) & 0x1ff) << endl;
        cout << "lv3: " << ((x >> (12 + 18)) & 0x1ff) << endl;
        cout << "lv2: " << ((x >> (12 + 9)) & 0x1ff) << endl;
        cout << "lv1: " << ((x >> 12) & 0x1ff) << endl;
    }

}
