#include <bits/stdc++.h>

namespace stl { // 按照 STL 的风格自己实现的几个数据结构

template <typename, typename = std::void_t<>>
class is_container_type : public std::false_type {};

template <typename T>
class is_container_type 
    <T, 
        std::void_t < typename T::iterator, 
            decltype(std::declval <T> ().begin(), std::declval <T> ().end())
        >
    > : public std::true_type {};

template <typename T, typename inType>
concept is_iterator_type = requires(T it) {
    {*it} -> std::convertible_to <inType>;
    it++;
};

template <typename _T>
class List {
    private:

    class Element { // 内部存储结构（完全隐藏
        public:
        Element *pre, *nxt;
        _T ele;
        Element() : pre(nullptr), nxt(nullptr) {}
        Element(const _T &x) : pre(nullptr), nxt(nullptr), ele(x) {}
        Element(Element *p, const _T &x, Element *n) : pre(p), nxt(n), ele(x) {}
    };

    int _size;
    Element *head; // 第一个元素前面的元素，恒定存在
    Element *tail; // 指向最后一个元素（左开右闭）

    public:

    class iterator { // 公开的迭代器，用于外界遍历容器
        public:
        Element *ptr;
        
        iterator() : ptr(nullptr) {}
        iterator(const iterator &np) : ptr(np.ptr) {}
        iterator(Element *np) : ptr(np) {}
        iterator(const Element &val) : ptr(&val) {}

        // 一些基本且简单的迭代器运算符
        _T &operator * () { return this->ptr->ele; } // 直接返回指向的 Element 里的 ele 信息
        _T *operator ->() { return &(this->ptr->ele); }
        iterator operator ++ (int) { this->ptr = this->ptr->nxt; return *this; }
        iterator operator -- (int) { this->ptr = this->ptr->pre; return *this; }
        iterator &operator ++ ()   { this->ptr = this->ptr->nxt; return *this; }
        iterator &operator -- ()   { this->ptr = this->ptr->pre; return *this; }
        bool operator == (const iterator &p) const { return this->ptr == p.ptr; }
        bool operator != (const iterator &p) const { return !(*this == p); }
        iterator operator + (const int &p) const {
            auto it = *this;
            for(int i = 0; i < p; i++) ++it;
            return it;
        }
        iterator operator - (const int &p) const {
            auto it = *this;
            for(int i = 0; i < p; i++) --it;
            return it;
        }
    };

    List() {
        head = new Element(nullptr, _T(), nullptr);
        _size = 0;
        tail = head;
    }

    // 用已有容器的迭代器区间构造
    template <typename _inType>
    requires is_iterator_type <_inType, _T>
    List(const _inType begin, const _inType end) {
        head = new Element(nullptr, _T(), nullptr);
        _size = 0;
        tail = head;
        auto p = begin;
        while(p != end) {
            this->push_back(*p);
            p++;
        }
    }

    // 直接用可顺序遍历容器构造
    template <typename _conType>
    requires is_container_type <_conType> ::value
    List(const _conType& con) {
        head = new Element(nullptr, _T(), nullptr);
        _size = 0;
        tail = head;
        for(auto p: con) {
            this->push_back(p);
        }
    }

    List (const List &con) {
        head = new Element(nullptr, _T(), nullptr);
        _size = 0;
        tail = head;
        for(auto p: con) {
            this->push_back(p);
        }
    }

    ~List() { // 析构要释放内存
        auto it = this->tail;
        while(it != this->head) {
            it = it->pre;
            delete it->nxt;
        }
        delete it;
    }

    void push_back(const _T &x) {
        ++_size;
        tail->nxt = new Element(tail, x, nullptr);
        tail = tail->nxt;
    }

    void push_front(const _T &x) {
        ++_size;
        head->ele = x;
        head->pre = new Element(nullptr, _T(), head);
        head = head->pre;
    }

    void pop_back() {
        if(_size == 0) // 非法删除
            return;
        this->erase(this->getTail());
    }

    void pop_front() {
        if(_size == 0)
            return;
        this->erase(this->begin());
    }

    // 在 pos 迭代器后面插入一个 val
    void insert(const iterator &pos, const _T &val) {
        if(pos.ptr == nullptr)
            return;
        ++_size;
        if(pos.ptr->nxt == nullptr) {
            pos.ptr->nxt = new Element(pos.ptr, val, nullptr);
            tail = pos.ptr->nxt;
            return;
        }
        Element *newp = new Element(pos.ptr, val, pos.ptr->nxt);
        pos.ptr->nxt->pre = newp;
        pos.ptr->nxt = newp;
    }

