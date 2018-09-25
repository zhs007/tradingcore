#ifndef __TRADINGCORE_UTILS_H__
#define __TRADINGCORE_UTILS_H__

#include <time.h>
#include <string>

namespace trading {

struct CSVHead {
  std::string curtime;
  std::string open;
  std::string close;
  std::string high;
  std::string low;
  std::string volume;
  std::string openInterest;
};

enum CSVTIMETYPE {
  // like 2018-09-19 00:00:00
  CSVTIME_FORMAT_STR = 0,
  // unix timestamp
  CSVTIME_TIMESTAMP = 1,
};

struct CSVConfig {
  CSVHead head;
  CSVTIMETYPE timeType;
  int scalePrice;
  int scaleVolume;
};

typedef int64_t TradeID;
typedef int64_t OrderID;

inline time_t str2time(const char* str) {
  tm tm;
  strptime(str, "%Y-%m-%d %H:%M:%S", &tm);
  tm.tm_isdst = -1;
  return mktime(&tm);
}

// Percent

enum PERCENT_TYPE { PERCENT_TYPE_FIXEDPOINT_MILLION = 0 };

template <typename PercentType, typename NumType, PERCENT_TYPE PT>
struct PercentProc {
  static PercentType countPercent(NumType num, NumType den);
};

template <typename PercentType, typename NumType>
struct PercentProc<PercentType, NumType, PERCENT_TYPE_FIXEDPOINT_MILLION> {
  static PercentType countPercent(NumType num, NumType den) {
    return num * 1000000 / den;
  }
};

// template<typename PriceType>
// PriceType str2Price(const char* str);

// template<typename PriceType = int64_t>
// PriceType str2Price(const char* str, int scale) {
//     double dv = std::stod(str);
//     open = o * cfg.scalePrice;
//     return
// }

}  // namespace trading

#endif  // __TRADINGCORE_UTILS_H__