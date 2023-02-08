/*

    链表

    创建于2023年2月5日 江西省上饶市玉山县

*/

#include <lib/LinkedList.h>



LinkedList& LinkedList::append(LinkedListNode* node) {
    if (tail) {

        tail->next = node;
        node->prev = tail;
        node->next = nullptr;

        tail = node;

    } else {

        node->prev = node->next = nullptr;
        head = tail = node;

    }

    length++;

    return *this;

}


