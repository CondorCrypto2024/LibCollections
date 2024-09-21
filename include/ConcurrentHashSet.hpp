#ifndef __COLLECIONS_CONCURRENT_HASHSET
#define __COLLECIONS_CONCURRENT_HASHSET

#include <mutex>
#include <unordered_set>
#include <utility>

#include "HashSet.hpp"

namespace Collections
{
template <typename T>
class ConcurrentHashSet : private HashSet<T>
{
    std::mutex mutex;

public:
    typename ConcurrentHashSet<T>::iterator begin()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::begin();
    }

    typename ConcurrentHashSet<T>::iterator end()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::end();
    }

    typename ConcurrentHashSet<T>::const_iterator begin() const
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::begin();
    }

    typename ConcurrentHashSet<T>::const_iterator end() const
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::end();
    }

    std::unordered_set<T> Clone()
    {            
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Clone();
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Size();
    }

    inline bool Any()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Any();
    }

    void Insert(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        HashSet<T>::Insert(t);
    }

    bool TryInsert(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::TryInsert(t);
    }

    bool Exists(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Exists(t);
    }

    bool Contains(const T& t) 
    {
        return this->Exists(t);
    }

    void Remove(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Remove(t);
    }

    bool TryRemove(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::TryRemove(t);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::Clear();
    }

    template <typename F>
    void ForEach(const F &action)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return HashSet<T>::ForEach( action ) ;        
    }
};

} // namespace Collections

#endif // __COLLECIONS_CONCURRENT_HASHSET
