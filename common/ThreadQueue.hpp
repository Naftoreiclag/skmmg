#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <queue>
#include <mutex>

namespace skm {

    template <typename T> class ThreadQueue {
    private:
        std::queue<T> data;
        std::mutex mutex;
    public:

        void push_back(T item) {
            std::lock_guard<std::mutex> lock(mutex);
            data.push(item);
        }
        
        void clear() {
            std::lock_guard<std::mutex> lock(mutex);
            data.clear();
        }

        std::size_t size() {
            std::lock_guard<std::mutex> lock(mutex);
            return data.size();
        }

        bool pop_front(T& popped) {
            std::lock_guard<std::mutex> lock(mutex);

            if(data.size() > 0) {
                popped = data.front();
                data.pop();
                return true;
            }
            else {
                return false;
            }
        }
        

        ThreadQueue() {
        }
        ~ThreadQueue() {
        }
    };

}

#endif // THREADQUEUE_H
