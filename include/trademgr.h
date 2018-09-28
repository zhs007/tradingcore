#ifndef __TRADINGCORE_TRADEMGR_H__
#define __TRADINGCORE_TRADEMGR_H__

#include <map>
#include <string>
#include "trade.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class TradeMgr {
 public:
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef std::map<TradeID, TradeT> TradeMap;
  typedef typename TradeMap::iterator TradeMapIter;
  typedef std::pair<TradeID, TradeT> TradePair;

 public:
  TradeMgr() : m_curTradeID(0) {}
  ~TradeMgr() {}

 public:
  TradeID newTradeID() { return ++m_curTradeID; }

  TradeT& newTrade() {
    TradePair p;

    p.first = newTradeID();
    p.second.tradeID = p.first;

    m_map.insert(p);

    printf("new trade %lld\n", p.first);

    return m_map[p.first];
  }

  TradeT* getTrade(TradeID tradeID) {
    TradeMapIter it = m_map.find(tradeID);
    if (it != m_map.end()) {
      return &(it->second);
    }

    return NULL;
  }

 protected:
  TradeID m_curTradeID;
  TradeMap m_map;
};

// template <typename MoneyType, typename VolumeType>
// static TradeMgr<MoneyType, VolumeType>* getTradeMgr() {
//   typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;

//   static TradeMgrT* pMgr = new TradeMgrT();

//   return pMgr;
// }

}  // namespace trading

#endif  // __TRADINGCORE_TRADE_H__