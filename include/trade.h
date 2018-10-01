#ifndef __TRADINGCORE_TRADE_H__
#define __TRADINGCORE_TRADE_H__

#include <string>
// #include "exchange.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

// enum TRADE_TYPE { TRADE_MAKER = 0, TRADE_TAKER };

enum TRADE_SIDE { TRADE_BUY = 0, TRADE_SELL };

// template <typename MoneyType, typename VolumeType>
// struct TradeSideDetail {
//   TRADE_SIDE side;

//   // last value
//   MoneyType value;

//   MoneyType srcPrice;

//   MoneyType fee;
// };

template <typename MoneyType, typename VolumeType>
struct Trade {
  // typedef TradeSideDetail<MoneyType, VolumeType> TradeSideDetailT;

  TradeID tradeID;

  // TRADE_TYPE tradeType;
  TRADE_SIDE tradeSide;

  OrderID buyOrderID;
  OrderID sellOrderID;

  time_t ct;

  VolumeType vol;
  MoneyType price;

  // TradeSideDetailT buyDetail;
  // TradeSideDetailT sellDetail;
};

}  // namespace trading

#endif  // __TRADINGCORE_TRADE_H__