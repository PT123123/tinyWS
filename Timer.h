#ifndef TINYWS_TIMER_H
#define TINYWS_TIMER_H

#include <cstdint>

#include <functional>

#include "base/noncopyable.h"

namespace tinyWS {
    /**
     * 非线程安全，不暴露给用户，只向想用户提供 TdmerId 对象，用于识别定时器
     */
    class Timer : noncopyable {
    public:
        typedef std::function<void()> TimeCallback; // 定时器回调函数类型
        typedef int64_t TimeType; // 时间数据类型

        /**
         * 构造函数
         * @param cb 回调函数
         * @param timeout 到期时间
         * @param interval
         */
        explicit Timer(const TimeCallback &cb, TimeType timeout, TimeType interval = 0);

        /**
         * 执行回调函数
         */
        void run() const;

        /**
         * 获取到期时间
         * @return 到期时间
         */
        TimeType getExpiredTime();

        /**
         * 更新到期时间
         * @param timeout 新的到期时间
         */
        void updateExpiredTime(TimeType timeout);

        /**
         * 是否周期执行
         * @return true / false
         */
        bool repeat() const;

        /**
         * 是否有效
         * @return true / false
         */
        bool isVaild();

        /**
         * 返回无效时间
         * @return 0
         */
        Timer::TimeType invaild();

        /**
         * 重设定时器到期时间
         * 只有周期执行的定时器才能重设到期时间
         * 不是周期执行的定时器不能重设到期时间，如果调用此函数的定时器不是周期执行的定时器，其到期时间讲会设置为无效值（0）
         * @param now 新的到期时间
         */
        void restart(TimeType now);

        const static TimeType kMicroSecondsPerSecond = 1000 * 1000; // 一秒有 1000 * 1000 微秒

        /**
         * 获取当前时间距 1970年1月1日 00 : 00 : 00 的微秒数
         * @return 微秒
         */
        static TimeType now();

    private:
        const TimeCallback timeCallback_; // 定时器回调函数
        TimeType expiredTime_; // 到期时间
        const TimeType interval_; // 执行周期
        const bool repeat_; // 是否周期执行
    };
}

#endif //TINYWS_TIMER_H
