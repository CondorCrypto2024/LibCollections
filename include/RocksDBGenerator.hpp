#include <coroutine>
#include <memory>
#include <rocksdb/db.h>

// en rhel7 nunca encontramos el rocksdb.rpm
#if __GNUC__ >= 12

namespace Collections
{
    template <typename K, typename V>
    class RocksDBGenerator
    {
    public:
    
        struct promise_type; // declaración
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            std::pair<K, V> currentPair;
            std::unique_ptr<rocksdb::Iterator> it;

            RocksDBGenerator get_return_object()
            {
                return RocksDBGenerator(handle_type::from_promise(*this));
            }
            std::suspend_always initial_suspend() { return {}; } // coroutine suspendida al abrir
            std::suspend_always final_suspend() noexcept { return {}; } // suspender coroutine al cerrar
            void unhandled_exception() { std::terminate(); } // terminas la coroutine en caso de error
            std::suspend_always yield_value(const std::pair<K, V> &keyValue)
            {
                currentPair = keyValue; // entregar el valor
                return {}; // suspender
            }
        };

        RocksDBGenerator(handle_type h) : coro(h) {}
        ~RocksDBGenerator()
        {
            if (coro)
                coro.destroy();
        }

        bool next()
        {
            if (!coro)
                return false;
            coro.resume();
            return !coro.done();
        }

        std::pair<K, V> value() const
        {
            return coro.promise().currentPair;
        }

    private:
        handle_type coro;
    };

}

#endif // #if __GNUC__ >= 12 && 