#ifndef JBNAVIGABLESET_H
#define JBNAVIGABLESET_H

#include <QSet>

#include <algorithm>
#include <set>

template <class _Kty, class _Pr = std::less<_Kty>, class _Alloc = std::allocator<_Kty>>
class JBNavigableSet : public std::set<_Kty, _Pr, _Alloc> {
public:
    ~JBNavigableSet() = default;

public:
    using _Mybase = std::set<_Kty, _Pr, _Alloc>;
    using key_type = _Kty;
    using key_compare = _Pr;
    using value_compare = typename _Mybase::value_compare;
    using value_type = typename _Mybase::value_type;
    using allocator_type = typename _Mybase::allocator_type;
    using size_type = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer = typename _Mybase::pointer;
    using const_pointer = typename _Mybase::const_pointer;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = typename _Mybase::iterator;
    using const_iterator = typename _Mybase::const_iterator;
    using reverse_iterator = typename _Mybase::reverse_iterator;
    using const_reverse_iterator = typename _Mybase::const_reverse_iterator;

    using _Alnode = typename _Mybase::_Alnode;
    using _Alnode_traits = typename _Mybase::_Alnode_traits;

    JBNavigableSet() : _Mybase(key_compare()) {
    }

    explicit JBNavigableSet(const allocator_type &_Al) : _Mybase(key_compare(), _Al) {
    }

    JBNavigableSet(const _Mybase &_Right)
        : _Mybase(_Right, _Alnode_traits::select_on_container_copy_construction(_Right._Getal())) {
    }

    JBNavigableSet(const _Mybase &_Right, const allocator_type &_Al) : _Mybase(_Right, _Al) {
    }

    explicit JBNavigableSet(const key_compare &_Pred) : _Mybase(_Pred) {
    }

    JBNavigableSet(const key_compare &_Pred, const allocator_type &_Al) : _Mybase(_Pred, _Al) {
    }

    template <class _Iter>
    JBNavigableSet(_Iter _First, _Iter _Last) : _Mybase(key_compare()) {
        this->insert(_First, _Last);
    }

    template <class _Iter>
    JBNavigableSet(_Iter _First, _Iter _Last, const key_compare &_Pred) : _Mybase(_Pred) {
        this->insert(_First, _Last);
    }

    template <class _Iter>
    JBNavigableSet(_Iter _First, _Iter _Last, const allocator_type &_Al)
        : _Mybase(key_compare(), _Al) {
        this->insert(_First, _Last);
    }

    template <class _Iter>
    JBNavigableSet(_Iter _First, _Iter _Last, const key_compare &_Pred, const allocator_type &_Al)
        : _Mybase(_Pred, _Al) {
        this->insert(_First, _Last);
    }

    JBNavigableSet &operator=(const _Mybase &_Right) {
        _Mybase::operator=(_Right);
        return *this;
    }

    JBNavigableSet(_Mybase &&_Right) : _Mybase(_STD move(_Right)) {
    }

    JBNavigableSet(_Mybase &&_Right, const allocator_type &_Al) : _Mybase(_STD move(_Right), _Al) {
    }

