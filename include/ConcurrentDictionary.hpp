#ifndef __COLLECTIONS_CONCURRENT_DICTIONARY
#define __COLLECTIONS_CONCURRENT_DICTIONARY

#include <functional>
#include <mutex>
#include <unordered_map>
#include <utility>

#include "Dictionary.hpp"

namespace Collections
{

    template <typename K, typename V>
    class ConcurrentDictionary : private Dictionary<K, V>
    {
    public:
        // al publicar este mutex puedo sincronizar arbitrariamente (problema hunters)
        std::mutex mutex;

    public:
        ConcurrentDictionary() = default;
        ConcurrentDictionary(const std::unordered_map<K, V> &o) : Dictionary<K, V>(o){};
        ConcurrentDictionary(const Dictionary<K, V> &o) : Dictionary<K, V>(o){};

        void From(const ConcurrentDictionary<K, V> &src)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            Dictionary<K, V>::From(src);
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        V &operator[](const K &key)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::operator[](key);
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        const V &operator[](const K &key) const
        {
            std::lock_guard<std::mutex> m(const_cast<std::mutex&>(this->mutex));
            return Dictionary<K, V>::operator[](key);
        }

        typename ConcurrentDictionary<K, V>::iterator begin()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::begin();
        }

        typename ConcurrentDictionary<K, V>::iterator end()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::end();
        }

        typename ConcurrentDictionary<K, V>::const_iterator begin() const
        {
            std::lock_guard<std::mutex> m(const_cast<std::mutex&>(this->mutex));
            return Dictionary<K, V>::begin();
        }

        typename ConcurrentDictionary<K, V>::const_iterator end() const
        {
            std::lock_guard<std::mutex> m(const_cast<std::mutex&>(this->mutex));
            return Dictionary<K, V>::end();
        }

        inline std::vector<K> Keys()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Keys();
        }

        template <typename F>
        inline std::vector<K> Keys(const F &condition)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Keys(condition);
        }

        inline std::vector<V> Values()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Values();
        }

        inline size_t Size()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Size();
        }

        inline bool Any()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Any();
        }

        inline void Clear()
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Clear();
        }

        inline bool TryRemove(const K &key)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemove(key);
        }

        inline V& Incr(const K &key, const V &value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::operator[](key) += value ;
        }

        inline bool TryRemove(const K &key, V &value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemove(key, value);
        }

        inline bool TryRemove(const K &key, V *&value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemove(key, value);
        }

        inline bool TryRemoveIf(const K &key, const std::function<bool(V&)>& cond)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemoveIf(key, cond);
        }

        inline bool TryRemoveWhen(const K &key, const std::function<bool(V&)>& cond)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemoveWhen(key, cond);
        }

        template <typename F>
        inline bool TryRemoveExec(const K &key, const F &action)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryRemoveExec(key, action);
        }

        inline bool ContainsKey(const K &key)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::ContainsKey(key);
        }

        inline bool TryCheckValue(const K &key, const std::function<bool(V&)> &cond)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryCheckValue(key, cond);
        }

        inline bool TryGetValue(const K &key, V &value) 
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryGetValue(key, value);
        }

        inline bool TryGetValue(const K &key, V *&value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryGetValue(key, value);
        }

        template <typename F>
        inline bool TryGetValueExec(const K &key, const F &action)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryGetValueExec(key, action);
        }

        inline bool TryAdd(const K &key, const V &value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryAdd(key, value);
        }

        // nueva - para megahub
        inline bool TryAdd(const std::function<K()> &key, const V &value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryAdd(key(), value);
        }

        inline bool TryAdd(const K &key, const std::function<V()> &function)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::TryAdd(key, function);
        }

        inline bool Add(const K &key, const V &value)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::Add(key, value);
        }
                
        inline V &GetOrAdd(const K &key, const std::function<V()> &add)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::GetOrAdd(key, add);
        }

        inline V &GetOrAdd(const K &key)
        {            
            return this->GetOrAddNew(key);
        }

        inline V &GetOrAddNew(const K &key)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::GetOrAddNew(key);
        }
        
        inline V &GetOrAddOrNull(const K &key, const std::function<V()> &add)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::GetOrAddOrNull(key, add);
        }
                
        
        inline void AddOrUpdate(const K &key, const std::function<V()> &add, const std::function<void(V&)> &update)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            return Dictionary<K, V>::AddOrUpdate(key, add, update);
        }

        template <typename F>
        void Transform(const F &action)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            Dictionary<K, V>::Transform(action);
        }

        template <typename F>
        void ForEach(const F &action)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            Dictionary<K, V>::ForEach(action);
        }

        void FromMap(const std::unordered_map<K, V> &map)
        {
            std::lock_guard<std::mutex> m(this->mutex);
            Dictionary<K, V>::FromMap(map);
        }
    };
} // namespace Collections

#endif // __COLLECTIONS_CONCURRENT_DICTIONARY
