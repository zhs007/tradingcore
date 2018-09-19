#ifndef __TRADINGCORE_TRADER_H__
#define __TRADINGCORE_TRADER_H__

#include <string>
#include "exchange.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
struct TraderExchangeInfo {
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef Exchange<MoneyType, VolumeType> ExchangeT;

  WalletT wallet;
  ExchangeT& exchange;

  TraderExchangeInfo(ExchangeT& e) : exchange(e) {}
};

template <typename MoneyType, typename VolumeType>
class Trader {
 public:
  typedef TraderExchangeInfo<MoneyType, VolumeType> TraderExchangeInfoT;
  typedef std::map<std::string, TraderExchangeInfoT> ExchangeMap;

 public:
  Trader() {}
  ~Trader() {}

 public:
 protected:
  ExchangeMap m_map;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADER_H__