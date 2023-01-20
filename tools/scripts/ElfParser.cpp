/*

    ELF 文件解析实验。
    创建于2023年1月19日

*/

#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>

using namespace std;

#define __packed __attribute__((packed))

struct Elf64Header {
    uint8_t magic[4];

    /**
     * 位宽。
     * 1 = 32 bits, 2 = 64 bits
     */
    uint8_t bitType;

    /**
     *
     * 1 = little endian
     * 2 = big endian
     */
    uint8_t endianType;

    uint8_t elfHeaderVersion;
    uint8_t osAbi;
    uint64_t padding; // byte 8-15

    /**
     * 
     * 1 = relocatable
     * 2 = executable
     * 3 = shared
     * 4 = core
     */
    uint16_t objectType;
    uint16_t instructionSet;

    uint32_t elfVersion;
    uint64_t programEntryPosition;
    uint64_t programHeaderTablePosition;
    uint64_t sectionHeaderTablePosition;
    uint32_t flags;

    uint16_t headerSize;
    uint16_t programHeaderTableEntrySize;
    uint16_t programHeaderTableEntriesCount;
    
    uint16_t sectionHeaderTableEntrySize;
    uint16_t sectionHeaderTableEntriesCount;
    
    uint16_t sectionHeaderTableIdxWithSectionNames;
} __packed;

int checkElfHeader(Elf64Header& header) {

    cout << "------------------------" << endl;
    cout << "[elf header]" << endl;

    // 检查 magic
    if (header.magic[0] != 0x7f
        || header.magic[1] != 'E'
        || header.magic[2] != 'L'
        || header.magic[3] != 'F'
    ) {
        cout << "error: bad magic." << endl;
        return -1;
    }

    cout << "magic: " << endl;
    cout << "  0x7f ELF" << endl;

    // 检查 bit type
    cout << "bit width: " << endl;
    cout << "  ";
    if (header.bitType == 1) {
        cout << "32 bits";
    } else if (header.bitType == 2) {
        cout << "64 bits";
    } else {
        cout << "unrecognized";
    }

    cout << " (" << (int) header.bitType << ")" << endl;
    if (header.bitType != 2) {
        cout << "error: bit type not supported." << endl;
        return -1;
    }

    // 检查 endian type
    cout << "endian type:" << endl << "  ";
    if (header.endianType == 1) {
        cout << "little endian";
    } else if (header.endianType == 2) {
        cout << "big endian";
    } else {
        cout << "unrecognized";
    }
    cout << " (" << (int) header.endianType << ")" << endl;
    if (header.endianType != 1) {
        cout << "error: endian type not supported." << endl;
    }

    cout << "elf header version:" << endl 
        << "  " << (int) header.elfHeaderVersion << endl;
    
    cout << "os abi:" << endl;
    cout << "  " << (int) header.osAbi << endl;

    cout << "object type:" << endl;
    cout << "  ";
    switch (header.objectType) {
        case 1: {
            cout << "relocatable";   
            break;
        }

        
        case 2: {
            cout << "executable";   
            break;
        }

        
        case 3: {
            cout << "shared";   
            break;
        }

        
        case 4: {
            cout << "core";   
            break;
        }
        
        default: {
            cout << "unrecognized";
            break;
        }
    }
    cout << " (" << header.objectType << ")" << endl;

    cout << "instruction set:" << endl;
    cout << "  ";
    switch (header.instructionSet) {

        case 0: {
            cout << "not specified";
            break;
        }
        
        case 2: {
            cout << "sparc";
            break;
        }
        
        case 3: {
            cout << "x86";
            break;
        }
        
        case 8: {
            cout << "mips";
            break;
        }
        
        case 0x14: {
            cout << "powerPC";
            break;
        }
        
        case 0x28: {
            cout << "arm";
            break;
        }
        
        case 0x2a: {
            cout << "superH";
            break;
        }
        
        case 0x32: {
            cout << "ia64";
            break;
        }
        
        case 0x3e: {
            cout << "x86-64";
            break;
        }
        
        case 0xb7: {
            cout << "aarch64";
            break;
        }
        
        case 0xf3: {
            cout << "risc-v";
            break;
        }

        default: {
            cout << "unrecognized";
            break;
        }
    }
    cout << " (" << header.instructionSet << ")" << endl;

    cout << "elf version:" << endl << "  ";
    cout << header.elfVersion << endl;

    cout << "program entry position:" << endl << "  ";
    cout << hex << "0x" << header.programEntryPosition << endl;

    cout << "program header table position:" << endl << "  ";
    cout << "0x" << header.programHeaderTablePosition << endl;

    
    cout << "section header table position:" << endl << "  ";
    cout << "0x" << header.sectionHeaderTablePosition << endl;

    cout << oct;

    cout << "flags:" << endl;
    cout << "  " << header.flags << endl;

    cout << "header size:" << endl;
    cout << "  " << header.headerSize << endl;
    
    cout << "size of an entry in program header table:" << endl;
    cout << "  " << header.programHeaderTableEntrySize << endl;

    cout << "entries count in program header table:" << endl;
    cout << "  " << header.programHeaderTableEntriesCount << endl;

    cout << "size of an entry in section header table:" << endl;
    cout << "  " << header.sectionHeaderTableEntrySize << endl;

    cout << "entries count in section header table:" << endl;
    cout << "  " << header.sectionHeaderTableEntriesCount << endl;
    
    cout << "idx in section header table with the section names:" << endl;
    cout << "  " << header.sectionHeaderTableIdxWithSectionNames << endl;

    return 0;
}

int parse(ifstream& fin) {
    fin.seekg(0, ios::end);
    size_t fileSize = fin.tellg();
    fin.seekg(0, ios::beg);

    if (fileSize < sizeof(Elf64Header)) {
        cout << "error: file must be at least " 
            << sizeof(Elf64Header) << " bytes."
            << endl;
        return -1;
    }

    Elf64Header elfHeader;
    fin.read(reinterpret_cast<char*>(&elfHeader), sizeof(Elf64Header));

    int ret = checkElfHeader(elfHeader);
    if (!ret) {
        return ret;
    }

    

    return 0;
}

int main(int argc, const char* argv[]) {

    if (argc < 2) {
        cout << "usage: ElfParser [filename]" << endl;
        return -1;
    }

    ifstream fin(argv[1]);
    if (!fin.is_open()) {
        cout << "error: failed to open file " << argv[1] << endl;
        return -1;
    }

    return parse(fin);
}

#if 0

static void __check_size() {
    sizeof(Elf64Header); // 64 Bytes
}

#endif