    // 把 pos 迭代器删除
    void erase(const iterator &pos) {
        if(pos.ptr == nullptr)
            return;
        if(pos.ptr->nxt)
            pos.ptr->nxt->pre = pos.ptr->pre;
        if(pos.ptr->pre) {
            if(pos.ptr == this->tail)
                this->tail = this->tail->pre;
            pos.ptr->pre->nxt = pos.ptr->nxt;
        }
        delete pos.ptr;
        --_size;
    }

    // begin() 和 end() 提供左闭右开的迭代器
    iterator begin() const { return iterator(head->nxt); }
    iterator end()   const { return iterator(nullptr); } // 右开的一直是 nullptr
    // getTail() 和 getHead() 提供左开右闭的迭代器
    iterator getTail() const { return iterator(tail); }
    iterator getHead() const { return iterator(head); } 

    _T  back() const { return tail->ele; }
    _T front() const { return head->nxt->ele; }
    int size() const { return this->_size; }
    void clear() {*this = List();}

    void __debugPrint() const { // 顺序输出 List 里的元素
        printf("List: ");
        for(auto it = this->begin(); it != this->end(); it++) {
            std::cout << *it << ", "; // _T 需要重载过输出流
        }
        puts("");
    }

    List &operator =(const List &p) {
        head = new Element(nullptr, _T(), nullptr);
        _size = 0;
        tail = head;
        for(auto it = p.begin(); it != p.end(); it++) 
            this->push_back(*it);
        return *this;
    }

    void sort() { // O(n^2) 排序
        if(this->_size <= 1)
            return;
        for(auto i = this->begin(); i != this->end(); i++) {
            for(auto j = i + 1; j != this->end(); j++) {
                if(*j < *i) {
                    auto tmp = *i;
                    *i = *j;
                    *j = tmp;
                }
            }
        }
    }

    void reverse() {
        if(this->_size <= 1) 
            return;
        auto i = this->begin();
        auto j = this->getTail();
        while(i != j && i - 1 != j) {
            auto tmp = *i;
            *i = *j;
            *j = tmp;
            ++i, --j;
        }
    }
};


template <typename _TA, typename _TB>
class Pair {
    public:
    _TA a;
    _TB b;
    Pair() {}
    template <typename _inTA, typename _inTB>
    Pair(const _inTA &f, const _inTB &s) : a(f), b(s) {}

    bool operator ==(const Pair &p) const {
        return (this->a == p.a) && (this->b == p.b);
    }
    bool operator < (const Pair &p) const { // 字典序比较
        if(this->a == p.a) return this->b < p.b;
        return this->a < p.a;
    }
    bool operator != (const Pair &p) const {return !(*this == p);}
    bool operator <= (const Pair &p) const {return *this < p || *this == p;}
    bool operator >  (const Pair &p) const {return !(*this <= p);}
    bool operator >= (const Pair &p) const {return *this > p || *this == p;}
    Pair &operator = (const Pair &p) {
        this->a = p.a;
        this->b = p.b;
        return *this;
    }
};

template <typename _TA, typename _TB>
auto make_pair(const _TA &a, const _TB &b) {
    return Pair <_TA, _TB> (a, b);
}

template <typename _T>
class Queue {
    private:
    List <_T> L;

    public:
    _T front () {return L.front();}
    _T back() {return L.back();}
    void push (const _T &x) {L.push_back(x);}
    void pop () {L.pop_front();}
    void pop_back () {L.pop_back();}
    int size () {return L.size();}
    void clear () {L.clear();}
    bool empty() {return L.size() == 0;}
    
    void __debugPrint() {
        L.__debugPrint();
    }
};

template <typename _T>
class Stack {
    private:
    List <_T> L;

    public:
    _T top () {return L.back();}
    void push (const _T &x) {L.push_back(x);}
    void pop () {L.pop_back();}
    int size () {return L.size();}
    void clear () {L.clear();}
    bool empty() {return L.size() == 0;}

    void __debugPrint() {
        L.__debugPrint();
    }
};

}

stl::Pair <int, double> a[5];

#include <vector>

std::vector <int> v1(5);

int main () {
    std::cout << stl::is_container_type<stl::List <int>>::value << '\n';
    std::cout << stl::is_container_type<stl::Pair <int, int>>::value << '\n';
    v1[0] = 1;
    v1[1] = 2;
    v1[2] = 3;
    v1[3] = 4;
    v1[4] = 5;
    for(auto i: v1) std::cout << i << ' ';
    puts("");
    stl::List <int> v2(v1.begin(), v1.end());
    for(auto i: v2) std::cout << i << ' ';
    puts("");
    stl::List <int> v4(v2);
    for(auto i: v4) std::cout << i << ' ';
    puts("");
    stl::List <int> v3(v1);
    for(auto i: v3) std::cout << i << ' ';
    puts("");
    return 0;
}