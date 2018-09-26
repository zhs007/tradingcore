#ifndef __TRADINGCORE_TRADINGCORE_H__
#define __TRADINGCORE_TRADINGCORE_H__

#include "candle.h"
#include "csvfile.h"
#include "exchange.h"
#include "indicator.h"
#include "indicatormgr.h"
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
typedef Trader<int64_t, int64_t, int64_t, int64_t> TraderInt64;
typedef Exchange<int64_t, int64_t, int64_t> ExchangeInt64;
typedef Indicator<int64_t, int64_t, int64_t> IndicatorInt64;
typedef IndicatorMgr<int64_t, int64_t, int64_t> IndicatorMgrInt64;

typedef SimExchange<int64_t, int64_t, int64_t> SimExchangeInt64;
typedef SimExchangeCategory<int64_t, int64_t, int64_t> SimExchangeCategoryInt64;
typedef OrderLogic_Simple2<int64_t, int64_t> OrderLogic_Simple2Int64;

void initInt64();

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

bool saveCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg);

typedef CandleData<double, double> CandleDataFloat64;
typedef CandleList<double, double> CandleListFloat64;
typedef Trader<double, double, double, double> TraderFloat64;
typedef Exchange<double, double, double> ExchangeFloat64;

}  // namespace trading

#endif  // __TRADINGCORE_TRADINGCORE_H__