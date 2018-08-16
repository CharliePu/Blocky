#include <mutex>
#include <shared_mutex>
#include <functional>

// template wrapper allowing only single-threaded writing / multithreaded reading
template <typename T>
class ThreadSafeWrapper
{
public:
    ThreadSafeWrapper(const T &var = T()): mutex(), var(var) { }

    T &operator*()
    {
        return var;
    }

    T *operator->()
    {
        return &var;
    }

    void set(const T &v)
    {
        operateWriting([&]
        {
            var = v;
        });
    }

    T get() const
    {
        T temp;
        operateReading([&]
        {
			temp = var;
        }); 
		return temp;
    }

    //only one thread can access the value at the same time
    template <typename R>
    R operateWriting(const std::function<R()> &f) const
    {
        std::unique_lock<std::shared_mutex> lock(mutex);
        return f();
    }

    void operateWriting(const std::function<void()> &f) const
    {
        std::unique_lock<std::shared_mutex> lock(mutex);
        f();
    }

    //multiple threads can access the value at the same time
    template <typename R>
    R operateReading(const std::function<R()> &f) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return f();
    }

    void operateReading(const std::function<void()> &f) const
    {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return f();
    }
    

private:
    mutable std::shared_mutex mutex;
    T var;
};