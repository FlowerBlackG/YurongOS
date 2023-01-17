#include <iostream>
#include <iomanip>
using namespace std;

int main() {

    while (true) {
        cout << "address in hex: ";
        unsigned long long x;
        
        cin >> hex >> x;
        double f = x;
        
        if (x < 1024) {
            cout << f << " byte" << endl;
        } else if (x / 1024 < 1024) {
            cout << f / 1024 << " KB" << endl;
        } else if (x / 1024 / 1024 < 1024) {
            cout << f / 1024 / 1024 << " MB" << endl;
        } else if (true) {
            cout << f / 1024 / 1024 / 1024 << " GB" << endl;
        }

        cout << "lv4: " << ((x >> (12 + 27)) & 0x1ff) << endl;
        cout << "lv3: " << ((x >> (12 + 18)) & 0x1ff) << endl;
        cout << "lv2: " << ((x >> (12 + 9)) & 0x1ff) << endl;
        cout << "lv1: " << ((x >> 12) & 0x1ff) << endl;
    }

}
