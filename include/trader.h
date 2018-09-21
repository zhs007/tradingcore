#ifndef __TRADINGCORE_TRADER_H__
#define __TRADINGCORE_TRADER_H__

#include <string>
#include "exchange.h"
#include "pnl.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename PercentType>
struct TraderExchangeInfo {
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef Exchange<MoneyType, VolumeType> ExchangeT;
  typedef PNL<PercentType> PNLT;

  WalletT wallet;
  ExchangeT& exchange;
  PNLT pnl;

  TraderExchangeInfo(ExchangeT& e) : exchange(e) {}
};

template <typename MoneyType, typename VolumeType, typename PercentType>
class Trader {
 public:
  typedef TraderExchangeInfo<MoneyType, VolumeType, PercentType>
      TraderExchangeInfoT;
  typedef std::map<std::string, TraderExchangeInfoT> ExchangeMap;
  typedef PNL<PercentType> PNLT;

 public:
  Trader() {}
  ~Trader() {}

 public:
 protected:
  ExchangeMap m_map;
  PNLT m_pnl;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADER_H__