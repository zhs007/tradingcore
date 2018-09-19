#ifndef __TRADINGCORE_ORDER_H__
#define __TRADINGCORE_ORDER_H__

#include <string>
#include <vector>
#include "exchange.h"
#include "trade.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

enum ORDER_TYPE { ORDER_LIMIT = 0 };

template <typename MoneyType, typename VolumeType>
struct Order {
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef std::vector<TradeT> TradeList;

  OrderID orderID;

  ORDER_TYPE orderType;

  MoneyType destPrice;
  VolumeType destVolume;
  time_t bt;

  MoneyType avgPrice;
  VolumeType curVolume;
  time_t ct;

  TradeList lstTrade;
};

}  // namespace trading

#endif  // __TRADINGCORE_ORDER_H__