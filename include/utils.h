#ifndef __TRADINGCORE_UTILS_H__
#define __TRADINGCORE_UTILS_H__

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