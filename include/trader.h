#ifndef __TRADINGCORE_TRADER_H__
#define __TRADINGCORE_TRADER_H__

#include <string>
#include <vector>
#include "exchange.h"
#include "pnl.h"
#include "strategy.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
struct TraderExchangeInfo {
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef Exchange<MoneyType, VolumeType, ValueType> ExchangeT;
  // typedef PNL<PercentType> PNLT;

  // WalletT wallet;
  ExchangeT& exchange;
  // PNLT pnl;

  TraderExchangeInfo(ExchangeT& e) : exchange(e) {}
};

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
class Trader {
 public:
  typedef Trader<MoneyType, VolumeType, ValueType, PercentType> TraderT;
  typedef TraderExchangeInfo<MoneyType, VolumeType, ValueType, PercentType>
      TraderExchangeInfoT;
  typedef std::map<std::string, TraderExchangeInfoT*> ExchangeMap;
  typedef typename ExchangeMap::iterator ExchangeMapIter;
  typedef std::pair<std::string, TraderExchangeInfoT*> ExchangePair;
  typedef Exchange<MoneyType, VolumeType, ValueType> ExchangeT;
  typedef Strategy<MoneyType, VolumeType, ValueType, PercentType> StrategyT;
  typedef std::vector<StrategyT*> StrategyList;
  typedef typename StrategyList::iterator StrategyListIter;
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
  typedef std::function<void(ExchangeCategoryT&)> FuncForEachECT;
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryMgr<MoneyType, VolumeType> CategoryMgrT;
  // typedef PNL<PercentType> PNLT;

 public:
  Trader(CategoryMgrT& mgr) : m_wallet(mgr) {}
  ~Trader() {}

 public:
  void addExchange(ExchangeT& exchange) {
    ExchangePair p(exchange.getName(), new TraderExchangeInfoT(exchange));

    m_map.insert(p);
  }

  void addStrategy(StrategyT* pStrategy) {
    m_lstStrategy.push_back(pStrategy);

    FuncForEachECT func = std::bind(&TraderT::_forEachExchangeCategory, this,
                                    pStrategy, std::placeholders::_1);

    for (ExchangeMapIter it = m_map.begin(); it != m_map.end(); ++it) {
      it->second->exchange.forEach(func);
    }
  }

  int getCandleLength() {
    int len = -1;

    for (ExchangeMapIter it = m_map.begin(); it != m_map.end(); ++it) {
      int cl = it->second->exchange.getCandleLength();
      if (len == -1) {
        len = cl;
      }

      assert(len == cl);
    }

    return len;
  }

  void initCategoryInfo(const char* code, VolumeType vol, MoneyType avgPrice) {
    m_wallet.initCategoryInfo(code, vol, avgPrice);
  }

  void startSimTrade() {
    for (int i = 0; i < getCandleLength(); ++i) {
      for (ExchangeMapIter it = m_map.begin(); it != m_map.end(); ++it) {
        it->second->exchange.onCandle(m_wallet, i);
        // MoneyType tv = it->second->wallet.countTotalValue();
        // printf("%lld", tv);
      }

      for (StrategyListIter it = m_lstStrategy.begin();
           it != m_lstStrategy.end(); ++it) {
        (*it)->onCandle(m_wallet, i);
      }

      MoneyType tv = m_wallet.countTotalValue();
      printf("%lld \n", tv);
    }
  }

 protected:
  void _forEachExchangeCategory(StrategyT* pStrategy, ExchangeCategoryT& ec) {
    if (pStrategy->hasExchangeCategory(ec.getName())) {
      pStrategy->setExchangeCategory(ec.getName(), &ec);
    }
  }

 protected:
  ExchangeMap m_map;
  StrategyList m_lstStrategy;
  WalletT m_wallet;
  // PNLT m_pnl;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADER_H__