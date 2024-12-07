#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include <stddef.h>

template<typename T>
class SimpleVector {
public:
    SimpleVector() : _size(0), _capacity(4), _data(new T[_capacity]) {}

    ~SimpleVector() {
        delete[] _data;
    }

    void push_back(const T& value) {
        if (_size >= _capacity) {
            _resize();
        }
        _data[_size++] = value;
    }

    T& operator[](size_t index) {
        return _data[index];
    }

    const T& operator[](size_t index) const {
        return _data[index];
    }

    size_t size() const {
        return _size;
    }

private:
    void _resize() {
        _capacity *= 2;
        T* newData = new T[_capacity];
        for (size_t i = 0; i < _size; ++i) {
            newData[i] = _data[i];
        }
        delete[] _data;
        _data = newData;
    }

    size_t _size;
    size_t _capacity;
    T* _data;
};

#endif // SIMPLE_VECTOR_H