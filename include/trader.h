#ifndef __TRADINGCORE_TRADER_H__
#define __TRADINGCORE_TRADER_H__

#include <string>
#include "exchange.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class Trader {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef Exchange<MoneyType, VolumeType> ExchangeT;

 public:
  Trader() {}
  ~Trader() {}

 public:
 protected:
  WalletT m_wallet;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADER_H__