#ifndef __TRADINGCORE_TRADINGCORE_H__
#define __TRADINGCORE_TRADINGCORE_H__

#include "candle.h"
#include "csvfile.h"
#include "indicator.h"
#include "utils.h"

namespace trading {

typedef CandleData<int64_t, int64_t> CandleDataInt64;
typedef CandleList<int64_t, int64_t> CandleListInt64;

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

bool saveCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

}  // namespace trading

#endif  // __TRADINGCORE_TRADINGCORE_H__