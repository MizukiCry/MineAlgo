#ifndef MINEALGO_MS_LIB_H_
#define MINEALGO_MS_LIB_H_

#include <algorithm>
#include <cassert>
#include <chrono>
#include <random>
#include <thread>
#include <utility>
#include <vector>

namespace ms_algo {
    using std::vector;

    template<class T>
    using Matrix = vector<vector<T>>;

    template<class T>
    vector<T> operator+(const vector<T>& lhs, const vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] + rhs[i];
        }
        return result;
    }

    template<class T>
    vector<T> operator-(const vector<T>& lhs, const vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] - rhs[i];
        }
        return result;
    }

    template<class T>
    vector<T> operator*(const vector<T>& lhs, const vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] * rhs[i];
        }
        return result;
    }

    template<class T>
    vector<T> operator/(const vector<T>& lhs, const vector<T>& rhs) {
        assert(lhs.size() == rhs.size());
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] / rhs[i];
        }
        return result;
    }

    template<class T>
    vector<T>& operator+=(vector<T>& lhs, const vector<T>& rhs) {
        lhs = lhs + rhs;
        return lhs;
    }

    template<class T>
    vector<T>& operator-=(vector<T>& lhs, const vector<T>& rhs) {
        lhs = lhs - rhs;
        return lhs;
    }

    template<class T>
    vector<T>& operator*=(vector<T>& lhs, const vector<T>& rhs) {
        lhs = lhs * rhs;
        return lhs;
    }

    template<class T>
    vector<T>& operator/=(vector<T>& lhs, const vector<T>& rhs) {
        lhs = lhs / rhs;
        return lhs;
    }

    template<class T>
    vector<T> operator+(const vector<T>& lhs, const T& rhs) {
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] + rhs;
        }
        return result;
    }

    template<class T>
    vector<T> operator-(const vector<T>& lhs, const T& rhs) {
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] - rhs;
        }
        return result;
    }

    template<class T>
    vector<T> operator*(const vector<T>& lhs, const T& rhs) {
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] * rhs;
        }
        return result;
    }

    template<class T>
    vector<T> operator/(const vector<T>& lhs, const T& rhs) {
        vector<T> result(lhs);
        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = result[i] / rhs;
        }
        return result;
    }

    template<class T>
    vector<T>& operator+=(vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs + rhs);
        return lhs;
    }

    template<class T>
    vector<T>& operator-=(vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs - rhs);
        return lhs;
    }

    template<class T>
    vector<T>& operator*=(vector<T>& lhs, const T& rhs) {
        lhs = std::move(lhs * rhs);
        return lhs;
    }

    template<class T>
    vector<T>& operator/=(vector<T>& lhs, const T& rhs) {
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

    bool Greater(double lhs, double rhs) {
        return lhs - rhs > kEpsilon;
    }

    bool Less(double lhs, double rhs) {
        return rhs - lhs > kEpsilon;
    }

    bool IsZero(double x) {
        return std::abs(x) < kEpsilon;
    }

    bool NotZero(double x) {
        return std::abs(x) > kEpsilon;
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
        return (double)ms_rand() / std::mt19937::max() * (r - l) + l;
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

    // Shuffles a vector.
    template<class T>
    void ShuffleVector(vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), ms_rand);
    }

    const int kMaxRowCount = 50;
    const int kMaxColumnCount = 100;
    const int kMaxThreadCount = std::clamp((int)std::thread::hardware_concurrency(), 8, 64);
    const int kMaxTimeLimitMilliseconds = 60 * 1000;

    bool Inside(int row, int column, int row_count, int column_count) {
        return 1 <= row && row <= row_count && 1 <= column && column <= column_count;
    }
}

#endif