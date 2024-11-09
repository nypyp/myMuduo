#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
        const std::string &name) 
        : loop_(nullptr),
          exiting_(false),
          thread_(std::bind(&EventLoopThread::threadFunc, this), name),
          mutex_(),
          cond_(),
          callback_(cb) {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

/**
 * 阻塞同步：线程主动等待操作完成，并且被挂起直到操作完成。
 * 非阻塞同步：线程主动等待操作完成，但不会被挂起，而是通过轮询或其他机制检查操作是否完成。
 * 阻塞异步：不常见，因为异步操作本质上是非阻塞的。
 * 非阻塞异步：线程不会主动等待操作完成，而是继续执行其他任务，当操作完成时，通过回调函数或其他机制通知线程。
 * 在这个例子中，线程主动等待操作完成，判断是同步的，而线程被挂起，判断是阻塞的所以是同步阻塞的
 */
EventLoop* EventLoopThread::startLoop() {
    thread_.start();

    EventLoop* loop = nullptr;
    {
        // 作用域内声明即加锁，当运行到cond_.wait(lock)时自动释放锁，实现异步等待
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop == nullptr) {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (callback_) {
        callback_(&loop);
    }
    {
        //RAII声明即初始化，对loop_加锁，作用域结束调用析构函数解锁
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one(); // 条件变量用于通知正在等待的线程，实现线程间异步执行（如何说它是异步呢？
    }

    loop.loop();
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}