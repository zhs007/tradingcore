#ifndef __TRADINGCORE_TRADINGCORE_H__
#define __TRADINGCORE_TRADINGCORE_H__

#include "candle.h"
#include "csvfile.h"
#include "indicator.h"

namespace trading {

typedef CandleData<int64_t, int64_t> CandleDataInt64;
typedef CandleList<int64_t, int64_t> CandleListInt64;

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

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

bool saveCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

}  // namespace trading

#endif  // __TRADINGCORE_TRADINGCORE_H__