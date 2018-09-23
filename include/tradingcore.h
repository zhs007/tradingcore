#ifndef __TRADINGCORE_TRADINGCORE_H__
#define __TRADINGCORE_TRADINGCORE_H__

#include "candle.h"
#include "csvfile.h"
#include "exchange.h"
#include "indicator.h"
#include "order.h"
#include "orderlogic.h"
#include "ordermgr.h"
#include "simexchange.h"
#include "trade.h"
#include "trademgr.h"
#include "trader.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

typedef CandleData<int64_t, int64_t> CandleDataInt64;
typedef CandleList<int64_t, int64_t> CandleListInt64;
typedef Trader<int64_t, int64_t, int64_t> TraderInt64;

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

bool saveCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

}  // namespace trading

#endif  // __TRADINGCORE_TRADINGCORE_H__