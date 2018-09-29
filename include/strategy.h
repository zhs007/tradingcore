#ifndef __TRADINGCORE_STRATEGY_H__
#define __TRADINGCORE_STRATEGY_H__

#include <map>
#include <string>
#include "candle.h"
#include "exchange.h"
#include "pnl.h"
#include "utils.h"
#include "wallet.h"
// #include "trader.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
class Strategy {
 public:
  typedef Strategy<MoneyType, VolumeType, ValueType, PercentType> StrategyT;
  typedef PNL<PercentType> PNLT;
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
  typedef std::map<std::string, ExchangeCategoryT*> ExchangeCategoryMap;
  typedef
      typename ExchangeCategoryMap::const_iterator ExchangeCategoryMapConstIter;
  typedef Wallet<MoneyType, VolumeType> WalletT;
  // typedef Trader<MoneyType, VolumeType, ValueType, PercentType> TraderT;

 public:
  Strategy() {}
  virtual ~Strategy() {}

 public:
  virtual void onTick() = 0;

  virtual void onCandle(WalletT& wallet, int candleIndex) = 0;

 public:
  void addExchangeCategory(const char* name) { m_map[name] = NULL; }

  bool hasExchangeCategory(const char* name) const {
    ExchangeCategoryMapConstIter it = m_map.find(name);
    return it != m_map.end();
  }

  void setExchangeCategory(const char* name, ExchangeCategoryT* pEC) {
    m_map[name] = pEC;
  }

  ExchangeCategoryT* getExchangeCategory(const char* name) {
    ExchangeCategoryMapConstIter it = m_map.find(name);
    if (it != m_map.end()) {
      return it->second;
    }

    return NULL;
  }

 protected:
  PNLT m_pnl;

  ExchangeCategoryMap m_map;
  // TraderT& m_trader;
};

}  // namespace trading

#endif  // __TRADINGCORE_STRATEGY_H__