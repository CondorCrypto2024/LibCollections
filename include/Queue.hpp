#ifndef __COLLECIONS_QUEUE
#define __COLLECIONS_QUEUE

#include <queue>
#include <optional>

namespace Collections
{
    template <typename T>
    class Queue : protected std::queue<T>
    {

        // ojo : containers always, always destroy their contained objects when they're removed from the container BUT  A pointer object does NOT have a destructo
        // https://stackoverflow.com/questions/2002282/c-stdqueuepop-calls-destructor-what-of-pointer-types

    public:
        
        // en std la std::queue no tiene iteradores
        inline int Size()
        {
            return std::queue<T>::size();
        }

        inline bool Any()
        {
            return !std::queue<T>::empty();
        }

        inline void Enqueue(const T &t)
        {
            std::queue<T>::emplace(t);
        }

        inline bool TryEnqueue(const T &t)
        {
            std::queue<T>::emplace(t);
            return false; // este método es más un dummy para poderlo meter en IF's
        }

        inline bool TryDequeue(T *&t)
        {
            if (!std::queue<T>::empty())
            {
                t = &std::queue<T>::front();
                std::queue<T>::pop();
                return true;
            }
            return false;
        }

        inline bool TryPeek(T &t)
        {
            if (!std::queue<T>::empty())
            {
                t = std::queue<T>::front();
                return true;
            }
            return false;
        }

        inline bool TryDequeue(T &t)
        {
            if (!std::queue<T>::empty())
            {
                t = std::queue<T>::front();
                std::queue<T>::pop();
                return true;
            }
            return false;
        }

        inline std::optional<T> Dequeue()
        {
            if (!std::queue<T>::empty())
            {
                auto t = std::queue<T>::front();
                std::queue<T>::pop();
                return std::optional<T>(t);
            }
            return std::nullopt;
        }

        template <typename F>
        inline bool TryPeek(const F &action)
        {
            if (!std::queue<T>::empty())
            {
                action(std::queue<T>::front());
                return true;
            }
            return false;
        }

        template <typename F>
        inline bool TryDequeue(const F &action)
        {
            if (!std::queue<T>::empty())
            {
                action(std::queue<T>::front());
                std::queue<T>::pop();
                return true;
            }
            return false;
        }

        template <typename F>
        inline void WhileTryDequeue(const F &action)
        {
            while (this->TryDequeue(action))
            {
            }
        }

        inline void Clear()
        {
            while (!std::queue<T>::empty())
                std::queue<T>::pop();
        }
    };

} // namespace Collections

#endif // __COLLECIONS_QUEUE
