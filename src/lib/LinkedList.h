// SPDX-License-Identifier: MulanPSL-2.0

/*

    链表

    创建于2023年2月5日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>


/**
 * 链表节点。
 * 将该结构嵌入需要用链表连接的对象结构，通过偏移方式得到数据结构本体。
 */
struct LinkedListNode {
    LinkedListNode* prev;
    LinkedListNode* next;
};

struct LinkedList {
    LinkedListNode* head = nullptr;
    LinkedListNode* tail = nullptr;

    int length = 0;

    LinkedList& append(LinkedListNode* node);
    LinkedList& insert(LinkedListNode* node, int index);

    /**
     * 逐位置判断是否可插入。
     * 
     * @param judge 返回 0 表示可以插入。返回正数表示继续查找。返回负数表示取消操作。
     */
    LinkedList& insertWhen(
        LinkedListNode& node,
        const int64_t cargo, 
        int (* judge) (const int64_t cargo, LinkedListNode* before, LinkedListNode* after)
    );

    LinkedList& insertWhen(
        LinkedListNode* node,
        const int64_t cargo, 
        int (* judge) (const int64_t cargo, LinkedListNode* before, LinkedListNode* after)
    );

    LinkedList& pushBack(LinkedListNode* node);

    LinkedList& remove(LinkedListNode* node);
    
    LinkedListNode* removeAt(int index);
    LinkedListNode* removeFirst();
    LinkedListNode* removeLast();
    LinkedListNode* popBack();

    bool contains(LinkedListNode* node);

    LinkedListNode* getFirst();
    LinkedListNode* getLast();
    LinkedListNode* get(int index);

    int indexOf(LinkedListNode* node);

    bool isEmpty();
    bool isNotEmpty();

    // todo.
};
