/*

    链表

    创建于2023年2月5日 江西省上饶市玉山县

*/

#pragma once

struct LinkedListNode {
    LinkedListNode* prev;
    LinkedListNode* next;
};

struct LinkedList {
    LinkedListNode* head = nullptr;
    LinkedListNode* tail = nullptr;

    int length = 0;

    LinkedList& append(LinkedListNode* node);

    // todo.
};
