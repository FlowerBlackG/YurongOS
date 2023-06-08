/*

    空闲内存管理器

    创建于2023年1月14日 上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

/**
 * 空闲内存管理器。负责分配和释放物理内存。
 * 
 * FreeMemoryManager 直接“趴”在物理内存上工作。
 * 其内部使用“双链树”结构管理内存块，线索分别为空闲区大小和开始地址。
 * 
 * 该管理器理论上以字节为单位。实际使用时，推荐以 4K 页为单位使用。
 * 该管理器负责管理物理内存，她不知道虚拟空间是什么东西。
 */
namespace FreeMemoryManager {

    const int ALLOC_PAGE_NODE_THREHOLD = 50 - 6;
    const int FREE_PAGE_NODE_THREHOLD = 6;


    void init();

    int free(uint64_t address, uint64_t size, bool dontAdjustPage = false);
    uint64_t alloc(uint64_t size, bool dontAdjustPage = false);
    
    uint64_t getTotalFreeMemory();

    uint64_t getMaxAllocatableMemorySize();


    struct DoubleLinkedTreeNode {
        
        /**
         * 内容结构。
         * 数组第0项表示按照地址索引。
         *   地址指起始地址，存储物理地址。
         * 数组第1项表示按照尺寸索引。
         *   以字节为单位，应该满足 4KB 对齐。
         */
        struct NodeData {
            uint64_t value;
            DoubleLinkedTreeNode* leftChild;
            DoubleLinkedTreeNode* rightChild;
            DoubleLinkedTreeNode* parent;
            uint64_t reserved;
        } nodeData[2];

        inline NodeData& addressIdxData() {
            return this->nodeData[0];
        }

        
        inline NodeData& sizeIdxData() {
            return this->nodeData[1];
        }

        int compareWith(const DoubleLinkedTreeNode& node, int index) const;
        int compareWith(const DoubleLinkedTreeNode* pNode, int index) const;

    };
    
    /**
     * 双重索引树。
     * 左节点值严格小于父节点。
     * 右节点值可以等于父节点。
     */
    struct DoubleLinkedTree {
        DoubleLinkedTreeNode* root;

        /**
         * 查找节点时，匹配的策略。
         */
        enum class FindNodeStrategy {
            /** 刚好相等。 */
            MATCH,

            /** 如果找不到相等的，就找稍微小一点点的。 */
            MATCH_OR_LESS,

            /** 如果找不到相等的，就找稍微大一点点的。 */
            MATCH_OR_MORE
        };

        static const int NODE_DATA_ADDRESS_IDX = 0;
        static const int NODE_DATA_SIZE_IDX = 1;

        int removeNode(DoubleLinkedTreeNode* node, int index);
        int addNode(DoubleLinkedTreeNode* node, int index);

        /**
         * 
         * 需要手动处理树根指向问题。
         */
        int moveNode(DoubleLinkedTreeNode* dest, DoubleLinkedTreeNode* src);
        DoubleLinkedTreeNode* findNode(uint64_t value, int index, FindNodeStrategy strategy);
    };

    struct PageLinkNode {

        /**
         * 
         * 使用逻辑地址。
         */
        PageLinkNode* prevNode;
        PageLinkNode* nextNode;
        DoubleLinkedTreeNode nodes[50];

        uint64_t nodeUsed;
        uint8_t padding[72];

        DoubleLinkedTreeNode* allocNode();
        int popNode(int count = 1);

    };
    
};

