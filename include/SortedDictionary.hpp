#ifndef __COLLECTIONS_SORTED_DICTIONARY
#define __COLLECTIONS_SORTED_DICTIONARY

#include <map>
#include <utility>
#include <vector>

namespace Collections
{
    // C = std::less<K> | std::greater<K> | o propietaria
    template <typename K, typename V, typename C>
    class SortedDictionary : std::map<K, V, C>
    {        

    public:
        void From(const SortedDictionary<K, V, C> &src)
        {            
            for (auto& [k,v] : src)
                this->operator[](k) = v;
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        inline V &operator[](const K &key)
        {
            return std::map<K, V, C>::operator[](key);
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        const inline V &operator[](const K &key) const
        {
            return std::map<K, V, C>::operator[](key);
        }

        typename SortedDictionary<K, V, C>::iterator begin()
        {
            return std::map<K, V, C>::begin();
        }

        typename SortedDictionary<K, V, C>::iterator end()
        {
            return std::map<K, V, C>::end();
        }

        typename SortedDictionary<K, V, C>::const_iterator begin() const
        {
            return std::map<K, V, C>::begin();
        }

        typename SortedDictionary<K, V, C>::const_iterator end() const
        {
            return std::map<K, V, C>::end();
        }

        inline std::vector<K> Keys()
        {            
            std::vector<K> result;
            for (auto kvp = this->begin(); kvp != this->end(); ++kvp)
                result.push_back(kvp->first);
            return result;
        }

        inline std::vector<V> Values()
        {            
            std::vector<V> result;
            for (auto kvp = this->begin(); kvp != this->end(); ++kvp)
                result.push_back(kvp->second);
            return result;
        }

        inline size_t Size() const
        {
            return std::map<K, V, C>::size();
        }

        inline bool Any() const
        {
            return !std::map<K, V, C>::empty();
        }

        inline void Clear()
        {
            return std::map<K, V, C>::clear();
        }

        const inline std::pair<K, V> First() const
        {
            auto kvp = this->begin();
            return std::make_pair(kvp->first, kvp->second);
        }

        const inline K& FirstKey() const
        {
            return this->begin()->first;
        }

        inline bool TryRemove(const K &key)
        {
            return std::map<K, V, C>::erase(key);
        }

        inline bool TryRemove(const K &key, V &value)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                value = result->second;
                std::map<K, V, C>::erase(key);
                return true;
            }
            return false;
        }

        inline bool TryRemove(const K &key, V *&value)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                value = &result->second;
                std::map<K, V, C>::erase(key);
                return true;
            }
            return false;
        }

        template <typename F>
        inline bool TryRemoveExec(const K &key, const F &action)
        {
            if (V value; this->TryRemove(key, value))
            {
                action(value);
                return true;
            }
            return false;
        }

        inline bool ContainsKey(const K &key)
        {
            return std::map<K, V, C>::find(key) != std::map<K, V, C>::end();
        }

        inline bool TryGetValue(const K &key, V &value)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                value = result->second;
                return true;
            }
            return false;
        }

        inline bool TryGetValue(const K &key, V *&value)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                value = &result->second;
                return true;
            }
            return false;
        }

        template <typename F>
        inline bool TryGetValue(const K &key, const F &action)
        {            
            if (V value; this->TryGetValue(key, value))
            {
                action(value);
                return true;
            }
            return false;
        }

        inline bool TryAdd(const K &key, const V &value)
        {
            // el emplace es mas efectivo pues no genera nueva copia
            return std::map<K, V, C>::try_emplace(key, value).second;
        }

        inline bool Add(const K &key, const V &value)
        {
            return std::map<K, V, C>::insert_or_assign(key, value).second;
        }

        template <typename F>
        inline V &GetOrAdd(const K &key, const F &add)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                return result->second;
            }
            
            return std::map<K, V, C>::operator[](key) = add();
            
        }

        inline V &GetOrAdd(const K &key)
        {            
            return this->GetOrAddNew(key);
        }

        inline V &GetOrAddNew(const K &key)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end())
            {
                return result->second;
            }
            
            // typename std::remove_pointer<V>::type pure_type;
            using pure_type = typename std::remove_pointer<V>::type;
            return this->operator[](key) = new pure_type();
            
        }

        template <typename F>
        inline V &GetOrAddOrNull(const K &key, const F &action)
        {            
            if (auto result = std::map<K, V, C>::find(key); result != std::map<K, V, C>::end() && result->second)
            {
                return result->second;
            }
            
            return this->operator[](key) = action();
            
        }

        // Restamos hasta que no haya nada, en cuyo caso eliminamos registro, notificamos cuando hay toque en la punta
        inline bool Sub(const K &key, const V &sub)
        {            
            return (this->operator[](key) -= sub) > 0 || this->TryRemove(key);
        }

    };
} // namespace Collections

#endif // __COLLECTIONS_SORTED_DICTIONARY
