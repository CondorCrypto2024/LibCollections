#ifndef __COLLECIONS_WAITED_QUEUE
#define __COLLECIONS_WAITED_QUEUE

#include <mutex>
#include <condition_variable>

using namespace std::literals::chrono_literals;
namespace Collections
{
    template <typename T>
    class WaitedQueue : private ConcurrentQueue<T>
    {
        std::mutex wait_mtx;
        std::unique_lock<std::mutex> wait_lck;
        std::condition_variable wait_event;

    public:
        WaitedQueue() : wait_lck(this->wait_mtx) {}
        virtual ~WaitedQueue() { this->wait_event.notify_one(); }

        void Enqueue(const T &t)
        {
            ConcurrentQueue<T>::Enqueue(t);
            this->wait_event.notify_one();
        }

        template <class Rep, class Period>
        bool TryDequeue(T *&t, std::chrono::duration<Rep, Period> const &timeout_duration)
        {
            this->wait_event.wait_for(this->wait_lck, timeout_duration);
            return ConcurrentQueue<T>::TryDequeue(t);
        }

        template <class Rep, class Period>
        bool TryDequeue(T &t, std::chrono::duration<Rep, Period> const &timeout_duration)
        {
            this->wait_event.wait_for(this->wait_lck, timeout_duration);
            return ConcurrentQueue<T>::TryDequeue(t);
        }

        template <typename F, class Rep, class Period>
        bool TryDequeue(const F &action, std::chrono::duration<Rep, Period> const &timeout_duration)
        {
            this->wait_event.wait_for(this->wait_lck, timeout_duration);
            return ConcurrentQueue<T>::TryDequeue(action);
        }

        template <typename F, class Rep, class Period>
        void WhileTryDequeue(const F &action, std::chrono::duration<Rep, Period> const &timeout_duration)
        {
            this->wait_event.wait_for(this->wait_lck, timeout_duration);
            ConcurrentQueue<T>::WhileTryDequeue(action);
        }
    };

} // namespace Collections

#endif // __COLLECIONS_WAITED_QUEUE
