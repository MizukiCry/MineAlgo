#ifndef MINEALGO_MS_LIB_H_
#define MINEALGO_MS_LIB_H_

#include <algorithm>
#include <cassert>
#include <chrono>
#include <random>

namespace ms_algo {
    template<class T>
    std::vector<T>&& operator+(const std::vector<T>& lhs, const std::vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] - rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator-(const std::vector<T>& lhs, const std::vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] - rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator*(const std::vector<T>& lhs, const std::vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] * rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator/(const std::vector<T>& lhs, const std::vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] / rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>& operator+=(std::vector<T>& lhs, const std::vector<T>& rhs) {
        lhs = std::move(lhs + rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator-=(std::vector<T>& lhs, const std::vector<T>& rhs) {
        lhs = std::move(lhs - rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator*=(std::vector<T>& lhs, const std::vector<T>& rhs) {
        lhs = std::move(lhs * rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator/=(std::vector<T>& lhs, const std::vector<T>& rhs) {
        lhs = std::move(lhs / rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>&& operator+(const std::vector<T>& lhs, const T& rhs) {
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] + rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator-(const std::vector<T>& lhs, const T& rhs) {
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] - rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator*(const std::vector<T>& lhs, const T& rhs) {
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] * rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>&& operator/(const std::vector<T>& lhs, const T& rhs) {
        std::vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = std::move(result[i] / rhs[i]);
        }
        return std::move(result);
    }

    template<class T>
    std::vector<T>& operator+=(std::vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs + rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator-=(std::vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs - rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator*=(std::vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs * rhs);
        return lhs;
    }

    template<class T>
    std::vector<T>& operator/=(std::vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs / rhs);
        return lhs;
    }

    const int kRowOffset[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int kColumnOffset[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    const double kEpsilon = 1e-5;

    // Checks whether lhs and rhs is nearly the same.
    bool Equal(double lhs, double rhs) {
        return std::abs(lhs - rhs) < kEpsilon;
    }

    std::chrono::steady_clock::time_point initial_clock = std::chrono::steady_clock::now();
    std::mt19937 ms_rand(initial_clock.time_since_epoch().count());

    // Generates a random integer in [l, r).
    int RandInteger(int l, int r) {
        assert(l < r);
        return ms_rand() % (r - l) + l;
    }

    // Generates a random float in [l, r).
    double RandFloat(float l, float r) {
        assert(l < r);
        return (double)ms_rand() / std::mt19937::max() / (r - l) + l;
    }

    int64_t GetMicroseconds() {
        std::chrono::steady_clock::time_point current_clock = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(current_clock - initial_clock).count();
    }

    int64_t GetMilliseconds() {
        std::chrono::steady_clock::time_point current_clock = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(current_clock - initial_clock).count();
    }

    double GetTime() {
        return (double)GetMicroseconds() * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    }
}

#endif