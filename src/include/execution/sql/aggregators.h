#pragma once

#include <algorithm>
#include <limits>

#include "common/macros.h"
#include "execution/sql/value.h"

namespace terrier::execution::sql {

/** Counting aggregate. */
class CountAggregate {
 public:
  /**
   * Constructor.
   */
  CountAggregate() = default;

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(CountAggregate);

  /**
   * Advance the count based on the NULL-ness of the input value.
   */
  void Advance(const Val &val) { count_ += static_cast<uint64_t>(!val.is_null_); }

  /**
   * Merge this count with the @em that count.
   */
  void Merge(const CountAggregate &that) { count_ += that.count_; }

  /**
   * Reset the aggregate.
   */
  void Reset() { count_ = 0; }

  /**
   * Return the current value of the count.
   */
  Integer GetCountResult() const { return Integer(count_); }

 private:
  uint64_t count_{0};
};

/** COUNT(*) aggregate. */
class CountStarAggregate {
 public:
  /**
   * Constructor.
   */
  CountStarAggregate() = default;

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(CountStarAggregate);

  /**
   * Advance the aggregate by one.
   */
  void Advance(UNUSED_ATTRIBUTE const Val &val) { count_++; }

  /**
   * Merge this count with the @em that count.
   */
  void Merge(const CountStarAggregate &that) { count_ += that.count_; }

  /**
   * Reset the aggregate.
   */
  void Reset() { count_ = 0; }

  /**
   * Return the current value of the count.
   */
  Integer GetCountResult() const { return Integer(count_); }

 private:
  uint64_t count_{0};
};

/** Generic summations. */
template <typename T>
class SumAggregate {
  static_assert(std::is_base_of_v<Val, T>, "Template type must subclass value");

 public:
  /**
   * Constructor.
   */
  SumAggregate() : sum_(static_cast<decltype(T::val_)>(0)) { sum_.is_null_ = true; }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(SumAggregate);

  /**
   * Advance the aggregate by a given input value.
   * If the input is NULL, no change is applied to the aggregate.
   * @param val The (potentially NULL) value to advance the sum by.
   */
  void Advance(const T &val) {
    if (val.is_null_) {
      return;
    }
    sum_.is_null_ = false;
    sum_.val_ += val.val_;
  }

  /**
   * Merge a partial sum aggregate into this aggregate.
   * If the partial sum is NULL, no change is applied to this aggregate.
   * @param that The (potentially NULL) value to merge into this aggregate.
   */
  void Merge(const SumAggregate<T> &that) {
    if (that.sum_.is_null_) {
      return;
    }
    sum_.is_null_ = false;
    sum_.val_ += that.sum_.val_;
  }

  /**
   * Reset the summation.
   */
  void Reset() {
    sum_.is_null_ = true;
    sum_.val_ = 0;
  }

  /**
   * Return the result of the summation.
   * @return The current value of the sum.
   */
  const T &GetResultSum() const { return sum_; }

 private:
  T sum_;
};

/** Integer sums. */
class IntegerSumAggregate : public SumAggregate<Integer> {};

/** Real sums. */
class RealSumAggregate : public SumAggregate<Real> {};

/** FixedDecimal sums. */
class FixedDecimalSumAggregate {

 public:
  /**
   * Constructor.
   */
  FixedDecimalSumAggregate() : sum_(static_cast<decltype(DecimalVal::val_)>(0)) { sum_.is_null_ = true; }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(FixedDecimalSumAggregate);

  /**
   * Advance the aggregate by a given input value.
   * If the input is NULL, no change is applied to the aggregate.
   * @param val The (potentially NULL) value to advance the sum by.
   */
  void Advance(const DecimalVal &val) {
    if (val.is_null_) {
      return;
    }
    sum_.precision_ = val.precision_;
    sum_.is_null_ = false;
    sum_.val_ += val.val_;
  }

