#ifndef __COLLECTIONS_DICTIONARY
#define __COLLECTIONS_DICTIONARY

#include <chrono>
#include <functional>
#include <vector>
#include <utility>
#include <unordered_map>

namespace Collections
{

    template <typename K, typename V>
    class Dictionary : protected std::unordered_map<K, V>
    {
    public:
        Dictionary() = default;
        Dictionary(const std::unordered_map<K, V> &o) : std::unordered_map<K, V>(o){};

        void From(const Dictionary<K, V> &src)
        {            
            *this = src;
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        inline V &operator[](const K &key)
        {
            return std::unordered_map<K, V>::operator[](key);
        }

        // el operador[] insertará el valor por default en primitivas en donde exista default, de lo contrario, usar GetOrAdd
        const inline V &operator[](const K &key) const
        {
            return const_cast<Collections::Dictionary<K, V>*>(this)->operator[](key);
        }

        typename Dictionary<K, V>::iterator begin()
        {
            return std::unordered_map<K, V>::begin();
        }

        typename Dictionary<K, V>::iterator end()
        {
            return std::unordered_map<K, V>::end();
        }

        typename Dictionary<K, V>::const_iterator begin() const
        {
            return std::unordered_map<K, V>::begin();
        }

        typename Dictionary<K, V>::const_iterator end() const
        {
            return std::unordered_map<K, V>::end();
        }

        inline std::vector<K> Keys()
        {
            std::vector<K> result;
            for (auto kvp = this->begin(); kvp != this->end(); ++kvp)
                result.push_back(kvp->first);
            return result;
        }

        template <typename F>
        inline std::vector<K> Keys(const F &condition)
        {
            std::vector<K> result;
            for (auto kvp = this->begin(); kvp != this->end(); ++kvp)
            {
                if( condition(kvp->first) )
                    result.push_back(kvp->first);
            }
            return result;
        }

        inline std::vector<V> Values()
        {
            std::vector<V> result;
            for (auto kvp = this->begin(); kvp != this->end(); ++kvp)
                result.push_back(kvp->second);
            return result;
        }

        inline bool Any()
        {
            return !std::unordered_map<K, V>::empty() ;
        }

        inline size_t Size()
        {
            return std::unordered_map<K, V>::size();
        }

        inline void Clear()
        {
            return std::unordered_map<K, V>::clear();
        }

        inline bool TryRemove(const K &key)
        {
            return std::unordered_map<K, V>::erase(key);
        }

        inline bool TryRemove(const K &key, V &value)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                value = result->second;
                std::unordered_map<K, V>::erase(key);
                return true;
            }
            return false;
        }

        inline bool TryRemove(const K &key, V *&value)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                value = &result->second;
                std::unordered_map<K, V>::erase(key);
                return true;
            }
            return false;
        }

        // regresa true cuando encontró la llave Y se dá la condición de borrado
        inline bool TryRemoveIf(const K &key, const std::function<bool(V &)> &cond)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end() && cond(result->second))
            {
                std::unordered_map<K, V>::erase(key);
                return true;
            }
            return false;
        }

        // similar a TryRemoveIf + Contains : regresa true si contiene la llave (después)
        inline bool TryRemoveWhen(const K &key, const std::function<bool(V &)> &cond)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                if (cond(result->second))
                {
                    std::unordered_map<K, V>::erase(key);
                }
                else
                {
                    return true; // true = aún existe la llave
                }
            }
            return false; // false = (ya) no existe la llave
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
            return std::unordered_map<K, V>::find(key) != std::unordered_map<K, V>::end();
        }

        inline bool TryGetValue(const K &key, V &value) 
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                value = result->second;
                return true;
            }
            return false;
        }

        // regresa true si tiene la llave & se dá la condincionante del valor obtenido en dicha llave
        inline bool TryCheckValue(const K &key, const std::function<bool(V &)> &cond)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                return cond(result->second);
            }
            return false;
        }

        // este nos funciona para cuando el valor es una clase, y no queremos estarla inicializando cada vez que pedimos algo
        // https://stackoverflow.com/questions/5286453/how-to-return-a-pointer-as-a-function-parameter
        inline bool TryGetValue(const K &key, V *&value)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                value = &result->second;
                return true;
            }
            return false;
        }

        template <typename F>
        inline bool TryGetValueExec(const K &key, const F &action)
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
            return std::unordered_map<K, V>::try_emplace(key, value).second;
        }

        inline bool TryAdd(const K &key, const std::function<V()> &function)
        {
            if (!this->ContainsKey(key))
            {
                this->Add(key, function());
                return true;
            }
            return false;
        }

        inline bool Add(const K &key, const V &value)
        {
            return std::unordered_map<K, V>::insert_or_assign(key, value).second;
        }
        
        inline V &GetOrAdd(const K &key, const std::function<V()> &add)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                return result->second;
            }
            
            return this->operator[](key) = add();
            
        }

        inline V &GetOrAdd(const K &key)
        {            
            return this->GetOrAddNew(key);
        }

        inline V &GetOrAddNew(const K &key)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end())
            {
                return result->second;
            }
            
            using pure_type = typename std::remove_pointer<V>::type;
            return this->operator[](key) = new pure_type();            
        }
        
        inline V &GetOrAddOrNull(const K &key, const std::function<V()> &add)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result != std::unordered_map<K, V>::end() && result->second)
            {
                return result->second;
            }
            
            return this->operator[](key) = add();
            
        }
        
        inline void AddOrUpdate(const K &key, const std::function<V()> &add, const std::function<void(V&)> &update)
        {            
            if (auto result = std::unordered_map<K, V>::find(key); result == std::unordered_map<K, V>::end())
            {                
                std::unordered_map<K, V>::emplace(key, add());
            }
            else
            {
                update(result->second);
            }
        }

        template <typename F>
        void Transform(const F &action)
        {
            std::transform(this->begin(), this->end(), this->begin(), action);
        }

        template <typename F>
        void ForEach(const F &action)
        {
            std::for_each(this->begin(), this->end(), action);
        }

        void FromMap(const std::unordered_map<K, V> &map)
        {
            this->Clear();
            for (const auto &[k, v] : map)
                this->operator[](k) = v;
        }
    };
} // namespace Collections

#endif // __COLLECTIONS_DICTIONARY
