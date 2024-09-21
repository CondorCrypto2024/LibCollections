#define BOOST_TEST_MODULE TestCollections
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <thread>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

#include "Collections.hpp"
#include "Helpers.hpp"

BOOST_AUTO_TEST_SUITE(CollectionsSuite)

BOOST_AUTO_TEST_CASE(EmptyQueue)
{
    std::queue<int *> q;
    auto t = q.front(); // no truena
    q.pop();            // no truena pero t es basura
}

BOOST_AUTO_TEST_CASE(Incr)
{
    Collections::ConcurrentDictionary<int, int> dict_ints;

    dict_ints.TryAdd(0, 0);
    BOOST_CHECK_EQUAL(dict_ints.Incr(0, 1), 1);
    BOOST_CHECK_EQUAL(dict_ints[0], 1);

    BOOST_CHECK_EQUAL(dict_ints.Incr(0, -1), 0);
    BOOST_CHECK_EQUAL(dict_ints[0], 0);

    BOOST_CHECK_EQUAL(dict_ints[0], 0);
}

BOOST_AUTO_TEST_CASE(HashSetWitStrings)
{
    Collections::HashSet<std::string> hash_set;
    {
        std::string uno("uno");
        hash_set.Insert(uno);
    }

    {
        std::string uno("uno");
        BOOST_CHECK(hash_set.Exists(uno));
    }
}

BOOST_AUTO_TEST_CASE(NullPtrCollections)
{
    Collections::ConcurrentDictionary<std::string, std::string *> dict;

    auto uno = new std::string("uno");
    auto dos = new std::string("dos");
    auto tres = new std::string("tres");

    dict["1"] = uno;
    dict["2"] = dos;
    dict["3"] = tres;

    dict["*"] = tres;

    BOOST_CHECK(*dict["1"] == "uno");
    BOOST_CHECK(*dict["2"] == "dos");
    BOOST_CHECK(*dict["3"] == "tres");
    BOOST_CHECK(*dict["*"] == "tres");

    delete dict["1"];
    dict.operator[]("1") = nullptr;

    delete dict["2"];
    dict.operator[]("2") = nullptr;

    delete dict["3"];
    dict.operator[]("3") = nullptr;

    BOOST_CHECK(!dict["1"]);
    BOOST_CHECK(!dict["2"]);
    BOOST_CHECK(!dict["3"]);

    BOOST_CHECK(dict["*"]); // no se va a nulo !!!

    std::string *s;

    BOOST_CHECK(dict.TryGetValue("1", s));
    BOOST_CHECK(!s);

    BOOST_CHECK(dict.TryGetValue("2", s));
    BOOST_CHECK(!s);

    BOOST_CHECK(dict.TryGetValue("3", s));
    BOOST_CHECK(!s);

    BOOST_CHECK(dict.TryGetValue("*", s));
    BOOST_CHECK(s); // no es nulo !!!
}

BOOST_AUTO_TEST_CASE(TestSub)
{
    {
        Collections::SortedDictionary<int64_t, int32_t, std::greater<int64_t>> libro_compras;

        int64_t key = 1;
        BOOST_CHECK(!libro_compras.ContainsKey(key));
        libro_compras.TryAdd(key, 100);

        BOOST_CHECK(libro_compras.ContainsKey(key));
        libro_compras.Sub(key, 50);
        BOOST_CHECK(libro_compras.ContainsKey(key));
        libro_compras.Sub(key, 50);
        BOOST_CHECK(!libro_compras.ContainsKey(key));
        libro_compras.Sub(key, 50);
        BOOST_CHECK(!libro_compras.ContainsKey(key));

        key = 3;
        BOOST_CHECK(!libro_compras.ContainsKey(key));
        libro_compras.Sub(3, 250);
        BOOST_CHECK(!libro_compras.ContainsKey(key));
    }

    {
        Collections::SortedDictionary<int64_t, int32_t, std::less<int64_t>> libro_ventas;

        int64_t key = 2;
        BOOST_CHECK(!libro_ventas.ContainsKey(key));
        libro_ventas.TryAdd(key, 100);
        BOOST_CHECK(libro_ventas.ContainsKey(key));
        libro_ventas.Sub(key, 50);
        BOOST_CHECK(libro_ventas.ContainsKey(key));
        libro_ventas.Sub(key, 49);
        BOOST_CHECK(libro_ventas.ContainsKey(key));
        libro_ventas.Sub(key, 50);
        BOOST_CHECK(!libro_ventas.ContainsKey(key));
        key = 4;
        libro_ventas.Sub(key, 250);
        BOOST_CHECK(!libro_ventas.ContainsKey(key));
    }
}

