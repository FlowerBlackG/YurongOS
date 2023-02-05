/*

    空闲内存管理器

    创建于2023年1月14日 上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

class FreeMemoryManager {

public:
    static const int ALLOC_PAGE_NODE_THREHOLD = 50 - 6;
    static const int FREE_PAGE_NODE_THREHOLD = 6;

public:
    FreeMemoryManager() = default;

    void init();

    int free(uint64_t address, uint64_t size, bool dontAdjustPage = false);
    uint64_t alloc(uint64_t size, bool dontAdjustPage = false);
    
    inline uint64_t getTotalFreeMemory() {
        return this->totalFreeMemory;
    }

    uint64_t getMaxAllocatableMemorySize();


protected:

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

protected:
    void adjustPageLinkNodes();
    
protected:
    PageLinkNode* currentPage;
    PageLinkNode* nextPage;

    DoubleLinkedTree addressIdxTree;
    DoubleLinkedTree sizeIdxTree;
    
    uint64_t totalFreeMemory;

private: 
    FreeMemoryManager(const FreeMemoryManager&) = delete;
    

    static_assert(sizeof(FreeMemoryManager::PageLinkNode) == 4096);
    
};

