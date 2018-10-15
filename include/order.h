#ifndef __TRADINGCORE_ORDER_H__
#define __TRADINGCORE_ORDER_H__

#include <string>
#include <vector>
#include "category.h"
// #include "exchange.h"
#include "trade.h"
#include "utils.h"
// #include "wallet.h"

namespace trading {

enum ORDER_TYPE { ORDER_LIMIT = 0 };

enum ORDER_SIDE { ORDER_BUY = 0, ORDER_SELL };

template <typename MoneyType, typename VolumeType>
struct Order {
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef std::vector<TradeT> TradeList;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef AvgPriceResult<MoneyType, VolumeType> AvgPriceResultT;

  std::string categoryName;

  OrderID orderID;

  ORDER_TYPE orderType;

  ORDER_SIDE orderSide;

  MoneyType destPrice;
  VolumeType destVolume;  // must > 0
  time_t bt;

  MoneyType avgPrice;
  VolumeType lastVolume;  // must >= 0
  time_t ct;

  TradeList lstTrade;

  void setLimitOrder(ORDER_SIDE side, MoneyType price, VolumeType volume,
                     time_t t) {
    orderType = ORDER_LIMIT;
    orderSide = side;
    destPrice = price;
    destVolume = volume;

    bt = t;

    avgPrice = 0;
    lastVolume = volume;
    ct = t;
    lstTrade.clear();
  }

  // vol > 0
  void procTransaction(const CategoryConfigT& cfg, MoneyType price,
                       VolumeType vol) {
    assert(vol > 0);
    assert(vol <= lastVolume);
    assert(destVolume >= lastVolume);

    if (orderSide == ORDER_BUY) {
      assert(price <= destPrice);

      AvgPriceResultT ap =
          cfg.countAvgPriceEx(avgPrice, destVolume - lastVolume, price, vol);
      avgPrice = ap.avgPrice;
      lastVolume -= std::abs(ap.lastVolume);
    } else {
      assert(price >= destPrice);

      AvgPriceResultT ap = cfg.countAvgPriceEx(
          avgPrice, -(destVolume - lastVolume), price, -vol);
      avgPrice = ap.avgPrice;
      lastVolume -= std::abs(ap.lastVolume);
    }
  }
};

template <typename MoneyType, typename VolumeType>
struct FuncOrderCmp {
  typedef Order<MoneyType, VolumeType> OrderT;

  bool operator()(OrderT* pA, OrderT* pB) const {
    assert(pA != NULL);
    assert(pB != NULL);

    return pA->bt < pB->bt;
  }
};

inline const char* getOrderTypeStr(ORDER_TYPE t) {
  switch (t) {
    case ORDER_LIMIT:
      return "limit";
  }

  return "unknow";
}

inline const char* getOrderSideStr(ORDER_SIDE s) {
  switch (s) {
    case ORDER_BUY:
      return "buy";
    case ORDER_SELL:
      return "sell";
  }

  return "unknow";
}

}  // namespace trading

#endif  // __TRADINGCORE_ORDER_H__