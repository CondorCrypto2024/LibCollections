#ifndef __COLLECTIONS_CONCURRENT_LIST
#define __COLLECTIONS_CONCURRENT_LIST

#include <chrono>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <utility>

namespace Collections
{

template <typename V>
class ConcurrentList : private List<V>
{    
    std::mutex mutex ;

public:
    // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
    inline V &operator[](int i)
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::operator[](i);
    }

    // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
    const inline V &operator[](int i) const
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::operator[](i);
    }

    typename List<V>::iterator begin()
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::begin();
    }

    typename List<V>::iterator end()
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::end();
    }

    typename List<V>::const_iterator begin() const
    {
        std::lock_guard<std::mutex> m(const_cast<std::mutex&>(this->mutex));
        return List<V>::begin();
    }

    typename List<V>::const_iterator end() const
    {
        std::lock_guard<std::mutex> m(const_cast<std::mutex&>(this->mutex));
        return List<V>::end();
    }
    
    inline bool Any()
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return !List<V>::empty();
    }

    inline size_t Size()
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::size();
    }

    inline void Clear()
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        return List<V>::clear();
    }

    inline void Remove(int i)
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        List<V>::erase(List<V>::begin()+i);
    }

    inline void Add(const V &value)
    {
        this->Push(value);
    }   
        
    inline void Push(const V &value)
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        List<V>::push_back(value);
    }
    
    inline void Pop(const V &value)
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        List<V>::pop_back(value);
    }

    template <typename F>
    inline void Transform(const F& action)
    {
        std::lock_guard<std::mutex> m(this->mutex); 
        std::transform( this->begin(), this->end(), this->begin(), action ) ;
    }

    void FromVector(const std::vector<V> &vector)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        List<V>::FromVector(vector);
    }
    
};
} // namespace Collections

#endif // __COLLECTIONS_DICTIONARY
