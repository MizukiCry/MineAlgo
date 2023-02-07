#ifndef MINEALGO_MS_TIMER_H_
#define MINEALGO_MS_TIMER_H_

#include <atomic>
#include <cassert>

#include "ms_lib.h"

namespace ms_algo {
    class Timer {
    private:
        int time_limit_milliseconds_;

        int64_t beginning_timestamp_;

        std::atomic_bool time_is_up_;

    public:
        Timer(const int time_limit_milliseconds = 1000) {
            assert(1 <= time_limit_milliseconds && time_limit_milliseconds <= 100'000'000);
            time_limit_milliseconds_ = time_limit_milliseconds;
            beginning_timestamp_ = GetMilliseconds();
            time_is_up_ = false;
        }

        int time_limit_milliseconds() const {
            return time_limit_milliseconds_;
        }

        int64_t beginning_timestamp() const {
            return beginning_timestamp_;
        }

        void Terminate() {
            time_is_up_ = true;
        }

        bool TimeIsUp() {
            if (time_is_up_) {
                return true;
            }
            if (GetMilliseconds() - beginning_timestamp() >= time_limit_milliseconds()) {
                Terminate();
                return true;
            }
            return false;
        }
    };
}

#endif