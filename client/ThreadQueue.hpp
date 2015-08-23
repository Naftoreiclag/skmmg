#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <list>
#include <mutex>

namespace skm {

    template <typename T> class ThreadQueue {
    private:
        std::list<T> data;
        std::mutex mutex;
    public:

        void push_back(T data) {
            mutex.lock();
            data.push_back(data);
            mutex.unlock();
        }
        
        void clear() {
            mutex.lock();
            data.clear();
            mutex.unlock();
        }

        std::size_t size() {
            std::size_t size;
            mutex.lock();
            size = data.size();
            mutex.unlock();
            return size;
        }

        T* pop_front() {
            T* ptr;

            mutex.lock();
            if(data.size() > 0) {
                ptr = &data.front();
                data.pop_front();
            }
            else {
                ptr = nullptr;
            }

            mutex.unlock();

            return ptr;
        }
        

        ThreadQueue() {
        }
        virtual ~ThreadQueue() {
        }
    };

}

#endif // THREADQUEUE_H
