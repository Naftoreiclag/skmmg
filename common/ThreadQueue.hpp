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

        void push_back(T item) {
            std::lock_guard<std::mutex> lock(mutex);
            data.push_back(item);
        }
        
        void clear() {
            std::lock_guard<std::mutex> lock(mutex);
            data.clear();
        }

        std::size_t size() {
            std::lock_guard<std::mutex> lock(mutex);
            std::size_t size = data.size();
            return size;
        }

        T* pop_front() {
            std::lock_guard<std::mutex> lock(mutex);
            T* ptr;

            if(data.size() > 0) {
                ptr = &data.front();
                data.pop_front();
            }
            else {
                ptr = nullptr;
            }
            
            return ptr;
        }
        

        ThreadQueue() {
        }
        virtual ~ThreadQueue() {
        }
    };

}

#endif // THREADQUEUE_H