  /**
   * Merge a partial sum aggregate into this aggregate.
   * If the partial sum is NULL, no change is applied to this aggregate.
   * @param that The (potentially NULL) value to merge into this aggregate.
   */
  void Merge(const FixedDecimalSumAggregate &that) {
    if (that.sum_.is_null_) {
      return;
    }
    sum_.precision_ = that.sum_.precision_;
    sum_.is_null_ = false;
    sum_.val_ += that.sum_.val_;
  }

  /**
   * Reset the summation.
   */
  void Reset() {
    sum_.is_null_ = true;
    sum_.val_ = Decimal128(0);
  }

  /**
   * Return the result of the summation.
   * @return The current value of the sum.
   */
  const DecimalVal &GetResultSum() const { return sum_; }

 private:
  DecimalVal sum_;
};

/** Generic max. */
template <typename T>
class MaxAggregate {
  static_assert(std::is_base_of_v<Val, T>, "Template type must subclass value");

 public:
  /**
   * Constructor.
   */
  MaxAggregate() : max_(std::numeric_limits<decltype(T::val_)>::min()) { max_.is_null_ = true; }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(MaxAggregate);

  /**
   * Advance the aggregate by the input value @em val.
   */
  void Advance(const T &val) {
    if (val.is_null_) {
      return;
    }
    max_.is_null_ = false;
    max_.val_ = std::max(val.val_, max_.val_);
  }

  /**
   * Merge a partial max aggregate into this aggregate.
   */
  void Merge(const MaxAggregate<T> &that) {
    if (that.max_.is_null_) {
      return;
    }
    max_.is_null_ = false;
    max_.val_ = std::max(that.max_.val_, max_.val_);
  }

  /**
   * Reset the aggregate.
   */
  void Reset() {
    max_.is_null_ = true;
    max_.val_ = std::numeric_limits<decltype(T::val_)>::min();
  }

  /**
   * Return the result of the max.
   */
  const T &GetResultMax() const { return max_; }

 private:
  T max_;
};

/** Integer max. */
class IntegerMaxAggregate : public MaxAggregate<Integer> {};

/** Real max. */
class RealMaxAggregate : public MaxAggregate<Real> {};

/** Date max. */
class DateMaxAggregate : public MaxAggregate<DateVal> {};

/** Timestamp max. */
class TimestampMaxAggregate : public MaxAggregate<TimestampVal> {};

/** String max. */
class StringMaxAggregate : public MaxAggregate<StringVal> {};

/** FixedDecimal max. */
class FixedDecimalMaxAggregate {

 public:
  /**
   * Constructor.
   */
  FixedDecimalMaxAggregate() : max_(std::numeric_limits<decltype(DecimalVal::val_)>::min()) {

    int128_t minval = 1;
    minval = minval << 127;
    max_.val_.SetValue(minval);
    max_.is_null_ = true;
  }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(FixedDecimalMaxAggregate);

  /**
   * Advance the aggregate by the input value @em val.
   */
  void Advance(const DecimalVal &val) {
    if (val.is_null_) {
      return;
    }
    max_.precision_ = val.precision_;
    max_.is_null_ = false;
    max_.val_ = std::max(val.val_, max_.val_);
  }

  /**
   * Merge a partial max aggregate into this aggregate.
   */
  void Merge(const FixedDecimalMaxAggregate &that) {
    if (that.max_.is_null_) {
      return;
    }
    max_.precision_ = that.max_.precision_;
    max_.is_null_ = false;
    max_.val_ = std::max(that.max_.val_, max_.val_);
  }

  /**
   * Reset the aggregate.
   */
  void Reset() {
    max_.is_null_ = true;
    int128_t minval = 1;
    minval = minval << 127;
    max_.val_.SetValue(minval);
  }

  /**
   * Return the result of the max.
   */
  const DecimalVal &GetResultMax() const { return max_; }

 private:
  DecimalVal max_;
};

/** Generic min. */
template <typename T>
class MinAggregate {
  static_assert(std::is_base_of_v<Val, T>, "Template type must subclass value");

 public:
  /**
   * Constructor.
   */
  MinAggregate() : min_(std::numeric_limits<decltype(T::val_)>::max()) { min_.is_null_ = true; }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(MinAggregate);

