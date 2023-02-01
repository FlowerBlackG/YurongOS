#include <iostream>
#include <iomanip>
using namespace std;

#define __packed __attribute__((packed))

struct SegmentDescriptor {
    uint16_t limitLow : 16;
    uint16_t baseLow : 16;

    uint8_t baseMid : 8;

    // access byte

    /** 保留为 0，供 cpu 使用。 */
    uint8_t access : 1;

    /**
     * readable bit or writable bit
     * 
     * 对于代码段，该变量规定指定内容是否可读。
     * 代码段永远不可写。
     * 
     * 对于数据段，该变量规定内容是否可写。
     * 数据段永远可读。
     */
    uint8_t rw : 1;

    /**
     * direction bit or conforming bit
     * 
     * 对于数据段，该值为 0 时表示“向上生长”，为 1 表示“向下生长”。
     * 生长方向表示的是 limit 相对 base 的生长方向。
     * 该“生长”并不会影响运行过程中 esp 的增长方向。
     * （运行时，入栈时 esp 应该是向下增长的）
     */
    uint8_t dc : 1;

    /**
     * 0 表示描述符定义的是数据段。
     * 1 表示描述符表示的段允许被执行。
     */
    uint8_t executable : 1;

    /** 0表示系统段，1表示代码/数据段。 */
    uint8_t descriptorType : 1;
    uint8_t privilegeLevel : 2;
    uint8_t present : 1;

    uint8_t limitHigh : 4;

    // flags

    uint8_t reserved : 1;

    /** 表示是否使用64位模式。 */
    uint8_t longMode : 1;

    /** 0表示16位保护模式段。1表示32位保护模式段。 */
    uint8_t sizeFlag : 1;

    /** 粒度。0表示1byte，1表示4KB，用于描述 limit 的粒度。 */
    uint8_t granularity : 1;


    uint8_t baseHigh : 8;

} __packed;

struct GdtRegister {
    uint16_t limit :16;
    uint64_t baseAddress:64 ;
} __attribute__((packed));

unsigned char segs[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00
};

long long qSegs[] = {
 0x00009a000000ffff, 0x00cf92000000ffff
};

int main() {
    
    SegmentDescriptor* pS = reinterpret_cast<SegmentDescriptor*> (qSegs);

    for (int i = 0; i < 2; i++) {
        auto& it = pS[i];
        cout << endl;
        cout << "idx      : " << i + 1 << endl;
        cout << hex;
        cout << "limit    : " << 0LL + (it.limitLow | (it.limitHigh << 16)) << endl;
        cout << dec;
        cout << "base     : " << 0LL + (it.baseLow | (it.baseMid << 16) | (it.baseHigh << 24)) << endl;
        cout << "access   : " << !!it.access << endl;
        cout << "rw       : " << !!it.rw << endl;
        cout << "dc       : " << !!it.dc << endl;
        cout << "exec     : " << !!it.executable << endl;

        cout << "descType : " << (it.descriptorType ? "code/data" : "system") << endl;
        cout << "privi lv : " << (it.privilegeLevel & 0xf) << endl;
        cout << "present  : " << !!(it.present) << endl;
        cout << "longMode : " << !!(it.longMode) << endl;
        cout << "sizeFlag : " << (it.sizeFlag ? "32 bits" : "16 bits") << endl;
        cout << "granular : " << (it.granularity ? "4 KB" : "1 B") << endl;    

    }

    return 0;
}
