#ifndef FRAMERANGE_H
#define FRAMERANGE_H
#include <QtCore>

//! @brief Inclusive range
template <typename T>
struct _ValueRange {
    static_assert(std::is_same<T, char>::value ||
                  std::is_same<T, uchar>::value ||
                  std::is_same<T, int>::value ||
                  std::is_same<T, uint>::value ||
                  std::is_same<T, long>::value ||
                  std::is_same<T, ulong>::value,
                  "_ValueRange can be used only with char, uchar, int, uint, long, ulong");
    T fMin; //! @brief Inclusive range min
    T fMax; //! @brief Inclusive range max

    bool isUnary() const {
        return fMin == fMax;
    }

    bool isValid() const {
        return fMax < fMin;
    }

    bool inRange(const T& val) const {
        return val >= fMin && val <= fMax;
    }

    T span() const {
        return fMax - fMin + 1;
    }

    bool overlaps(const _ValueRange<T>& b) const {
        return !this->operator*(b).isValid();
    }

    bool neighbours(const _ValueRange<T>& b) const {
        if(b.fMax == fMin - 1) return true;
        if(fMax == b.fMin - 1) return true;
        return false;
    }

    void fixOrder() {
        if(fMin <= fMax) return;
        auto maxT = fMax;
        fMax = fMin;
        fMin = maxT;
    }

    bool operator<(const _ValueRange<T>& b) const {
        Q_ASSERT(!overlaps(b));
        return fMin < b.fMin;
    }

    bool operator>(const _ValueRange<T>& b) const {
        Q_ASSERT(!overlaps(b));
        return fMin > b.fMin;
    }

    bool operator==(const _ValueRange<T>& b) const {
        return fMin == b.fMin && fMax == b.fMax;
    }

    bool operator!=(const _ValueRange<T>& b) const {
        return fMin != b.fMin || fMax != b.fMax;
    }

    _ValueRange<T> operator*(const _ValueRange<T>& b) const {
        return {qMax(this->fMin, b.fMin), qMin(this->fMax, b.fMax)};
    }
    _ValueRange<T>& operator*=(const _ValueRange<T>& b) {
        this->fMin = qMax(this->fMin, b.fMin);
        this->fMax = qMin(this->fMax, b.fMax);
        return *this;
    }

    _ValueRange<T> operator+(const _ValueRange<T>& b) const {
        Q_ASSERT(neighbours(b));
        return {qMin(this->fMin, b.fMin), qMax(this->fMax, b.fMax)};
    }
    _ValueRange<T>& operator+=(const _ValueRange<T>& b) {
        Q_ASSERT(neighbours(b));
        this->fMin = qMin(this->fMin, b.fMin);
        this->fMax = qMax(this->fMax, b.fMax);
        return *this;
    }
};

typedef int Frame;
typedef int RelFrame;
typedef int AbsFrame;
//! @brief Inclusive frame range
typedef _ValueRange<Frame> FrameRange;
typedef _ValueRange<AbsFrame> AbsFrameRange;
typedef _ValueRange<RelFrame> RelFrameRange;

typedef int Sample;
//! @brief Inclusive sample range
typedef _ValueRange<Sample> SampleRange;
#endif // FRAMERANGE_H