    JBNavigableSet &operator=(_Mybase &&_Right) noexcept(
        _Alnode_traits::is_always_equal::value &&is_nothrow_move_assignable_v<_Pr>) {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    void swap(_Mybase &_Right) noexcept(noexcept(_Mybase::swap(_Right))) {
        _Mybase::swap(_Right);
    }

    JBNavigableSet(std::initializer_list<value_type> _Ilist) : _Mybase(key_compare()) {
        this->insert(_Ilist);
    }

    JBNavigableSet(std::initializer_list<value_type> _Ilist, const key_compare &_Pred)
        : _Mybase(_Pred) {
        this->insert(_Ilist);
    }

    JBNavigableSet(std::initializer_list<value_type> _Ilist, const allocator_type &_Al)
        : _Mybase(key_compare(), _Al) {
        this->insert(_Ilist);
    }

    JBNavigableSet(std::initializer_list<value_type> _Ilist, const key_compare &_Pred,
                   const allocator_type &_Al)
        : _Mybase(_Pred, _Al) {
        this->insert(_Ilist);
    }

    JBNavigableSet &operator=(std::initializer_list<value_type> _Ilist) {
        this->clear();
        this->insert(_Ilist);
        return *this;
    }

public:
    typedef _Kty T;

    JBNavigableSet(const QSet<T> &set) {
        for (auto it = set.begin(); it != set.end(); ++it) {
            this->insert(*it);
        }
    }

    JBNavigableSet(QSet<T> &&set) {
        for (auto it = set.begin(); it != set.end(); ++it) {
            this->insert(*it);
        }
    }

    JBNavigableSet &operator=(const QSet<T> &set) {
        for (auto it = set.begin(); it != set.end(); ++it) {
            this->insert(*it);
        }
        return *this;
    }

    JBNavigableSet &operator=(QSet<T> &&set) {
        for (auto it = set.begin(); it != set.end(); ++it) {
            this->insert(*it);
        }
        return *this;
    }

    iterator lower(T key) {
        if (this->empty()) {
            return _Mybase::end();
        }
        auto it = ceiling(key);
        return --it;
    }

    const_iterator lower(T key) const {
        if (this->empty()) {
            return _Mybase::end();
        }
        auto it = ceiling(key);
        return --it;
    }

    iterator floor(T key) {
        if (this->empty()) {
            return _Mybase::end();
        }
        auto it = ceiling(key);
        if (it != this->end() && *it == key) {
            return it;
        }
        return --it;
    }

    const_iterator floor(T key) const {
        if (this->empty()) {
            return _Mybase::end();
        }
        auto it = ceiling(key);
        if (it != this->end() && *it == key) {
            return it;
        }
        return --it;
    }

    iterator ceiling(T key) {
        return _Mybase::lower_bound(key);
    }

    const_iterator ceiling(T key) const {
        return _Mybase::lower_bound(key);
    }

    iterator higher(T key) {
        return _Mybase::upper_bound(key);
    }

    const_iterator higher(T key) const {
        return _Mybase::upper_bound(key);
    }

    bool contains(T key) const {
        return this->find(key) != this->end();
    }

    bool isEmpty() const {
        return this->empty();
    }

    void remove(T key) {
        this->erase(key);
    }

    QSet<T> toQSet() const {
        QSet<T> res;
        for (auto it = this->begin(); it != this->end(); ++it) {
            res.insert(*it);
        }
        return res;
    }

    QList<T> toQList() const {
        QList<T> res;
        for (auto it = this->begin(); it != this->end(); ++it) {
            res.append(*it);
        }
        return res;
    }

    JBNavigableSet &unite(const JBNavigableSet &other) {
        for (const T &e : other) {
            this->insert(e);
        }
        return *this;
    }

    JBNavigableSet &intersect(const JBNavigableSet &other) {
        JBNavigableSet copy1;
        JBNavigableSet copy2;
        if (size() <= other.size()) {
            copy1 = *this;
            copy2 = other;
        } else {
            copy1 = other;
            copy2 = *this;
            *this = copy1;
        }
        for (const auto &e : qAsConst(copy1)) {
            if (!copy2.contains(e)) {
                this->remove(e);
            }
        }
        return *this;
    }

    bool intersects(const JBNavigableSet<T> &other) const {
        const bool otherIsBigger = other.size() > size();
        const JBNavigableSet &smallestSet = otherIsBigger ? *this : other;
        const JBNavigableSet &biggestSet = otherIsBigger ? other : *this;

        typename JBNavigableSet::const_iterator i = smallestSet.cbegin();
        typename JBNavigableSet::const_iterator e = smallestSet.cend();

        while (i != e) {
            if (biggestSet.contains(*i)) {
                return true;
            }
            ++i;
        }

        return false;
    }

    JBNavigableSet<T> &subtract(const JBNavigableSet<T> &other) {
        for (const auto &e : other) {
            remove(e);
        }
        return *this;
    }
};

#endif // JBNAVIGABLESET_H
