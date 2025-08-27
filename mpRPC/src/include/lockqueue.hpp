#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class LockQueue
{
public:
    void push(const T &data)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        mqueue_.push(data);
        mcv_.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while(mqueue_.empty())
        {
            mcv_.wait(lock);
        }
        T data = mqueue_.front();
        mqueue_.pop();
        return data;
    }

private:
    std::queue<T> mqueue_;
    std::mutex mtx_;
    std::condition_variable mcv_;
};