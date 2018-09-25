#ifndef __TRADINGCORE_ORDER_H__
#define __TRADINGCORE_ORDER_H__

#include <string>
#include <vector>
#include "category.h"
#include "exchange.h"
#include "trade.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

enum ORDER_TYPE { ORDER_LIMIT = 0 };

enum ORDER_SIDE { ORDER_BUY = 0, ORDER_SELL };

template <typename MoneyType, typename VolumeType>
struct Order {
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef std::vector<TradeT> TradeList;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef AvgPriceResult<MoneyType, VolumeType> AvgPriceResultT;

  OrderID orderID;

  ORDER_TYPE orderType;

  ORDER_SIDE orderSide;

  MoneyType destPrice;
  VolumeType destVolume;  // must > 0
  time_t bt;

  MoneyType avgPrice;
  VolumeType curVolume;  // must >= 0
  time_t ct;

  TradeList lstTrade;

  // vol > 0
  void procTransaction(const CategoryConfigT& cfg, MoneyType price, VolumeType vol) {
    assert(vol > 0);
    assert(vol <= curVolume);
    assert(destVolume >= curVolume);

    if (orderSide == ORDER_BUY) {
      assert(price <= destPrice);

      AvgPriceResultT ap =
          cfg.countAvgPriceEx(avgPrice, destVolume - curVolume, price, vol);
      avgPrice = ap.avgPrice;
      curVolume = ap.lastVolume;
    } else {
      assert(price >= destPrice);

      AvgPriceResultT ap =
          cfg.countAvgPriceEx(avgPrice, -(destVolume - curVolume), price, -vol);
      avgPrice = ap.avgPrice;
      curVolume = std::abs(ap.lastVolume);
    }
  }
};

}  // namespace trading

#endif  // __TRADINGCORE_ORDER_H__