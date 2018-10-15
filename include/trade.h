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

  std::string categoryName;

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

template <typename MoneyType, typename VolumeType>
struct FuncTradeCmp {
  typedef Trade<MoneyType, VolumeType> TradeT;

  bool operator()(TradeT* pA, TradeT* pB) const {
    assert(pA != NULL);
    assert(pB != NULL);

    return pA->ct < pB->ct;
  }
};

inline const char* getTradeSideStr(TRADE_SIDE t) {
  switch (t) {
    case TRADE_BUY:
      return "buy";
    case TRADE_SELL:
      return "sell";
  }

  return "unknow";
}

}  // namespace trading

#endif  // __TRADINGCORE_TRADE_H__