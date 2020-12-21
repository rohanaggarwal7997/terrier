#include <vector>

#include "benchmark/benchmark.h"
#include "benchmark_util/benchmark_config.h"
#include "common/scoped_timer.h"
#include "execution/sql/runtime_types.h"

namespace noisepage {

class DecimalBenchmark : public benchmark::Fixture {
 public:
  void SetUp(const benchmark::State &state) final {
    // generate a random column of decimals and floats
//    for (unsigned i = 0; i < 1000000; i++) {
//      execution::sql::Decimal128 d(i);
//      decimals_.push_back(d);
//      float f = (static_cast<float>(i)) / (static_cast<float>(1000));
//      floats_.push_back(f);
//    }
    for (unsigned i = 0; i < 1000000; i++) {
      int128_t decimal = 0;
      std::string str_decimal = "";
      for(unsigned j = 0; j < 34; j++) {
        int128_t last_digit = rand() % 9;
        last_digit += 1;
        decimal = decimal+last_digit;
        decimal *= 10;
        str_decimal.push_back('0' + static_cast<int>(last_digit));
      }
      int128_t last_digit = rand() % 10;
      decimal = decimal+last_digit;
      str_decimal.push_back('0' + static_cast<int>(last_digit));
      execution::sql::Decimal128 d(decimal);
      decimals_lhs_.push_back(d);
      str_decimals_lhs_.push_back(str_decimal);
    }
    for (unsigned i = 0; i < 1000000; i++) {
      int128_t decimal = 0;
      std::string str_decimal = "";
      for(unsigned j = 0; j < 34; j++) {
        int128_t last_digit = rand() % 9;
        last_digit += 1;
        decimal = decimal+last_digit;
        decimal *= 10;
        str_decimal.push_back('0' + static_cast<int>(last_digit));
      }
      int128_t last_digit = rand() % 10;
      decimal = decimal+last_digit;
      str_decimal.push_back('0' + static_cast<int>(last_digit));
      execution::sql::Decimal128 d(decimal);
      decimals_rhs_.push_back(d);
      str_decimals_rhs_.push_back(str_decimal);
    }
  }

  void TearDown(const benchmark::State &state) final {}

  std::vector<execution::sql::Decimal128> decimals_lhs_;
  std::vector<execution::sql::Decimal128> decimals_rhs_;
  std::vector<std::string> str_decimals_lhs_;
  std::vector<std::string> str_decimals_rhs_;
  std::vector<execution::sql::Decimal128> decimals_;
  std::vector<int128_t> floats_;
};

// Insert the num_inserts_ of tuples into a DataTable concurrently
// NOLINTNEXTLINE
BENCHMARK_DEFINE_F(DecimalBenchmark, AddDecimal)(benchmark::State &state) {
  // NOLINTNEXTLINE
  uint64_t elapsed_ms;
  {
    execution::sql::Decimal128 result(0);
    common::ScopedTimer<std::chrono::milliseconds> timer(&elapsed_ms);
    for (unsigned j = 0; j < 1000; j++)
      for (unsigned i = 0; i < decimals_.size(); i++) {
        result += decimals_[i];
      }
  }
  state.SetIterationTime(static_cast<double>(elapsed_ms) / 1000.0);
}

// NOLINTNEXTLINE
BENCHMARK_DEFINE_F(DecimalBenchmark, AddFloat)(benchmark::State &state) {
  // NOLINTNEXTLINE
  uint64_t elapsed_ms;
  {
    float result = 0;
    common::ScopedTimer<std::chrono::milliseconds> timer(&elapsed_ms);
    for (unsigned j = 0; j < 1000; j++)
      for (unsigned i = 0; i < floats_.size(); i++) {
        result += floats_[i];
      }
  }
  state.SetIterationTime(static_cast<double>(elapsed_ms) / 1000.0);
}

std::string string_multiply(std::string nums1, std::string nums2) {
  int n = nums1.size();
  int m = nums2.size();
  std::string ans(n + m, '0');
  for(int i = n - 1; i>=0; i--){
    for(int j = m - 1; j >= 0; j--){
      int p = (nums1[i] - '0') * (nums2[j] - '0') + (ans[i + j + 1] - '0');
      ans[i+j+1] = p % 10 + '0';
      ans[i+j] += p / 10 ;
    }
  }
  for(int i = 0; i < m + n; i++){
    if(ans[i] !='0')return ans.substr(i);
  }
  return "0";
}

// NOLINTNEXTLINE
BENCHMARK_DEFINE_F(DecimalBenchmark, MultiplyDecimal)(benchmark::State &state) {
  // NOLINTNEXTLINE
  uint64_t elapsed_ms;
  {
    common::ScopedTimer<std::chrono::milliseconds> timer(&elapsed_ms);
    for (unsigned i = 0; i < 1000000; i++) {
      decimals_lhs_[i].MultiplyAndSet(decimals_rhs_[i], 33);
    }
  }
  state.SetIterationTime(static_cast<double>(elapsed_ms) / 1000.0);
}

// NOLINTNEXTLINE
BENCHMARK_DEFINE_F(DecimalBenchmark, MultiplyString)(benchmark::State &state) {
  // NOLINTNEXTLINE
  uint64_t elapsed_ms;
  {
    common::ScopedTimer<std::chrono::milliseconds> timer(&elapsed_ms);
    for (unsigned i = 0; i < 1000000; i++) {
      str_decimals_lhs_[i] = string_multiply(str_decimals_lhs_[i], str_decimals_rhs_[i]);
      str_decimals_lhs_[i] = str_decimals_lhs_[i].substr(0, 37);
    }
  }
  state.SetIterationTime(static_cast<double>(elapsed_ms) / 1000.0);
}

// ----------------------------------------------------------------------------
// Benchmark Registration
// ----------------------------------------------------------------------------
// clang-format off
//BENCHMARK_REGISTER_F(DecimalBenchmark, AddDecimal)
//    ->Unit(benchmark::kMillisecond)
//    ->UseRealTime()
//    ->UseManualTime();
//BENCHMARK_REGISTER_F(DecimalBenchmark, AddFloat)
//    ->Unit(benchmark::kMillisecond)
//    ->UseRealTime()
//    ->UseManualTime();
BENCHMARK_REGISTER_F(DecimalBenchmark, MultiplyDecimal)
->Unit(benchmark::kMillisecond)
    ->UseRealTime()
    ->UseManualTime();
BENCHMARK_REGISTER_F(DecimalBenchmark, MultiplyString)
->Unit(benchmark::kMillisecond)
    ->UseRealTime()
    ->UseManualTime();
// clang-format on

}  // namespace noisepage
