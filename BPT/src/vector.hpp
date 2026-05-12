#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>

#include "exceptions.hpp"

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
   public:
    /**
     * TODO

     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    long long length;
    T *head = nullptr;
    long long cap = 1;
    class const_iterator;
    class iterator {
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a
        // type. For instance, for an iterator, iterator::value_type is the type
        // that the iterator points to. STL algorithms and containers may use
        // these type_traits (e.g. the following typedef) to work properly. In
        // particular, without the following code,
        // @code{std::sort(iter, iter1);} would not compile.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type:
        // https://blog.csdn.net/u014299153/article/details/72419713 About
        // iterator_category: https://en.cppreference.com/w/cpp/iterator
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        T *ptr = nullptr;
        vector<T> *owner;
        /**
         * TODO add data members
         *   just add whatever you want.
         */
       public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator() {
            owner = nullptr;
        }
        iterator(T *ptr_, vector<T> *owner_) {
            ptr = ptr_;
            owner = owner_;
        }
        iterator operator+(const int &n) const {
            iterator new_;
            new_.ptr = ptr;
            new_.owner = owner;
            for (int i = 1; i <= n; i++) new_.ptr++;
            return new_;
            // TODO
        }
        iterator operator-(const int &n) const {
            iterator new_;
            new_.ptr = ptr;
            new_.owner = this->owner;
            for (int i = 1; i <= n; i++) new_.ptr--;
            return new_;
            // TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const iterator &rhs) const {
            if (rhs.owner != this->owner) throw invalid_iterator();
            int ans = 0;
            ans = (this->ptr - rhs.ptr);
            return ans;  // TODO
        }
        iterator &operator+=(const int &n) {
            for (int i = 1; i <= n; i++) ptr++;
            return *this;
            // TODO
        }
        iterator &operator-=(const int &n) {
            for (int i = 1; i <= n; i++) ptr--;
            return *this;
            // TODO
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator new_;
            new_.owner = owner;
            new_.ptr = ptr;
            ptr++;
            return new_;
        }
        /**
         * TODO ++iter
         */
        iterator &operator++() {
            ptr++;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator new_;
            new_.ptr = ptr;
            new_.owner = owner;
            ptr--;
            return new_;
        }
        /**
         * TODO --iter
         */
        iterator &operator--() {
            ptr--;
            return *this;
        }
        /**
         * TODO *it
         */
        T &operator*() const {
            return *ptr;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return rhs.ptr - ptr == 0;
        }
        bool operator==(const const_iterator &rhs) const {
            return rhs.ptr - ptr == 0;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        long long site() {
            return this->ptr - this->owner->head;
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        /*TODO*/
        T *ptr = nullptr;
        const vector<T> *owner;

       public:
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator(T *ptr_, const vector<T> *owner_) {
            ptr = ptr_;
            owner = owner_;
        }
        const_iterator operator+(const int &n) const {
            const_iterator new_;
            new_.ptr = ptr;
            new_.owner = owner;
            for (int i = 1; i <= n; i++) new_.ptr++;
            return new_;
            // TODO
        }
        const_iterator operator-(const int &n) const {
            const_iterator new_;
            new_.ptr = ptr;
            new_.owner = owner;
            for (int i = 1; i <= n; i++) new_.ptr--;
            return new_;
            // TODO
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if (rhs.owner != this->owner) throw invalid_iterator();
            int ans = 0;
            ans = (this->ptr - rhs.ptr);
            return ans;  // TODO
        }
        const_iterator &operator+=(const int &n) {
            for (int i = 1; i <= n; i++) ptr++;
            return *this;
            // TODO
        }
        const_iterator &operator-=(const int &n) {
            for (int i = 1; i <= n; i++) ptr--;
            return *this;
            // TODO
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator new_;
            new_.owner = owner;
            new_.ptr = ptr;
            ptr++;
            return new_;
        }
        /**
         * TODO ++iter
         */
        const_iterator &operator++() {
            ptr++;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator new_;
            new_.ptr = ptr;
            new_.owner = owner;
            ptr--;
            return new_;
        }
        /**
         * TODO --iter
         */
        const_iterator &operator--() {
            ptr--;
            return *this;
        }
        /**
         * TODO *it
         */
        T &operator*() const {
            return *ptr;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return rhs.ptr - ptr == 0;
        }
        bool operator==(const const_iterator &rhs) const {
            return rhs.ptr - ptr == 0;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        long long site() const {
            return this->ptr - this->owner->head;
        }
    };
    /**
     * TODO Constructs
     * At least two: default constructor, copy constructor
     */
    vector() {
        this->head = nullptr;
        length = 0;
        cap = 1;
    }
    vector(const vector &other) {
        cap = other.cap;
        length = other.length;
        head = (T *)(::operator new(cap * sizeof(T)));
        for (long long i = 0; i < length; i++) new (head + i) T(other.head[i]);
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for (int i = 0; i < length; ++i) head[i].~T();
        ::operator delete(head);
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        T *tmp = head;
        for (int i = 0; i < length; ++i) tmp[i].~T();
        ::operator delete(tmp);
        cap = other.cap;
        length = other.length;
        head = (T *)(::operator new(cap * sizeof(T)));
        for (int i = 0; i < length; i++) new (head + i) T(other.head[i]);
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
        if (pos > length)
            throw index_out_of_bound();
        else {
            return head[pos];
        }
    }
    const T &at(const size_t &pos) const {
        if (pos >= length)
            throw index_out_of_bound();
        else {
            return head[pos];
        }
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T &operator[](const size_t &pos) {
        if (pos >= length) {
            throw index_out_of_bound();
        } else {
            return head[pos];
        }
    }
    const T &operator[](const size_t &pos) const {
        if (pos >= length) {
            throw index_out_of_bound();
        } else {
            return head[pos];
        }
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const {
        if (length == 0)
            throw container_is_empty();
        else
            return head[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const {
        if (length == 0)
            throw container_is_empty();
        else
            return head[length - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator x(head, this);
        return x;
    }
    const_iterator begin() const {
        const_iterator x(head, this);
        return x;
    }
    const_iterator cbegin() const {
        const_iterator x(head, this);
        return x;
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        iterator x(head + length, this);
        return x;
    }
    const_iterator end() const {
        const_iterator x(head + length, this);
        return x;
    }
    const_iterator cend() const {
        const_iterator x(head + length, this);
        return x;
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
        if (length == 0) return true;
        return false;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return length;
    }
    /**
     * clears the contents
     */
    void clear() {
        for (int i = 0; i < length; ++i) head[i].~T();
        ::operator delete(head);
        length = 0;
        cap = 1;
        head = nullptr;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        long long ans = pos.site();
        if (length == 0) {
            head = (T *)(::operator new(sizeof(T)));
            length = 1;
            cap = 1;
            new (head) T(value);
            return iterator(head, this);
        } else if (length < cap) {
            new (head + length) T(*(head + length - 1));
            for (auto i = this->end() - 1; i != pos - 1; i--) {
                *(i + 1) = *i;
            }
            length++;
            *pos = value;
            return pos;
        } else {
            T *tmp = head;
            cap = cap * 2;
            head = (T *)(::operator new(cap * sizeof(T)));
            for (int i = 0; i <= length - 1; i++) new (head + i) T(tmp[i]);
            for (int i = 0; i < length; ++i) tmp[i].~T();
            ::operator delete(tmp);
            pos = iterator(head + ans, this);
            new (head + length) T(*(head + length - 1));
            for (auto i = this->end() - 1; i != pos - 1; i--) {
                *(i + 1) = *i;
            }
            *pos = value;
            length++;
            return pos;
        }
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size
     * because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if (ind > length) throw index_out_of_bound();
        if (length == 0) {
            head = (T *)(::operator new(sizeof(T)));
            length = 1;
            new (head) T(value);
            iterator x(head, this);
            return x;
        } else if (length != cap) {
            new (head + length) T(*(head + length - 1));
            for (int i = length; i > ind; i--) head[i] = head[i - 1];
            head[ind] = value;
            length++;
            iterator x(head + ind, this);
            return x;
        } else {
            T *tmp = head;
            head = (T *)(::operator new(2 * cap * sizeof(T)));
            for (int i = 0; i <= length - 1; i++) new (head + i) T(tmp[i]);
            for (int i = 0; i < length; ++i) tmp[i].~T();
            ::operator delete(tmp);
            new (head + length) T(*(head + length - 1));
            for (int i = length; i > ind; i--) head[i] = head[i - 1];
            head[ind] = value;
            length++;
            cap *= 2;
            iterator x(head + ind, this);
            return x;
        }
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is
     * returned.
     */
    iterator erase(iterator pos) {
        if (length == 1) {
            this->clear();
            return this->end();
        }
        for (auto it = pos; it != this->end() - 1; it++) {
            *it = *(it + 1);
        }
        head[length - 1].~T();
        length--;
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind >= length) throw index_out_of_bound();
        if (length == 1) {
            this->clear();
            return this->end();
        }
        for (int i = ind; i < length - 1; i++) head[i] = head[i + 1];
        head[length - 1].~T();
        iterator x(head + ind, this);
        length--;
        return x;
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        this->insert(length, value);
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        this->erase(length - 1);
    }
};

}  // namespace sjtu

#endif
