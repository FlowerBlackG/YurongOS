// SPDX-License-Identifier: MulanPSL-2.0

/*

    链表

    创建于2023年2月5日 江西省上饶市玉山县

*/

#include "./LinkedList.h"



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

    if (index >= this->length) {
        pushBack(node);
    } else if (index <= 0) {
        if (this->head) {
            node->next = this->head->next;
            this->head->next->prev = node;
        }

        node->prev = nullptr;
        this->head = node;
        this->length++;
    } else {
        
        auto prevNode = this->get(index);
        node->prev = prevNode;
        node->next = prevNode->next;
        prevNode->next->prev = node;
        prevNode->next = node;
        this->length++;

    }
    

    return *this;
}

LinkedList& LinkedList::insertWhen(
    LinkedListNode& node,
    const int64_t cargo, 
    int (* judge) (const int64_t cargo, LinkedListNode* before, LinkedListNode* after)
) {


    LinkedListNode* prev = nullptr;
    LinkedListNode* next = this->head;

    do {

        int judgement = judge(cargo, prev, next);
        if (judgement == 0) { // accept

            this->length++;
            
            if (prev && next) {

                // ... -> prev -> (new) -> next -> ...

                prev->next = &node;
                next->prev = &node;
                node.prev = prev;
                node.next = next;

            } else if (prev) {
                
                // ... -> prev -> (new) -> tail

                this->tail = prev->next = &node;
                node.next = nullptr;
                node.prev = prev;
            
            } else if (next) {
                
                // head -> (new) -> next -> ...

                this->head = next->prev = &node;
                node.next = next;
                node.prev = nullptr;
            
            } else {
                
                // head ->  (new) -> tail
                this->head = this->tail = &node;
                node.prev = node.next = nullptr;
            
            }

            break;

        } else if (judgement < 0) { // cancel
            
            break;

        } else { // continue

            prev = next;
            if (next) {
                next = next->next;
            }

        }

    } while (prev || next);

    return *this;
}

LinkedList& LinkedList::insertWhen(
    LinkedListNode* node,
    const int64_t cargo, 
    int (* judge) (const int64_t cargo, LinkedListNode* before, LinkedListNode* after)
) {
    return this->insertWhen(*node, cargo, judge);
}

LinkedList& LinkedList::pushBack(LinkedListNode* node) {

    return this->append(node);

}


LinkedList& LinkedList::remove(LinkedListNode* node) {

    if (node == this->head && node == this->tail) {
        this->head = this->tail = nullptr;
        
    } else if (node == this->head) {
        this->head->next->prev = nullptr;
        this->head = this->head->next;
        
    } else if (node == this->tail) {
        this->tail->prev->next = nullptr;
        this->tail = this->tail->prev;
        
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    this->length --;

    return *this;
}


LinkedListNode* LinkedList::removeAt(int index) {
    auto node = this->get(index);

    this->remove(node);

    return node;
}

LinkedListNode* LinkedList::removeFirst() {

    LinkedListNode* ret = nullptr;

    if (head) {

        length--;
        ret = head;
        if (this->head == this->tail) {
            this->head = this->tail = nullptr;
        } else {
            this->head->next->prev = nullptr;
            this->head = this->head->next;
        }

    }

    return ret;
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

void LinkedList::clear() {
    this->head = this->tail = nullptr;
}

void LinkedList::forEach(
    void (* callable) (
        LinkedListNode* node
    )
) {
    auto curr = this->head;
    while (curr) {
        callable(curr);
        curr = curr->next;
    }
}
