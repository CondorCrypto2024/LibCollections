#ifndef __COLLECTIONS_ROCKS_DICTIONARY
#define __COLLECTIONS_ROCKS_DICTIONARY

// en rhel7 nunca encontramos el rocksdb.rpm
#if __GNUC__ >= 12

#include <filesystem>
#include <iostream>
#include <optional>
#include <regex>
#include <utility>
#include <vector>

#include <rocksdb/db.h>

#include "RocksDBGenerator.hpp"

namespace Collections
{
    template <typename K, typename V>
    class RocksDBDictionary
    {
        static_assert(std::is_standard_layout<K>::value, "K must be of standard layout");
        static_assert(std::is_trivial<K>::value, "K must be trivial");

        static_assert(std::is_standard_layout<V>::value, "V must be of standard layout");
        static_assert(std::is_trivial<V>::value, "V must be trivial");

        rocksdb::DB *db;
        rocksdb::Status last_status;

        class RocksDBComparator : public rocksdb::Comparator
        {
            int Compare(const rocksdb::Slice &a, const rocksdb::Slice &b) const override
            {
                K ai = *(reinterpret_cast<const K *>(a.data())), bi = *(reinterpret_cast<const K *>(b.data()));
                return (ai < bi) ? -1 : (ai > bi);
            }

            const char *Name() const override
            {
                return "RocksDBComparator<K>";
            }

            // No-op. For simple numeric types, this might be sufficient.
            void FindShortestSeparator(std::string *start, const rocksdb::Slice &limit) const override {}

            // No-op. For simple numeric types, this might be sufficient.
            void FindShortSuccessor(std::string *key) const override {}
        } comparator;

        inline bool ContainsKey(const rocksdb::Slice &db_key) const
        {
            std::string db_value;
            return this->db->KeyMayExist(rocksdb::ReadOptions(), db_key, &db_value);
        }

    public:
        RocksDBDictionary(const std::string &ruta, const std::string &nombre)
        {
            rocksdb::Options options;
            options.create_if_missing = true;
            options.comparator = &(this->comparator);

            auto path = std::filesystem::path(ruta + nombre);
            if (path.has_parent_path())
                std::filesystem::create_directories(path.parent_path());

            this->last_status = rocksdb::DB::Open(options, path, &this->db);
            assert(this->last_status.ok());

            // permisos 77x en archivos de este tipo, de lo contrario no permite multiples desarrolladores compilando
            try
            {
                std::filesystem::permissions(path, std::filesystem::perms::owner_all | std::filesystem::perms::group_all);
            }
            catch (const std::exception &e)
            {                
            }

            for (auto &p : std::filesystem::recursive_directory_iterator(path))
            {
                try
                {
                    std::filesystem::permissions(p, std::filesystem::perms::owner_all | std::filesystem::perms::group_all);
                }
                catch (const std::exception &e)
                {                    
                }
            }
        }

        // // constructor por default con ruta común predefinida
        // #ifdef NDEBUG
        //         RocksDBDictionary(const std::string &nombre) : RocksDBDictionary("/tmp/RocksDBDictionary/{username}/release/", nombre)
        //         {
        //         }
        // #else
        //         RocksDBDictionary(const std::string &nombre) : RocksDBDictionary("/tmp/RocksDBDictionary/{username}/debug/", nombre)
        //         {
        //         }
        // #endif

        RocksDBDictionary(const std::string &nombre) : RocksDBDictionary("/tmp/RocksDBDictionary/", nombre)
        {
        }

        // sin nombre, para que lo deduzca autónomamente la biblioteca (usar con cuidado)
        RocksDBDictionary() : RocksDBDictionary(__PRETTY_FUNCTION__) {}

        ~RocksDBDictionary()
        {
            this->db->Close();
            delete this->db;
        }

        bool Ok() const
        {
            return this->last_status.ok();
        }

        std::string Error() const
        {
            return this->last_status.ToString();
        }

        // -------------------------------------------------------------------------------------------------------
        // iterators
        // -------------------------------------------------------------------------------------------------------

        // c+20 coroutine
        RocksDBGenerator<K, V> Items() const
        {
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                auto sk = it->key(), sv = it->value();
                co_yield {*(reinterpret_cast<const K *>(sk.data())), *(reinterpret_cast<const V *>(sv.data()))};
            }
        }

