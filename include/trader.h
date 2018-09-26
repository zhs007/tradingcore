#ifndef __TRADINGCORE_TRADER_H__
#define __TRADINGCORE_TRADER_H__

#include <string>
#include "exchange.h"
#include "pnl.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType, typename PercentType>
struct TraderExchangeInfo {
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef Exchange<MoneyType, VolumeType, ValueType> ExchangeT;
  typedef PNL<PercentType> PNLT;

  WalletT wallet;
  ExchangeT& exchange;
  PNLT pnl;

  TraderExchangeInfo(ExchangeT& e) : exchange(e), wallet(e.getCategoryMgr()) {}
};

template <typename MoneyType, typename VolumeType, typename ValueType, typename PercentType>
class Trader {
 public:
  typedef TraderExchangeInfo<MoneyType, VolumeType, ValueType, PercentType>
      TraderExchangeInfoT;
  typedef std::map<std::string, TraderExchangeInfoT*> ExchangeMap;
  typedef typename ExchangeMap::iterator ExchangeMapIter;
  typedef std::pair<std::string, TraderExchangeInfoT*> ExchangePair;
  typedef Exchange<MoneyType, VolumeType, ValueType> ExchangeT;
  typedef PNL<PercentType> PNLT;

 public:
  Trader() {}
  ~Trader() {}

 public:
  void addExchange(ExchangeT& exchange) {
    ExchangePair p(exchange.getName(), new TraderExchangeInfoT(exchange));

    m_map.insert(p);
  }

  void startSimTrade(time_t bt, time_t et, time_t ot) {
    time_t pt = bt;
    for (time_t ct = bt; ct <= et; pt = ct, ct += ot) {
      for (ExchangeMapIter it = m_map.begin(); it != m_map.end(); ++it) {
        it->second->exchange.onTick(it->second->wallet, pt, ct);
      }
    }
  }

 protected:
  ExchangeMap m_map;
  PNLT m_pnl;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADER_H__