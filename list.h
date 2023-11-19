#include <iostream>
#include <vector>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class List {
  public:
    explicit List(const Allocator& _alloc = Allocator());
    List(size_t count);
    List(size_t count, const T& value, const Allocator& _alloc = Allocator());
    List(const List& another);
    List& operator=(const List& another);
    ~List();

    Allocator get_allocator() const;
    size_t size() const;

    void push_back(const T& x);
    void push_front(const T& x);
    void pop_back();
    void pop_front();

  private:
    struct Node {
        T value;
        Node* next;
        Node* prev;

        Node();
        Node(const T& value);
        Node(const Node& another);
    };

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<NodeAlloc>;

    Node* _base;
    NodeAlloc _alloc;
    size_t _size;

    template<bool isConst>
    struct common_iterator {
      private:
        Node* adress;
      public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<isConst, const T, T>;
        using pointer = std::conditional_t<isConst, const T*, T*>;
        using reference = std::conditional_t<isConst, const T&, T&>;
        using iterator_category = std::bidirectional_iterator_tag;

        common_iterator();
        common_iterator(Node* x);
        common_iterator(const common_iterator<false>& it);
        bool operator==(common_iterator<isConst> x);
        bool operator!=(common_iterator<isConst> x);
        reference operator*();
        pointer operator->();
        common_iterator<isConst>& operator++();
        common_iterator<isConst> operator++(int);
        common_iterator<isConst>& operator--();
        common_iterator<isConst> operator--(int);


        friend class List<T, Allocator>;
    };
  public:
    typedef common_iterator<false> iterator;
    typedef common_iterator<true> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() const;
    iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin() const;
    reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    template<bool isConst>
    iterator insert(common_iterator<isConst> it, const T& value);
    template<bool isConst>
    iterator insert(common_iterator<isConst> it);
    template<bool isConst>
    void erase(common_iterator<isConst> it);
};

// *****
// LIST
// *****

template<typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& _alloc): _alloc(_alloc), _size(0) {
    _base = AllocTraits::allocate(this->_alloc, 1);
    _base->next = _base->prev = _base;
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count): _alloc(Allocator()), _size(0) {
    _base = AllocTraits::allocate(this->_alloc, 1);
    _base->next = _base->prev = _base;
    for (size_t i = 0; i < count; ++i) {
        insert(begin());
    }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& _alloc):
        _alloc(_alloc), _size(0) {
    _base = AllocTraits::allocate(this->_alloc, 1);
    _base->next = _base->prev = _base;
    for (size_t i = 0; i < count; ++i) {
        insert(begin(), value);
    }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List& another): _size(0) {
    _alloc = AllocTraits::select_on_container_copy_construction(another._alloc);

    _base = AllocTraits::allocate(_alloc, 1);
    _base->next = _base->prev = _base;
    iterator it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& another) {
    if (another._base == _base) return *this;
    while (_size != 0) {
        erase(begin());
    }
    AllocTraits::deallocate(_alloc, _base, 1);

    if (AllocTraits::propagate_on_container_copy_assignment::value) {
        _alloc = another._alloc;
    }

    _base = AllocTraits::allocate(_alloc, 1);
    _base->next = _base->prev = _base;
    iterator it = another.begin();
    while (it != another.end()) {
        push_back(*it);
        ++it;
    }
    return *this;
}

template<typename T, typename Allocator>
List<T, Allocator>::~List() {
    while (_size != 0) {
        erase(begin());
    }
    AllocTraits::deallocate(_alloc, _base, 1);
}

// *****

template<typename T, typename Allocator>
Allocator List<T, Allocator>::get_allocator() const {
    return _alloc;
}

template<typename T, typename Allocator>
size_t List<T, Allocator>::size() const {
    return _size;
}

// *****

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
    insert(end(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
    insert(begin(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
    erase(--end());
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
    erase(begin());
}

// *****

template<typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() const {
    return iterator(_base->next);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() const {
    return iterator(_base);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
    return const_iterator(_base->next);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
    return const_iterator(_base);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() const {
    return reverse_iterator(_base);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() const {
    return reverse_iterator(_base->next);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const {
    return const_reverse_iterator(_base);
}

template<typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const {
    return const_reverse_iterator(_base->next);
}

// *****

template<typename T, typename Allocator>
template<bool isConst>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(common_iterator<isConst> it, const T& value) {
    Node* node = AllocTraits::allocate(_alloc, 1);
    AllocTraits::construct(_alloc, node, value);
    node->prev = it.adress->prev;
    node->next = it.adress;
    it.adress->prev = node;
    node->prev->next = node;
    ++_size;
    return iterator(node);
}

template<typename T, typename Allocator>
template<bool isConst>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(common_iterator<isConst> it) {
    Node* node = AllocTraits::allocate(_alloc, 1);
    AllocTraits::construct(_alloc, node);
    node->prev = it.adress->prev;
    node->next = it.adress;
    it.adress->prev = node;
    node->prev->next = node;
    ++_size;
    return iterator(node);
}

template<typename T, typename Allocator>
template<bool isConst>
void List<T, Allocator>::erase(common_iterator<isConst> it) {
    it.adress->next->prev = it.adress->prev;
    it.adress->prev->next = it.adress->next;
    AllocTraits::destroy(_alloc, it.adress);
    AllocTraits::deallocate(_alloc, it.adress, 1);
    --_size;
}

// *****
// List::iterator

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::common_iterator<isConst>::common_iterator() {}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::common_iterator<isConst>::common_iterator(Node* x): adress(x) {}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::common_iterator<isConst>::common_iterator(const common_iterator<false>& it): adress(it.adress) {}

template<typename T, typename Allocator>
template<bool isConst>
bool List<T, Allocator>::common_iterator<isConst>::operator==(List<T, Allocator>::common_iterator<isConst> x) {
    return adress == x.adress;
}

template<typename T, typename Allocator>
template<bool isConst>
bool List<T, Allocator>::common_iterator<isConst>::operator!=(List<T, Allocator>::common_iterator<isConst> x) {
    return !operator==(x);
}

template<typename T, typename Allocator>
template<bool isConst>
std::conditional_t<isConst, const T&, T&> List<T, Allocator>::common_iterator<isConst>::operator*() {
    return adress->value;
}

template<typename T, typename Allocator>
template<bool isConst>
std::conditional_t<isConst, const T*, T*> List<T, Allocator>::common_iterator<isConst>::operator->() {
    return &(adress->value);
}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::template common_iterator<isConst>& List<T, Allocator>::common_iterator<isConst>::operator++() {
    adress = adress->next;
    return *this;
}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::template common_iterator<isConst> List<T, Allocator>::common_iterator<isConst>::operator++(int) {
    List<T, Allocator>::common_iterator<isConst> ret = *this;
    adress = adress->next;
    return ret;
}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::template common_iterator<isConst>& List<T, Allocator>::common_iterator<isConst>::operator--() {
    adress = adress->prev;
    return *this;
}

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::template common_iterator<isConst> List<T, Allocator>::common_iterator<isConst>::operator--(int) {
    List<T, Allocator>::common_iterator<isConst> ret = *this;
    adress = adress->prev;
    return ret;
}

// *****
// List::Node

template<typename T, typename Allocator>
List<T, Allocator>::Node::Node(): next(this), prev(this) {}

template<typename T, typename Allocator>
List<T, Allocator>::Node::Node(const T& value): value(value), next(this), prev(this) {}

template<typename T, typename Allocator>
List<T, Allocator>::Node::Node(const List<T, Allocator>::Node& another): value(another.value), next(nullptr), prev(nullptr) {}
