#ifndef __TRADINGCORE_ORDERLOGIC_H__
#define __TRADINGCORE_ORDERLOGIC_H__

#include <assert.h>
#include <vector>
#include "candle.h"
#include "order.h"
#include "utils.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class OrderLogic {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;

 public:
  OrderLogic() {}
  virtual ~OrderLogic() {}

 public:
  virtual void procCandle(CategoryConfigT& cfg, OrderT& order,
                          CandleDataT& candle, VolumeType& lastVol) = 0;

 protected:
};

template <typename MoneyType, typename VolumeType>
class OrderLogic_Simple : public OrderLogic<MoneyType, VolumeType> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;

 public:
  OrderLogic_Simple() {}
  virtual ~OrderLogic_Simple() {}

 public:
  virtual void procCandle(CategoryConfigT& cfg, OrderT& order,
                          CandleDataT& candle, VolumeType& lastVol) {
    if (order.side == ORDER_BUY) {
      _procCandleBuyOrder(cfg, order, candle, lastVol);
    } else {
      _procCandleSellOrder(cfg, order, candle, lastVol);
    }
  }

  void _procCandleBuyOrder(CategoryConfigT& cfg, OrderT& order,
                           CandleDataT& candle, VolumeType& lastVol) {
    assert(order.side == ORDER_BUY);

    if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(cfg, order.destPrice, vol);

      lastVol -= vol;
    }
  }

  void _procCandleSellOrder(CategoryConfigT& cfg, OrderT& order,
                            CandleDataT& candle, VolumeType& lastVol) {
    assert(order.side == ORDER_SELL);

    if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(cfg, order.destPrice, vol);

      lastVol -= vol;
    }
  }

 protected:
};

template <typename MoneyType, typename VolumeType>
class OrderLogic_Simple2 : public OrderLogic<MoneyType, VolumeType> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;

 public:
  OrderLogic_Simple2() {}
  virtual ~OrderLogic_Simple2() {}

 public:
  virtual void procCandle(CategoryConfigT& cfg, OrderT& order,
                          CandleDataT& candle, VolumeType& lastVol) {
    if (order.side == ORDER_BUY) {
      _procCandleBuyOrder(cfg, order, candle, lastVol);
    } else {
      _procCandleSellOrder(cfg, order, candle, lastVol);
    }
  }

  void _procCandleBuyOrder(CategoryConfigT& cfg, OrderT& order,
                           CandleDataT& candle, VolumeType& lastVol) {
    assert(order.side == ORDER_BUY);

    if (order.destPrice >= candle.close) {
      PriceType price = std::min(order.destPrice, candle.close);
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(cfg, price, vol);

      lastVol -= vol;
    }
  }

  void _procCandleSellOrder(CategoryConfigT& cfg, OrderT& order,
                            CandleDataT& candle, VolumeType& lastVol) {
    assert(order.side == ORDER_SELL);

    if (order.destPrice <= candle.close) {
      PriceType price = std::max(order.destPrice, candle.close);
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(cfg, order.destPrice, vol);

      lastVol -= vol;
    }
  }

 protected:
};

}  // namespace trading

#endif  // __TRADINGCORE_ORDERLOGIC_H__