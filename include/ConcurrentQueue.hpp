#ifndef __COLLECIONS_CONCURRENT_QUEUE
#define __COLLECIONS_CONCURRENT_QUEUE

#include <mutex>

namespace Collections
{

template <typename T>
class ConcurrentQueue : private Queue<T>
{
    std::mutex mutex;

public:

    // en std la std::queue no tiene iteradores

    inline int Size()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::Size();
    }

    inline bool Any()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::Any();
    }

    void Enqueue(const T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        Queue<T>::Enqueue(t);
    }

    bool TryDequeue(T *&t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::TryDequeue(t);
    }

    bool TryDequeue(T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::TryDequeue(t);
    }

    bool TryPeek(T *&t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::TryPeek(t);
    }

    bool TryPeek(T &t)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::TryPeek(t);
    }

    template <typename F>
    bool TryDequeue(const F& action)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::TryDequeue(action);
    }

    template <typename F>
    void WhileTryDequeue(const F& action)
    {
        std::lock_guard<std::mutex> m(this->mutex);
        Queue<T>::WhileTryDequeue(action);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> m(this->mutex);
        return Queue<T>::Clear();
    }
};

} // namespace Collections

#endif // __COLLECIONS_CONCURRENT_QUEUE