BOOST_AUTO_TEST_CASE(NullPtrDictionary)
{
    class Test
    {
    public:
        int i;
    };

    Collections::ConcurrentDictionary<int, Test *> dict;

    dict[1] = new Test{1};
    dict[2] = new Test{2};
    dict[3] = new Test{3};

    BOOST_CHECK_EQUAL(dict[1]->i, 1);
    BOOST_CHECK_EQUAL(dict[2]->i, 2);
    BOOST_CHECK_EQUAL(dict[3]->i, 3);

    dict[4] = nullptr;
    BOOST_CHECK(dict[4] == nullptr);
    BOOST_CHECK_EQUAL(dict[4], nullptr);

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TestSortedDictionary)
{
    Collections::SortedDictionary<int64_t, uint64_t, std::greater<int64_t>> libro_compras;

    libro_compras[10L] += 10UL;
    libro_compras[11L] += 11UL;
    libro_compras[12L] += 12UL;
    libro_compras[13L] += 13UL;

    BOOST_CHECK_EQUAL(libro_compras[10L], 10UL);
    BOOST_CHECK_EQUAL(libro_compras[11L], 11UL);
    BOOST_CHECK_EQUAL(libro_compras[12L], 12UL);
    BOOST_CHECK_EQUAL(libro_compras[13L], 13UL);

    BOOST_CHECK(libro_compras.Sub(10L, 1UL));
    BOOST_CHECK_EQUAL(libro_compras[10L], 9UL);

    BOOST_CHECK(libro_compras.Sub(11L, 2UL));
    BOOST_CHECK_EQUAL(libro_compras[11L], 9UL);

    BOOST_CHECK(libro_compras.Sub(12L, 12UL));
    BOOST_CHECK(!libro_compras.ContainsKey(12L));

    BOOST_CHECK(libro_compras.Sub(13L, 999UL));
    // BOOST_CHECK(!libro_compras.ContainsKey(13L));

    // BOOST_CHECK(!libro_compras.Sub(14L, 999UL));
}

BOOST_AUTO_TEST_CASE(Test1)
{
    static const int NUM_THREADS = 8;
    static const int NUM_REGISTERS = 100'000;

    // si aquí usaramos diccionario no concurrente, causaríamos access violation por los threads
    auto dict3 = new Collections::ConcurrentDictionary<std::string, int>();

    auto now1 = boost::posix_time::microsec_clock::local_time();

    std::thread ThreadsDict[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        ThreadsDict[i] = std::thread([](auto dict, auto NumThread)
                                     {
                                         for (int i = NumThread * NUM_REGISTERS; i < (NumThread + 1) * NUM_REGISTERS; dict->TryAdd(std::to_string(i), i++))
                                         {
                                         }; },
                                     dict3, i);
    }

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        ThreadsDict[i].join();
    }

    auto now2 = boost::posix_time::microsec_clock::local_time();
    auto diff = (now2 - now1).total_milliseconds();

    BOOST_CHECK_EQUAL(dict3->Size(), NUM_THREADS * NUM_REGISTERS);
    // BOOST_CHECK_LT(diff, NUM_THREADS * NUM_REGISTERS / 500);

    delete dict3;
}

class Dog
{

public:
    std::string str_name;
    char *name;

    Dog() : Dog("Solovino"){};

    Dog(std::string str_name) : str_name(str_name)
    {
        this->name = &this->str_name[0];
        std::cout << "Dog " << this->name << " created\n";
    }

    Dog(const Dog &src)
    {
        *this = src;
        std::cout << "Dog " << this->name << " copied\n";
    }

    ~Dog()
    {
        std::cout << "Dog " << this->name << " destroyed\n";
    }
};

