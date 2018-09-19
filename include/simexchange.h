#ifndef __TRADINGCORE_SIMEXCHANGE_H__
#define __TRADINGCORE_SIMEXCHANGE_H__

#include "exchange.h"
#include "candle.h"

namespace trading {

template <typename PriceType, typename VolumeType>
class SimExchange : public Exchange<PriceType, VolumeType> {
 public:
  typedef Exchange<PriceType, VolumeType> ExchangeT;
  typedef CandleList<PriceType, VolumeType> CandleListT;

 public:
  SimExchange() {}
  ~SimExchange() {}

 public:
 protected:
};

}  // namespace trading

#endif  // __TRADINGCORE_SIMEXCHANGE_H__