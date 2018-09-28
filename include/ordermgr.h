#ifndef __TRADINGCORE_ORDERMGR_H__
#define __TRADINGCORE_ORDERMGR_H__

#include <map>
#include <string>
#include "order.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class OrderMgr {
 public:
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef std::map<TradeID, OrderT> OrderMap;
  typedef typename OrderMap::iterator OrderMapIter;
  typedef std::pair<TradeID, OrderT> OrderPair;

 public:
  OrderMgr() : m_curOrderID(0) {}
  ~OrderMgr() {}

 public:
  OrderID newOrderID() { return ++m_curOrderID; }

  OrderT& newOrder() {
    OrderPair p;

    p.first = newOrderID();
    p.second.orderID = p.first;

    m_map.insert(p);

    return m_map[p.first];
  }

  OrderT* getOrder(OrderID orderID) {
    OrderMapIter it = m_map.find(orderID);
    if (it != m_map.end()) {
      return &(it->second);
    }

    return NULL;
  }

 protected:
  OrderID m_curOrderID;
  OrderMap m_map;
};

// template <typename MoneyType, typename VolumeType>
// static OrderMgr<MoneyType, VolumeType>* getOrderMgr() {
//   typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;

//   static OrderMgrT* pMgr = new OrderMgrT();

//   return pMgr;
// }

}  // namespace trading

#endif  // __TRADINGCORE_ORDERMGR_H__