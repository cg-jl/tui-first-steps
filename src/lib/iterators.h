#if 0
#pragma once
#include <functional>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <vector>

namespace iterators2::backend {
template <typename T>
class base_iterator {
   public:
    // base iterator exits immediatly
    virtual std::optional<const T &> next() { return {}; }
};

template <typename T>
class Filter : public base_iterator<T> {
   public:
    using checkFn = typename std::function<bool(const T &)>;

   private:
    checkFn m_check;
    base_iterator<T> &m_it;

   public:
    Filter(base_iterator<T> &it, checkFn check) : m_check(check), m_it(it) {}
    virtual std::optional<const T &> next() {
        std::optional<const T &> opt;
        while ((opt = std::move(m_it.next())) && !m_check(*opt)) continue;
        return opt;
    }
};

template <typename T>
class FromVector : base_iterator<T> {
    using vec_iter = std::vector<T>::const_iterator;
    vec_iter m_start, m_end, m_current;

   public:
    FromVector(vec_iter start, vec_iter end)
        : m_start(start), m_end(end), m_current(start) {}
    virtual std::optional<const T &> next() {
        if (m_current == m_end) return {};
        const T &obj = *m_current;
        ++m_current;
        return obj;
    }
};

template <typename T, typename K>
class Map : public base_iterator<K> {
   public:
    using mapFn = std::function<K(const T &)>;

   private:
    mapFn m_fn;
    base_iterator<T> &m_it;

   public:
    Map(base_iterator<T> &it, mapFn fn) : m_it(it), m_fn(fn) {}
    virtual std::optional<const K &> next() {
        const auto &opt = m_it.next_mut();
        if (!opt) return {};
        return m_fn(*opt);
    }
};

template <typename T, typename K, typename lower_iterator>
class FilterMap : public base_iterator<K> {
   public:
    using fmapFn = std::function<std::optional<K>(const T &)>;

   private:
    fmapFn m_fn;
    base_iterator<T> &m_it;

   public:
    FilterMap(base_iterator<T> &it, fmapFn fn) : m_it(it), m_fn(fn) {}
    virtual std::optional<const K &> next() {
        std::optional<const K &> result;
        std::optional<const T &> input;
        while ((input = std::move(m_it.next())) &&
               (result = std::move(m_fn(*input))))
            continue;
        if (!input) return {};
        return result;
    }
};

}  // namespace iterators2::backend

#include <type_traits>
namespace iterators2 {
template <typename T, class iterator>
class Iterator : public backend::base_iterator<T> {
    backend::base_iterator<T> m_it;

   public:
    Iterator(const iterator &it) : m_it(it) {}
    Iterator(std::vector<T> &v)
        : m_it(backend::FromVector<T>(v.begin(), v.end())) {}
    virtual std::optional<const T &> next() { return m_it.next(); }
    template <typename K>
    Iterator<T, backend::Map<T, K>> map(typename backend::Map<T, K>::mapFn fn) {
        return Iterator(backend::Map<T, K>(*this, fn));
    }
    Iterator<T, backend::Filter<T>> filter(backend::Filter<T>::checkFn fn) {
        return Iterator(backend::Filter<T>(*this, fn));
    }
    template <typename K>
    Iterator<K, backend::FilterMap<T, K>> filter_map(
        typename backend::FilterMap<T, K>::fmapFn fn) {
        return Iterator(backend::FilterMap<T, K>(*this, fn));
    }

    void foreach (std::function<void(const T &)> cb) {
        std::optional<const T &> nxt;
        while ((nxt = std::move(next()))) cb(*nxt);
    }

    void foreach_while(std::function<bool(const T &)> cb) {
        std::optional<const T &> nxt;
        while ((nxt = std::move(next())) && cb(*nxt))
            ;
    }

    template <typename K>
    K reduce(K &init, std::function<const K &(K &, const T &)> reducer) {
        this->foreach ([&](const T &v) { init = std::move(reducer(init, v)); });
        return init;
    }

    std::vector<T> collect() {
        std::vector<T> values;
        this->foreach ([&](const T &v) { values.push_back(v); });
        return values;
    }
};

}  // namespace iterators2

namespace iterators::backend {

template <typename T>
class Filter : public std::vector<T>::iterator {
   public:
    using checkFn = std::function<bool(const T &)>;
    typedef std::vector<T>::iterator iterator;

   private:
    iterator m_iter;
    checkFn m_check;

   public:
    Filter(iterator iter, checkFn check) : m_iter(iter), m_check(check){};
    T &operator*() const { return *m_iter; }
    iterator &operator++() {
        auto &ret = *this;
        ++(*this);
        return ret;
    }
    iterator &operator++(int) {
        while (!m_check(*m_iter)) ++m_iter;
        return *this;
    }
    bool operator==(const Filter<T> &other) const {
        if (!m_check(*m_iter) && !other.m_check(*other.m_iter)) return true;
        return m_iter == other.m_iter;
    }
    bool operator!=(const Filter<T> &other) const { return !(*this == other); }
};
}  // namespace iterators::backend

namespace iterators {
template <typename T>
class filter {
   public:
    using iterator = typename backend::Filter<T>;
    using checkFn = typename backend::Filter<T>::checkFn;

   private:
    checkFn m_check;
    typename iterator::iterator m_start, m_end;

   public:
    filter(typename iterator::iterator begin, typename iterator::iterator end,
           checkFn check)
        : m_start(begin), m_end(end), m_check(check) {}
    iterator begin() const { return backend::Filter(m_start, m_check); }
    iterator end() const {
        return backend::Filter<T>(m_end, [](const T &f) { return false; });
    }
};
}  // namespace iterators
#endif