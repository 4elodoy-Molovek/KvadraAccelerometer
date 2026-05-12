#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <optional>

namespace kvadra {
namespace server {

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() : active_(true) {}

    void push(T item) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(item));
        cv_.notify_one();
    }

    std::optional<T> pop(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx_);
        if (cv_.wait_for(lock, timeout, [this] { return !queue_.empty() || !active_; })) {
            if (!active_ && queue_.empty()) {
                return std::nullopt;
            }
            T item = std::move(queue_.front());
            queue_.pop();
            return item;
        }
        return std::nullopt;
    }

    void shutdown() {
        std::lock_guard<std::mutex> lock(mtx_);
        active_ = false;
        cv_.notify_all();
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mtx_);
        active_ = true;
        std::queue<T> empty;
        std::swap(queue_, empty);
    }

private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> active_;
};

} // namespace server
} // namespace kvadra