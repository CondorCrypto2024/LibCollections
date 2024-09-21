#ifndef __COLLECTIONS_LIST
#define __COLLECTIONS_LIST

#include <algorithm>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Collections
{

    template <typename V>
    class List : protected std::vector<V>
    {
    public:
    
        List() = default ;
        List(const std::vector<V> &o) : std::vector<V>(o) {} ;

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        inline V &operator[](int i)
        {
            return std::vector<V>::operator[](i);
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        const inline V &operator[](int i) const
        {
            return std::vector<V>::operator[](i);
        }

        typename List<V>::iterator begin()
        {
            return std::vector<V>::begin();
        }

        typename List<V>::iterator end()
        {
            return std::vector<V>::end();
        }

        typename List<V>::const_iterator begin() const
        {
            return std::vector<V>::begin();
        }

        typename List<V>::const_iterator end() const
        {
            return std::vector<V>::end();
        }

        inline bool Any()
        {
            return !std::vector<V>::empty();
        }

        inline size_t Size()
        {
            return std::vector<V>::size();
        }

        inline void Clear()
        {
            return std::vector<V>::clear();
        }

        inline void Remove(int i)
        {
            std::vector<V>::erase(std::vector<V>::begin() + i);
        }

        inline void Add(const V &value)
        {
            this->Push(value);
        }

        inline void Push(const V &value)
        {
            std::vector<V>::emplace_back(value);
        }

        inline bool Contains(const V &t)
        {
            return std::find(std::vector<V>::begin(), std::vector<V>::end(), t) != std::vector<V>::end();
        }

        inline bool Pop(V& value)
        {
            if (!std::vector<V>::empty()) {
                value = std::vector<V>::back();
                std::vector<V>::pop_back();
                return true;
            } else {
                return false;
            }
        }

        inline std::optional<V> Pop()
        {                        
            V value;
            return this->Pop(value) ? std::optional<V>(value) : std::nullopt;
        }

        template <typename F>
        inline void Transform(const F& action)
        {
            std::transform(this->begin(), this->end(), this->begin(), action);
        }

        void FromVector(const std::vector<V> &vector)
        {
            this->Clear();
            for (const V &i : vector)
            {
                this->Push(i);
            }
        }
    };
} // namespace Collections

#endif // __COLLECTIONS_DICTIONARY