  /**
   * Advance the aggregate by the input value @em val.
   */
  void Advance(const T &val) {
    if (val.is_null_) {
      return;
    }
    min_.is_null_ = false;
    min_.val_ = std::min(val.val_, min_.val_);
  }

  /**
   * Merge a partial min aggregate into this aggregate.
   */
  void Merge(const MinAggregate<T> &that) {
    if (that.min_.is_null_) {
      return;
    }
    min_.is_null_ = false;
    min_.val_ = std::min(that.min_.val_, min_.val_);
  }

  /**
   * Reset the aggregate.
   */
  void Reset() {
    min_.is_null_ = true;
    min_.val_ = std::numeric_limits<decltype(T::val_)>::max();
  }

  /**
   * Return the result of the minimum.
   */
  const T &GetResultMin() const { return min_; }

 private:
  T min_;
};

/** Integer min. */
class IntegerMinAggregate : public MinAggregate<Integer> {};

/** Real min. */
class RealMinAggregate : public MinAggregate<Real> {};

/** Date min. */
class DateMinAggregate : public MinAggregate<DateVal> {};

/** Timestamp min. */
class TimestampMinAggregate : public MinAggregate<TimestampVal> {};

/** String min. */
class StringMinAggregate : public MinAggregate<StringVal> {};

/** FixedDecimal min. */
class FixedDecimalMinAggregate {
  static_assert(std::is_base_of_v<Val, DecimalVal>, "Template type must subclass value");

 public:
  /**
   * Constructor.
   */
  FixedDecimalMinAggregate() : min_(std::numeric_limits<decltype(DecimalVal::val_)>::max()) {
    uint128_t maxval = 1;
    maxval = maxval << 127;
    maxval -= 1;
    min_.val_.SetValue(maxval);
    min_.is_null_ = true;
  }

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(FixedDecimalMinAggregate);

  /**
   * Advance the aggregate by the input value @em val.
   */
  void Advance(const DecimalVal &val) {
    if (val.is_null_) {
      return;
    }
    min_.precision_ = val.precision_;
    min_.is_null_ = false;
    min_.val_ = std::min(val.val_, min_.val_);
  }

  /**
   * Merge a partial min aggregate into this aggregate.
   */
  void Merge(const FixedDecimalMinAggregate &that) {
    if (that.min_.is_null_) {
      return;
    }
    min_.precision_ = that.min_.precision_;
    min_.is_null_ = false;
    min_.val_ = std::min(that.min_.val_, min_.val_);
  }

  /**
   * Reset the aggregate.
   */
  void Reset() {
    min_.is_null_ = true;
    uint128_t maxval = 1;
    maxval = maxval << 127;
    maxval -= 1;
    min_.val_.SetValue(maxval);
  }

  /**
   * Return the result of the minimum.
   */
  const DecimalVal &GetResultMin() const { return min_; }

 private:
  DecimalVal min_;
};

/** Average aggregate. */
class AvgAggregate {
 public:
  /**
   * Constructor.
   */
  AvgAggregate() = default;

  /**
   * This class cannot be copied or moved.
   */
  DISALLOW_COPY_AND_MOVE(AvgAggregate);

  /**
   * Advance the aggregate by the input value @em val.
   */
  template <typename T>
  void Advance(const T &val) {
    if (val.is_null_) {
      return;
    }
    sum_ += static_cast<double>(val.val_);
    count_++;
  }

  /**
   * Merge a partial average aggregate into this aggregate.
   */
  void Merge(const AvgAggregate &that) {
    sum_ += that.sum_;
    count_ += that.count_;
  }

  /**
   * Reset the aggregate.
   */
  void Reset() {
    sum_ = 0.0;
    count_ = 0;
  }

  /**
   * Return the result of the minimum.
   */
  Real GetResultAvg() const {
    if (count_ == 0) {
      return Real::Null();
    }
    return Real(sum_ / static_cast<double>(count_));
  }

 private:
  double sum_{0.0};
  uint64_t count_{0};
};

}  // namespace terrier::execution::sql