BOOST_AUTO_TEST_CASE(TestConstructorsDestructors)
{
    std::cout << "***************************************************" << std::endl;

    {
        Collections::ConcurrentQueue<Dog> q;
        q.Enqueue(Dog("Scooby"));

        Dog *d1;
        q.TryDequeue(d1);
        std::cout << "Finishing up " << d1->name << std::endl;
    }

    std::cout << std::endl;

    {
        Collections::ConcurrentQueue<Dog *> q;
        q.Enqueue(new Dog("Goofy"));

        Dog *d1;
        q.TryDequeue(d1);
        std::cout << "Finishing up " << d1->name << std::endl;
        delete d1;
    }

    {
        // valor extracción con apuntador (se destruye tres veces)
        Collections::ConcurrentDictionary<int, Dog> q;
        q.TryAdd(1, Dog("Scrappy"));

        Dog *d1;
        q.TryRemove(1, d1);
        std::cout << "Finishing up " << d1->name << std::endl;
    }

    {
        // valor extracción con directa (se destruye tres veces mas una creación de solovino)
        Collections::Dictionary<int, Dog> q;
        q.TryAdd(1, Dog("Turbo"));

        Dog d1;
        q.TryRemove(1, d1);
        std::cout << "Finishing up " << d1.name << std::endl;
    }

    {
        // apuntador extracción tambien a apuntador (la más eficiente, pero hay que eliminar nosotros)
        Collections::Dictionary<int, Dog *> q;
        q.TryAdd(1, new Dog("snoopy"));

        Dog *d1;
        q.TryRemove(1, d1);
        std::cout << "Finishing up " << d1->name << std::endl;
        delete d1;
    }

    std::cout << "***************************************************" << std::endl;

    BOOST_CHECK(true); // apoyo para detener en caso de pruebas
}

BOOST_AUTO_TEST_CASE(WhileTryDequeue)
{
    {
        Collections::Queue<int> q;
        q.Enqueue(100);
        q.Enqueue(200);
        q.Enqueue(300);

        int *suma = new int(0);
        int i;
        while (q.TryDequeue(i))
            *suma += i;
        BOOST_CHECK_EQUAL(*suma, 100 + 200 + 300);
    }

    {
        Collections::Queue<int> q;
        q.Enqueue(100);
        q.Enqueue(200);
        q.Enqueue(300);

        int *suma = new int(0);
        int i;
        q.WhileTryDequeue([suma](int i)
                          { *suma += i; });
        BOOST_CHECK_EQUAL(*suma, 100 + 200 + 300);
    }

    {
        Collections::Queue<int *> q;
        q.Enqueue(new int(100));
        q.Enqueue(new int(200));
        q.Enqueue(new int(300));

        int *suma = new int(0);
        int *i;
        while (q.TryDequeue(i))
            *suma += *i;
        BOOST_CHECK_EQUAL(*suma, 100 + 200 + 300);
    }

    {
        Collections::Queue<int *> q;
        q.Enqueue(new int(100));
        q.Enqueue(new int(200));
        q.Enqueue(new int(300));

        int *suma = new int(0);
        int *i;
        q.WhileTryDequeue([suma](int *i)
                          { *suma += *i; });
        BOOST_CHECK_EQUAL(*suma, 100 + 200 + 300);
    }
}

BOOST_AUTO_TEST_CASE(ListTest)
{
    Collections::ConcurrentList<std::string> l;
    l.Add("Uno");
    l.Add("Dos");
    l.Add("Tres");
    BOOST_CHECK_EQUAL(l.Size(), 3);

    l.Remove(1);
    BOOST_CHECK_EQUAL(l.Size(), 2);
}

BOOST_AUTO_TEST_CASE(KeyArrayDictTest)
{
    Collections::ConcurrentDictionary<std::string, int> dict;
    dict.TryAdd("ABCDE", 10);
    dict.TryAdd("ABCDF", 20);
    dict.TryAdd("ABCDG", 30);

    int suma = 0;
    for (auto i : dict)
        suma += i.second;

    BOOST_CHECK_EQUAL(suma, 10 + 20 + 30);
}

