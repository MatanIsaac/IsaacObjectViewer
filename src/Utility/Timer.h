// The Timer class allows you to create and manage timers with precise control over
// starting, stopping, pausing, resuming, and checking the elapsed time. It is designed
// for single-threaded applications and can be used to execute a callback function when
// a timer interval elapses.
//
// Features:
// - Start, stop, pause, and resume timers.
// - Retrieve the elapsed time in seconds and milliseconds.
// - Set a timer to execute a callback function after a specified interval.
//
// Example Usage:
//
// @code
// venture::Timer myTimer;
// myTimer.SetTimer(5, []() {
//     std::cout << "Timer callback called!" << std::endl;
// });
// // Additional tasks can be performed here while the timer runs in the background.
// std::this_thread::sleep_for(std::chrono::seconds(10)); // Wait for the timer to finish.
// @endcode
//
// @note It is common to use this class within an application loop for tasks like handling
// user input, updating game logic, and rendering, allowing precise control over timed events.
// This class is intended for use in single-threaded applications.

#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

namespace isaacGraphicsEngine
{
    /**
     * @brief A Simple, Versatile <chrono> based Timer utility class for managing time intervals and
     * callbacks.
     */
    class Timer
    {
        using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;
        using TimerCallback = std::function<void(void)>;
        using Interval = std::chrono::seconds;

    public:
        /**
         * @brief Constructor to initialize a Timer instance.
         */
        Timer() = default;

        /**
         * @brief Destructor to clean up the Timer instance.
         * If the timer is started, it will be stopped.
         */
        ~Timer()
        {
            if (Started())
            {
                Stop();
            }
        }

        /**
         * @brief Start the timer.
         * Sets the timer as started and records the start time.
         */
        void Start()
        {
            mStarted = true;
            mPaused = false;
            mStartTime = std::chrono::steady_clock::now();
        }

        /**
         * @brief Stop the timer.
         * Stops the timer and records the end time.
         * @return The duration of the timer in seconds.
         */
        float Stop()
        {
            mStarted = false;
            mPaused = false;
            mEndTime = std::chrono::steady_clock::now();
            mDuration = mEndTime - mStartTime;
            return mDuration.count();
        }

        /**
         * @brief Pause the timer.
         * Pauses the timer while retaining the elapsed time.
         */
        void Pause()
        {
            if (mStarted)
            {
                mStarted = false;
                mPaused = true;
                mPauseTime = mStartTime;
                Stop();
            }
        }

        /**
         * @brief Resume a paused timer.
         * Resumes the timer from a paused state without resetting elapsed time.
         */
        void Resume()
        {
            if (mPaused)
            {
                mStarted = true;
                mPaused = false;
                StartAtTime(mPauseTime);
            }
        }

        /**
         * @brief Peek at the timer's current elapsed time in seconds.
         * @return The current elapsed time in seconds, or 0 if the timer is not started.
         */
        float Peek()
        {
            if (mStarted)
            {
                if (mPaused)
                {
                    mDuration = mPauseTime - mStartTime;
                    return mDuration.count();
                }
                SteadyTimePoint now = std::chrono::steady_clock::now();
                mDuration = now - mStartTime;
                return mDuration.count();
            }
            return 0.0f; // Timer is not started.
        }

        /**
         * @brief Get the number of milliseconds since the start of the Timer.
         * @return The time in milliseconds, or 0 if the timer is not started.
         */
        std::int64_t PeekMilliseconds()
        {
            if (mStarted)
            {
                if (mPaused)
                {
                    mDuration = mPauseTime - mStartTime;
                    return std::chrono::duration_cast<std::chrono::milliseconds>(mDuration).count();
                }
                SteadyTimePoint now = std::chrono::steady_clock::now();
                mDuration = now - mStartTime;
                return std::chrono::duration_cast<std::chrono::milliseconds>(mDuration).count();
            }
            return 0;
        }

        /**
         * @brief Check if the timer is started.
         * @return true if the timer is started, false otherwise.
         */
        bool Started() { return mStarted; }

        /**
         * @brief Check if the timer is paused.
         * @return true if the timer is paused, false otherwise.
         */
        bool Paused() { return mPaused; }

        /**
         * @brief Check if the timer has ended.
         * @return true if the timer has ended, false otherwise.
         */
        bool Ended()
        {
            if (!mStarted && !mPaused)
            {
                return true;
            }
            return false;
        }

        /**
         * @brief Sets a timer to count down for a specified number of seconds and calls a callback
         * function when it ends.
         * @param seconds The number of seconds for the timer.
         * @param callback The function to call when the timer ends.
         */
        void SetTimer(int seconds, const TimerCallback &callback)
        {
            Interval interval(seconds);
            // Create a new thread to run the timer
            std::thread timerThread(
                [=]()
                {
                    Start();                               // Start the timer
                    std::this_thread::sleep_for(interval); // Sleep for the specified interval
                    callback();                            // Call the callback function
                    Stop();                                // Stop the timer
                });

            // Detach the thread, so it runs independently
            timerThread.detach();
        }

    private:
        /**
         * @brief Start the timer at a specified time point.
         * @param timepoint The time point to start the timer from.
         */
        void StartAtTime(const SteadyTimePoint &timepoint)
        {
            mStarted = true;
            mPaused = false;
            mStartTime = timepoint;
        }

    private:
        SteadyTimePoint mStartTime{};
        SteadyTimePoint mPauseTime{};
        SteadyTimePoint mEndTime{};

        std::chrono::duration<float> mDuration{};

        bool mStarted{};
        bool mEnded{};
        bool mPaused{};
    };

    struct TimerDeleter
    {
        void operator()(Timer *t) const { delete t; }
    };
}