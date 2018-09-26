#ifndef __TRADINGCORE_STRATEGY_H__
#define __TRADINGCORE_STRATEGY_H__

#include <string>
#include "candle.h"
#include "exchange.h"
#include "pnl.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename PercentType>
class Strategy {
 public:
  typedef Strategy<MoneyType, VolumeType, PercentType> StrategyT;
  typedef PNL<PercentType> PNLT;
  typedef CandleData<MoneyType, VolumeType> CandleDataT;

 public:
  Strategy() {}
  virtual ~Strategy() {}

 public:
  virtual void onTick() = 0;

  virtual void onCandle(CandleDataT& cd) = 0;

 protected:
  PNLT m_pnl;
};

}  // namespace trading

#endif  // __TRADINGCORE_STRATEGY_H__