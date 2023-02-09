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


LinkedList& LinkedList::insert(LinkedListNode* node, int index) {
// todo
    return *this;
}

LinkedList& LinkedList::pushBack(LinkedListNode* node) {

    return this->append(node);

}


LinkedList& LinkedList::remove(LinkedListNode* node) {
// todo



    return *this;
}


LinkedListNode* LinkedList::removeAt(int index) {
    auto node = this->get(index);

    this->remove(node);

    return node;
}

LinkedListNode* LinkedList::removeFirst() {
// todo
    return nullptr;
}

LinkedListNode* LinkedList::removeLast() {

    auto res = this->tail;

    if (res) {

        this->tail = res->prev;
        res->prev = nullptr;

        if (this->tail) {
            this->tail->next = nullptr;
        } else {
            this->head = nullptr;
        }

        length--;
        
    }

    res->next = nullptr;
    res->prev = nullptr;

    return res;
}

LinkedListNode* LinkedList::popBack() {
    return this->removeLast();
}


bool LinkedList::contains(LinkedListNode* node) {
    return this->indexOf(node) >= 0;
}


LinkedListNode* LinkedList::getFirst() {
    return this->head;
}

LinkedListNode* LinkedList::getLast() {
    return this->tail;
}

LinkedListNode* LinkedList::get(int index) {

    if (index < length) {
        
        LinkedListNode* res;
        
        if (index < length / 2) {
        
            int counter = index;
            res = this->head;
            while (counter--) {
                res = res->next;
            }
        
        } else {
        
            int counter = length - index - 1;
            res = this->tail;
            while (counter--) {
                res = res->prev;
            }
        
        }

        return res;

    } else {
        return nullptr;
    }
}


int LinkedList::indexOf(LinkedListNode* node) {
    auto curr = this->head;
    int counter = 0;

    while (curr && curr != node) {
        counter++;
        curr = curr->next;
    }

    return curr ? counter : -1;
}

bool LinkedList::isEmpty() {
    return this->length == 0;
}

bool LinkedList::isNotEmpty() {
    return this->length != 0;
}