        // to std::pair<K,V> vector
        inline std::vector<std::pair<const K, const V>> ToVector()
        {
            std::vector<std::pair<const K, const V>> result;
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                auto sk = it->key(), sv = it->value();
                result.push_back(std::make_pair(*(reinterpret_cast<const K *>(sk.data())), *(reinterpret_cast<const V *>(sv.data()))));
            }
            return result;
        }

        // to std::pair<K,V> vector
        inline SortedDictionary<K, V, std::less<K>> ToSortedDictionary()
        {
            Collections::SortedDictionary<K, V, std::less<K>> result;
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                auto sk = it->key(), sv = it->value();
                result.TryAdd(*(reinterpret_cast<const K *>(sk.data())), *(reinterpret_cast<const V *>(sv.data())));
            }
            return result;
        }

        // standard foreach
        inline void ForEach(const std::function<void(const K &, const V &)> &action) const
        {
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                auto sk = it->key(), sv = it->value();
                action(*(reinterpret_cast<const K *>(sk.data())), *(reinterpret_cast<const V *>(sv.data())));
            }
        }

        // transforma los valores del cada kvp conforme lo indique function y lo guarda de nuevo (por eso el valor no puede ser const)
        inline void Transform(const std::function<V(const K &, const V &)> &function)
        {
            this->ForEach(
                [this, &function](const auto &k, const auto &v)
                {
                    this->Set(k, function(k, v));
                });
        }

        // standard foreach
        inline std::optional<std::pair<K, V>> First() const
        {
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
            {
                auto sk = it->key(), sv = it->value();
                return std::optional<std::pair<K, V>>{std::make_pair(*(reinterpret_cast<const K *>(sk.data())), *(reinterpret_cast<const V *>(sv.data())))};
            }
            return std::nullopt;
        }

        // proprietary simple iterator
        class Iterator
        {
            const Collections::RocksDBDictionary<K, V> *db;
            std::unique_ptr<rocksdb::Iterator> it;

        public:
            Iterator(const Collections::RocksDBDictionary<K, V> &pdb)
                : db(&pdb), it(db->db->NewIterator(rocksdb::ReadOptions()))
            {
                this->it->SeekToFirst();
            }

            bool TryNext(K &k, V &v)
            {
                if (this->it->Valid())
                {
                    k = *(reinterpret_cast<const K *>(this->it->key().data()));
                    v = *(reinterpret_cast<const V *>(this->it->value().data()));

                    it->Next();
                    return true;
                }
                return false;
            }

            bool TryNext(std::pair<K, V> &record)
            {
                return this->TryNext(record.first, record.second);
            }
        };

        // -------------------------------------------------------------------------------------------------------

        inline size_t Size() const
        {
            size_t result = 0;
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
                result++;
            return result;
        }

        inline bool Any() const
        {
            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            it->SeekToFirst();
            return it->Valid();
        }

        inline bool Clear()
        {
            rocksdb::WriteBatch batch;

            auto it = std::unique_ptr<rocksdb::Iterator>(this->db->NewIterator(rocksdb::ReadOptions()));
            for (it->SeekToFirst(); it->Valid(); it->Next())
                batch.Delete(it->key());

            this->last_status = this->db->Write(rocksdb::WriteOptions(), &batch);
            return this->last_status.ok();
        }

        // -------------------------------------------------------------------------------------------------------

        inline bool ContainsKey(const K &key) const
        {
            rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K));
            return this->ContainsKey(db_key);
        }

        // -------------------------------------------------------------------------------------------------------
        inline bool Set(const K &key, const V &value)
        {
            return this->Add(key, value);
        }

        inline bool Add(const K &key, const V &value)
        {
            rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K));
            rocksdb::Slice db_value(reinterpret_cast<const char *>(&value), sizeof(V));
            this->last_status = this->db->Put(rocksdb::WriteOptions(), db_key, db_value);
            return this->last_status.ok();
        }

        inline bool TryAdd(const K &key, const V &value)
        {
            if (rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K)); !this->ContainsKey(db_key))
            {
                rocksdb::Slice db_value(reinterpret_cast<const char *>(&value), sizeof(V));
                this->last_status = this->db->Put(rocksdb::WriteOptions(), db_key, db_value);
                return this->last_status.ok();
            }
            return false;
        }

        // -------------------------------------------------------------------------------------------------------

        inline bool TryRemove(const K &key, V *value)
        {
            rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K));
            rocksdb::PinnableSlice db_value;

            this->last_status = this->db->Get(rocksdb::ReadOptions(), this->db->DefaultColumnFamily(), db_key, &db_value);
            if (!this->last_status.IsNotFound())
            {
                memcpy(value, db_value.data(), sizeof(V));
                this->last_status = this->db->Delete(rocksdb::WriteOptions(), db_key);
                return this->last_status.ok();
            }

            return false;
        }

        inline bool TryRemove(const K &key)
        {
            rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K));
            this->last_status = this->db->Delete(rocksdb::WriteOptions(), db_key);
            return this->last_status.ok();
        }

        inline bool TryRemove(const K &key, const std::function<void(const V &)> &action)
        {
            if (V value; this->TryRemove(key, &value))
            {
                action(value);
                return true;
            }
            return false;
        }

        // -------------------------------------------------------------------------------------------------------

        inline bool TryGetValue(const K &key, V *value)
        {
            rocksdb::Slice db_key(reinterpret_cast<const char *>(&key), sizeof(K));
            rocksdb::PinnableSlice db_value;
            auto ColumnFamily = this->db->DefaultColumnFamily();
            auto Options = rocksdb::ReadOptions();
            this->last_status = this->db->Get(Options, ColumnFamily, db_key, &db_value);
            if (!this->last_status.IsNotFound())
            {
                memcpy(value, db_value.data(), sizeof(V));
                return true;
            }
            return false;
        }

        inline bool TryGetValue(const K &key, const std::function<void(const V &)> &action)
        {
            if (V value; this->TryGetValue(key, &value))
            {
                action(value);
                return true;
            }
            return false;
        }

        // -------------------------------------------------------------------------------------------------------

        inline V GetOrAdd(const K &key, const std::function<V()> &action)
        {
            V value;
            if (!this->TryGetValue(key, &value))
            {
                value = action();
                this->TryAdd(key, value);
            }
            return value;
        }

        inline V GetOrAddNew(const K &key)
        {
            V value;
            if (!this->TryGetValue(key, &value))
            {
                value = {};
                this->TryAdd(key, value);
            }
            return value;
        }
    };

} // namespace Collections

#endif // __COLLECTIONS_ROCKS_DICTIONARY

#endif // #if __GNUC__ >= 12 &&