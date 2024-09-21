#ifndef __COLLECIONS_HASHSET
#define __COLLECIONS_HASHSET

#include <algorithm>
#include <mutex>
#include <utility>
#include <unordered_set>

namespace Collections
{
    template <typename T>
    class HashSet : protected std::unordered_set<T>
    {
    public:
        HashSet() = default;
        HashSet(const std::unordered_set<T> &o) : std::unordered_set<T>(o){};

        typename HashSet<T>::iterator begin()
        {
            return std::unordered_set<T>::begin();
        }

        typename HashSet<T>::iterator end()
        {
            return std::unordered_set<T>::end();
        }

        typename HashSet<T>::const_iterator begin() const
        {
            return std::unordered_set<T>::begin();
        }

        typename HashSet<T>::const_iterator end() const
        {
            return std::unordered_set<T>::end();
        }

        inline size_t Size()
        {
            return std::unordered_set<T>::size();
        }

        inline bool Any()
        {
            return !std::unordered_set<T>::empty();
        }

        std::unordered_set<T> Clone()
        {            
            std::unordered_set<T> cloned_set;
            std::for_each(this->begin(), this->end(), [&cloned_set](T e) { cloned_set.insert(e); });
            return cloned_set;
        }

        inline void Insert(const T &t)
        {
            std::unordered_set<T>::emplace(t);
        }

        inline bool TryInsert(const T &t)
        {
            return std::unordered_set<T>::insert(t).second;
        }

        inline bool Exists(const T &t)
        {
            return std::unordered_set<T>::find(t) != std::unordered_set<T>::end();
        }

        inline bool Contains(const T &t)
        {
            return this->Exists(t);
        }

        inline void Remove(const T &t)
        {
            std::unordered_set<T>::erase(t);
        }

        inline bool TryRemove(const T &t)
        {
            return std::unordered_set<T>::erase(t) > 0;
        }

        inline void Clear()
        {
            std::unordered_set<T>::clear();
        }

        template <typename F>
        void ForEach(const F &action)
        {
            std::for_each(this->begin(), this->end(), action);
        }
    };

}

#endif // __COLLECIONS_HASHSET
