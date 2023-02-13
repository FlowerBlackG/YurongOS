/*

    链表

    创建于2023年2月5日 江西省上饶市玉山县

*/

#pragma once

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
