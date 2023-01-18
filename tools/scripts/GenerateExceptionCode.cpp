#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main() {

    string line;

    stringstream bindIdt, makeHandler, def;

    int index = -1;
    while (cin.good() && !cin.fail() && !cin.bad() && cin.peek() != EOF) {
        index++;

        line.clear();

        while (cin.peek() == ' ' 
            || cin.peek() == '\n' 
            || cin.peek() == '\r'
            || cin.peek() == '\t'
        ) {
            cin.get();
        }

        while (cin.peek() != '\r' && cin.peek() != EOF && cin.peek() != '\n') {
            int ch = cin.get();
            line += ch;
        }

        while (line.length() && line.back() == ' ') {
            line.pop_back();
        }

        if (line == "--") {
            continue;
        }

        for (int pos = 0; pos < line.length(); pos++) {
            if (line[pos] == ' ') {
                line.erase(pos, 1);
                if (line[pos] >= 'a' && line[pos] <= 'z') {
                    line[pos] += 'A' - 'a';
                }
            }
            
        }

        bool errcode = false;
        errcode |= index == 8 || index == 17 || index == 21;
        errcode |= index >= 10 && index <= 14;

        makeHandler << "    IMPLEMENT_EXCEPTION_";
        if (errcode) {
            makeHandler << "WITH_ERRCODE_";
        }
        makeHandler << "ENTRANCE(" 
            << line 
            << "ExceptionEntrance, "
            << line
            << "ExceptionHandler)"
            << endl;
        makeHandler << "    IMPLEMENT_EXCEPTION_";
        if (errcode) {
            makeHandler << "WITH_ERRCODE_";
        }
        makeHandler << "HANDLER(" 
            << line 
            << "ExceptionHandler, \""
            << line
            << "\", 0)"
            << endl;
        
        bindIdt << "    "
            << "idt.setTrapGate("
            << index
            << ", (long) InterruptHandlers::"
            << line
            << "ExceptionEntrance);"
            << endl;


        def << "    void "
            << line
            << "ExceptionEntrance();"
            << endl;
        def << "    void "
            << line
            << "ExceptionHandler(\n"
            << "        SoftwareContextRegisters* softwareRegs,\n"
            << "        HardwareContextRegisters* hardwareRegs\n"
            << "    );"
            << endl
            << endl;

        

    }

    cout << def.str() << endl;
    cout << makeHandler.str() << endl;
    cout << bindIdt.str() << endl;

    return 0;
}