BOOST_AUTO_TEST_CASE(ListPushPop)
{
    Collections::List<int32_t> l;

    l.Push(1);
    l.Push(2);
    l.Push(3);

    BOOST_CHECK_EQUAL(l.Size(), 3);

    auto p1 = l.Pop().value();
    BOOST_CHECK_EQUAL(p1, 3);

    auto p2 = l.Pop().value();
    BOOST_CHECK_EQUAL(p2, 2);

    auto p3 = l.Pop().value();
    BOOST_CHECK_EQUAL(p3, 1);

    auto p4 = l.Pop().value_or(-1);
    BOOST_CHECK_EQUAL(p4, -1);
}

BOOST_AUTO_TEST_CASE(ListHashSetClone)
{
    Collections::HashSet<int32_t> l;
    l.Insert(1);
    l.Insert(2);
    l.Insert(3);

    Collections::HashSet<int32_t> m = l.Clone();
    BOOST_CHECK_EQUAL(m.Size(), l.Size());

    l.Insert(4);
    l.Insert(5);
    l.Insert(6);

    BOOST_CHECK_EQUAL(m.Size() + 3, l.Size());

    m.Insert(4);
    m.Insert(5);
    m.Insert(6);

    BOOST_CHECK_EQUAL(m.Size(), l.Size());
}

BOOST_AUTO_TEST_CASE(DictionaryGetOrAddNew)
{
    Collections::Dictionary<int32_t, std::string *> d;
    d.Add(1, new std::string("one"));
    d.Add(2, new std::string("two"));
    d.Add(3, new std::string("three"));

    d.GetOrAdd(4)->assign("four");
    d.GetOrAdd(5)->assign("five");
    d.GetOrAdd(6)->assign("six");

    BOOST_CHECK_EQUAL(d.Size(), 6);

    d.ForEach([](auto &k)
              { std::cerr << k.first << ' ' << *(k.second) << std::endl ; delete k.second ; });
}

// en rhel7 nunca encontramos el rocksdb.rpm
#if __GNUC__ >= 12

struct DataRecord
{
    enum class Tipo : uint8_t
    {
        Compra,
        Venta,
        Hecho
    };

    uint64_t timestamp;

    Tipo tipo;
    int64_t volumen;
    int64_t precio;

    double VPIN;
    double RSI;
    double OFI;
};

