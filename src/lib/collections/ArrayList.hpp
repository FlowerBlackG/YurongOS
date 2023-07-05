// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Array List Structure
 *
 * 重型结构警告：
 *   该结构借助模板实现，每次使用都可能导致引用的程序体积增加（具体增量未测量）。
 *
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#pragma once

#include "../config.h"
#include "../sys/types.h"
#include "../string.h"

namespace lib {
namespace collections {


template<typename DataType>
class ArrayListIterator {
    typedef ArrayListIterator Self;

protected:
    int64_t size;
    int64_t curr;
    DataType* data;

public:
    ArrayListIterator(DataType* data, int64_t size, int64_t curr) {
        this->size = size;
        this->data = data;
        this->curr = curr;
    }

    DataType& operator * () const { return data[curr]; };
    Self& operator ++ () {
        curr++;
        return *this;
    }

    Self operator ++ (int) {
        auto tmp = *this;
        curr++;
        return tmp;
    }

    friend bool operator == (const Self& a, const Self& b) {
        return a.curr == b.curr
            && a.size == b.size
            && a.data == b.data;
    }

    friend bool operator != (const Self& a, const Self& b) {
        return a.curr != b.curr
            || a.size != b.size
            || a.data != b.data;
    }

};


template<typename DataType>
class ArrayList {

protected:
    int64_t _size = 0;
    int64_t _capacity = 0;
    DataType* _data = nullptr;
    Allocator* allocator;

public:
    ArrayList(Allocator& allocator = lib::defaultAllocator) {
        this->allocator = &allocator;
    }

    void clear() {
        _size = 0; // todo: 未析构数据。
    }

    ~ArrayList() {
        allocator->free(_data); // todo: 未析构数据。
    }

    inline int64_t size() {
        return this->_size;
    }

    inline int64_t capacity() {
        return this->_capacity;
    }

    inline DataType* data() {
        return this->_data;
    }

    int32_t append(const DataType& data) {

        if (_size == _capacity) {
            // 扩容。
            int64_t newCapacity = _capacity;
            if (newCapacity == 0) {
                newCapacity = 16;
            } else {
                newCapacity += newCapacity / 2;
            }

            auto newAddr = (DataType*) allocator->alloc(newCapacity * sizeof(DataType));
            if (!newAddr) {
                return 1; // 异常。
            }

            memcpy(newAddr, _data, _capacity * sizeof(DataType));
            allocator->free(_data);
            _data = newAddr;
            _capacity = newCapacity;
        }

        _data[_size++] = data;

        return 0; // 插入成功。
    }

    inline int32_t push(const DataType& data) {
        return this->append(data);
    }

    DataType pop() {
        if (_size) {
            return _data[--_size]; // todo: 未析构数据。
        }

        return * ( DataType* ) 0;
    }

    DataType& operator [] (int64_t idx) {
        return _data[idx];
    }

    // ------ 迭代相关 ------
    ArrayListIterator<DataType> begin() const {
        return ArrayListIterator<DataType>(_data, _size, 0);
    }

    ArrayListIterator<DataType> end() const {
        return ArrayListIterator<DataType>(_data, _size, _size - 1);
    }

};


}
}