// Binario
BOOST_AUTO_TEST_CASE(TestRocksDBSimpleFile)
{

    rocksdb::DB *db;
    {
        rocksdb::Options options;
        options.create_if_missing = true;

        std::string path("/tmp/LibCollections.RocksDB.Test");
        auto status = rocksdb::DB::Open(options, path, &db);
        BOOST_CHECK(status.ok());

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

    // Create some data
    auto seq = 1234567890UL; // Example key
    DataRecord record_0{123456789UL, DataRecord::Tipo::Hecho, 123L, 123456789L, 3.1416, 3.1416, 3.1416};
    rocksdb::Slice key(reinterpret_cast<char *>(&seq), sizeof(seq));

    // Write data to the database
    {
        rocksdb::Slice value(reinterpret_cast<char *>(&record_0), sizeof(DataRecord));
        auto status = db->Put(rocksdb::WriteOptions(), key, value);
        BOOST_CHECK(status.ok());
    }

    // Read data from the database
    {
        rocksdb::PinnableSlice value;
        auto status = db->Get(rocksdb::ReadOptions(), db->DefaultColumnFamily(), key, &value);
        auto *record_1 = reinterpret_cast<const DataRecord *>(value.data());

        BOOST_CHECK(status.ok());
        BOOST_CHECK(memcmp(record_1, &record_0, sizeof(DataRecord)) == 0);
    }

    delete db;
}

BOOST_AUTO_TEST_CASE(TestRocksDictionary)
{
    {
        // sin nombre, para que lo deduzca autónomamente la biblioteca (usar con cuidado)
        Collections::RocksDBDictionary<int, DataRecord> rocksdb_dict;
        BOOST_CHECK(rocksdb_dict.Ok());
    }

    {
        Collections::RocksDBDictionary<int, DataRecord> rocksdb_dict("LibCollections.RocksDBDictionary.Test");
        BOOST_CHECK(rocksdb_dict.Ok());

        // ----------------------------------------------------------------------------------------------------

        rocksdb_dict.Clear();
        BOOST_CHECK(rocksdb_dict.Ok());

        // ----------------------------------------------------------------------------------------------------

        auto size = rocksdb_dict.Size();
        BOOST_CHECK(size == 0);
        BOOST_CHECK(rocksdb_dict.Ok());

        // ----------------------------------------------------------------------------------------------------

        auto any = rocksdb_dict.Any();
        BOOST_CHECK(!any);
        BOOST_CHECK(rocksdb_dict.Ok());

        // ----------------------------------------------------------------------------------------------------

        DataRecord record_0{0, DataRecord::Tipo::Hecho, 123L, 123456789L, 3.1416, 3.1416, 3.1416};
        DataRecord record_1{1, DataRecord::Tipo::Hecho, 456L, 987654321L, 3.1416, 3.1416, 3.1416};

        // ----------------------------------------------------------------------------------------------------
        // Add
        // ----------------------------------------------------------------------------------------------------

        {

            auto add0 = rocksdb_dict.TryAdd(0, record_0);
            BOOST_CHECK(add0);

            add0 = rocksdb_dict.TryAdd(0, record_0);
            BOOST_CHECK(!add0);

            auto contains0 = rocksdb_dict.ContainsKey(0);
            BOOST_CHECK(contains0);

            BOOST_CHECK(rocksdb_dict.Size() == 1);

            auto add1 = rocksdb_dict.Add(1, record_1);
            BOOST_CHECK(add1);

            add1 = rocksdb_dict.Add(1, record_1);
            BOOST_CHECK(add1);

            auto contains1 = rocksdb_dict.ContainsKey(1);
            BOOST_CHECK(contains1);

            BOOST_CHECK(rocksdb_dict.Size() == 2);
        }

        // ----------------------------------------------------------------------------------------------------
        // ContainsKey
        // ----------------------------------------------------------------------------------------------------

        {
            DataRecord remove_1;
            auto removed_ok_1 = rocksdb_dict.TryRemove(1, &remove_1);
            BOOST_CHECK(removed_ok_1);
            BOOST_CHECK(memcmp(&remove_1, &record_1, sizeof(DataRecord)) == 0);

            auto contains1 = rocksdb_dict.ContainsKey(1);
            BOOST_CHECK(!contains1);

            BOOST_CHECK(rocksdb_dict.Size() == 1);

            auto remove0 = rocksdb_dict.TryRemove(0);
            BOOST_CHECK(remove0);

            auto contains0 = rocksdb_dict.ContainsKey(0);
            BOOST_CHECK(!contains0);

            BOOST_CHECK(rocksdb_dict.Size() == 0);
        }

        // ----------------------------------------------------------------------------------------------------
        // TryRemove
        // ----------------------------------------------------------------------------------------------------

        {

            auto add0 = rocksdb_dict.TryAdd(0, record_0);
            BOOST_CHECK(add0);

            bool removido = false;
            bool removed_ok_0 = rocksdb_dict.TryRemove(0, [&removido](const auto &v)
                                                       { removido = true; });
            BOOST_CHECK(removed_ok_0);
            BOOST_CHECK(removido);

            removido = false;
            removed_ok_0 = rocksdb_dict.TryRemove(0, [&removido](const auto &v)
                                                  { removido = true; });
            BOOST_CHECK(!removed_ok_0);
            BOOST_CHECK(!removido);
        }

        // ----------------------------------------------------------------------------------------------------
        // TryGetValue
        // ----------------------------------------------------------------------------------------------------

        {

            BOOST_CHECK(rocksdb_dict.TryAdd(0, record_0));
            BOOST_CHECK(rocksdb_dict.TryAdd(1, record_1));

            BOOST_CHECK(rocksdb_dict.Size() == 2);

            DataRecord get_0;
            auto get_ok_0 = rocksdb_dict.TryGetValue(0, &get_0);
            BOOST_CHECK(memcmp(&get_0, &record_0, sizeof(DataRecord)) == 0);

            DataRecord get_1;
            auto get_ok_1 = rocksdb_dict.TryGetValue(1, &get_1);
            BOOST_CHECK(memcmp(&get_1, &record_1, sizeof(DataRecord)) == 0);

            BOOST_CHECK(rocksdb_dict.Size() == 2);

            bool gotten = false;
            bool removed_ok_0 = rocksdb_dict.TryGetValue(0, [&gotten](const auto &v)
                                                         { gotten = true; });
            BOOST_CHECK(removed_ok_0);
            BOOST_CHECK(gotten);

            gotten = true;
            removed_ok_0 = rocksdb_dict.TryGetValue(0, [&gotten](const auto &v)
                                                    { gotten = false; });
            BOOST_CHECK(removed_ok_0);
            BOOST_CHECK(!gotten);
        }

        // ----------------------------------------------------------------------------------------------------
        // Clear
        // ----------------------------------------------------------------------------------------------------

        {
            BOOST_CHECK(rocksdb_dict.Any());
            BOOST_CHECK(rocksdb_dict.Size() != 0);

            rocksdb_dict.Clear();
            BOOST_CHECK(rocksdb_dict.Ok());

            BOOST_CHECK(rocksdb_dict.Size() == 0);
            BOOST_CHECK(!rocksdb_dict.Any());
        }

        // ----------------------------------------------------------------------------------------------------
        // GetOrAdd
        // ----------------------------------------------------------------------------------------------------

        {

            auto valor_0 = rocksdb_dict.GetOrAdd(0, [&record_0]()
                                                 { return record_0; });
            BOOST_CHECK(memcmp(&valor_0, &record_0, sizeof(DataRecord)) == 0);

            DataRecord get_0;
            BOOST_CHECK(rocksdb_dict.TryGetValue(0, &get_0));
            BOOST_CHECK(memcmp(&get_0, &record_0, sizeof(DataRecord)) == 0);

            valor_0 = rocksdb_dict.GetOrAdd(0, [&record_1]()
                                            { return record_1; });
            BOOST_CHECK(memcmp(&valor_0, &record_0, sizeof(DataRecord)) == 0);

            auto valor_1 = rocksdb_dict.GetOrAddNew(1);

            DataRecord get_1;
            BOOST_CHECK(rocksdb_dict.TryGetValue(1, &get_1));
            BOOST_CHECK(memcmp(&get_1, &valor_1, sizeof(DataRecord)) == 0);
            BOOST_CHECK(!memcmp(&get_1, &valor_0, sizeof(DataRecord)) == 0);
        }

        // ----------------------------------------------------------------------------------------------------
        // ForEach
        // ----------------------------------------------------------------------------------------------------

        {

            BOOST_CHECK(rocksdb_dict.Any());
            rocksdb_dict.Clear();
            BOOST_CHECK(rocksdb_dict.Ok());
            BOOST_CHECK(rocksdb_dict.Size() == 0);

            BOOST_CHECK(rocksdb_dict.TryAdd(0, record_0));
            BOOST_CHECK(rocksdb_dict.TryAdd(1, record_1));

            // foreach
            {
                rocksdb_dict.ForEach([&](auto k, auto v)
                                     { BOOST_CHECK_EQUAL(k, v.timestamp); });
            }

            // to-vector
            {
                for (auto &[k, v] : rocksdb_dict.ToVector())
                {
                    BOOST_CHECK_EQUAL(k, v.timestamp);
                }
            }

            // coroutine (c++20)
            {
                auto gen = rocksdb_dict.Items();
                while (gen.next())
                {
                    auto [k, v] = gen.value();
                    BOOST_CHECK_EQUAL(k, v.timestamp);
                }
            }

            // Iterator (co-rutina propietaria)
            {
                Collections::RocksDBDictionary<int, DataRecord>::Iterator iterator(rocksdb_dict);
                int key;
                DataRecord record;
                while (iterator.TryNext(key, record))
                {
                    BOOST_CHECK_EQUAL(key, record.timestamp);
                }
            }

            // Transform
            {
                rocksdb_dict.Transform([](auto &k, auto &v)
                                       { 
                                        DataRecord n{v};
                                        n.timestamp++;                                        
                                        return n; });

                Collections::RocksDBDictionary<int, DataRecord>::Iterator iterator(rocksdb_dict);
                int key;
                DataRecord record;
                while (iterator.TryNext(key, record))
                {
                    BOOST_CHECK_EQUAL(key, record.timestamp - 1);
                }
            }
        }

        rocksdb_dict.Clear();
        BOOST_CHECK(rocksdb_dict.Ok());
    }
}

#endif // #if __GNUC__ >= 12 &&

BOOST_AUTO_TEST_SUITE_END()
