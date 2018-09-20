#ifndef __TRADINGCORE_SIMEXCHANGE_H__
#define __TRADINGCORE_SIMEXCHANGE_H__

#include "candle.h"
#include "csvfile.h"
#include "exchange.h"
#include "order.h"
#include "ordermgr.h"
#include "trade.h"
#include "trademgr.h"
#include "utils.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class SimExchangeCategory : public ExchangeCategory<MoneyType, VolumeType> {
 public:
  typedef ExchangeCategory<MoneyType, VolumeType> ExchangeCategoryT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;
  typedef typename ExchangeCategoryT::OrderList OrderList;
  typedef typename ExchangeCategoryT::OrderListIter OrderListIter;

 public:
  SimExchangeCategory() : m_curCandleIndex(0) {}
  virtual ~SimExchangeCategory() {}

 public:
  virtual void onTick(WalletT& wallet, time_t bt, time_t ct) {
    for (int i = m_curCandleIndex; i < m_lstCandle.getLength(); ++i) {
      const CandleDataT& cd = m_lstCandle.get(i);
      if (cd.curtime > ct) {
        return;
      }

      if (cd.curtime > bt && cd.curtime <= ct) {
        VolumeType lastVol = cd.volume;

        if (lastVol > 0) {
          _procCandleBuyOrderList(cd, lastVol);
        }

        if (lastVol > 0) {
          _procCandleSellOrderList(cd, lastVol);
        }
      }

      m_curCandleIndex = i;
    }
  }

 public:
  bool loadCandleCSVFile(const char* filename, CSVConfig& cfg) {
    m_lstCandle.clear();

    CSVFile csv;
    bool loadok = csv.load(filename);
    if (!loadok) {
      return false;
    }

    for (int i = 0; i < csv.getLength(); ++i) {
      time_t curtime;
      MoneyType open, close, high, low;
      VolumeType volume;
      VolumeType openInterest;

      const char* strTime = csv.get(i, cfg.head.curtime.c_str());
      const char* strOpen = csv.get(i, cfg.head.open.c_str());
      const char* strClose = csv.get(i, cfg.head.close.c_str());
      const char* strHigh = csv.get(i, cfg.head.high.c_str());
      const char* strLow = csv.get(i, cfg.head.low.c_str());
      const char* strVolume = csv.get(i, cfg.head.volume.c_str());
      const char* strOpenInterest = csv.get(i, cfg.head.openInterest.c_str());

      if (cfg.timeType == CSVTIME_FORMAT_STR) {
        tm curtm;
        strptime(strTime, "%Y-%m-%d %H:%M:%S", &curtm);

        curtm.tm_isdst = -1;
        curtime = mktime(&curtm);
      } else if (cfg.timeType == CSVTIME_TIMESTAMP) {
        curtime = std::stoll(strTime);
      }

      double o = std::stod(strOpen);
      open = o * cfg.scalePrice;

      double c = std::stod(strClose);
      close = c * cfg.scalePrice;

      double h = std::stod(strHigh);
      high = h * cfg.scalePrice;

      double l = std::stod(strLow);
      low = l * cfg.scalePrice;

      double v = std::stod(strVolume);
      volume = v * cfg.scaleVolume;

      double oi = std::stod(strOpenInterest);
      openInterest = oi * cfg.scaleVolume;

      m_lstCandle.push(curtime, open, close, high, low, volume, openInterest);
    }
  }

 protected:
  void _procCandle(OrderT& order, CandleDataT& candle, VolumeType& lastVol) {
    if (order.side == ORDER_BUY) {
      _procCandleBuyOrder(order, candle, lastVol);
    } else {
      _procCandleSellOrder(order, candle, lastVol);
    }
  }

  void _procCandleBuyOrder(OrderT& order, CandleDataT& candle,
                           VolumeType& lastVol) {
    assert(order.side == ORDER_BUY);

    if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(this->m_cfgCategory, order.destPrice, vol);

      lastVol -= vol;
    }
  }

  void _procCandleSellOrder(OrderT& order, CandleDataT& candle,
                            VolumeType& lastVol) {
    assert(order.side == ORDER_SELL);

    if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
      VolumeType vol = std::min(order.curVolume, lastVol);

      order.procTransaction(this->m_cfgCategory, order.destPrice, vol);

      lastVol -= vol;
    }
  }

  void _procCandleBuyOrderList(CandleDataT& candle, VolumeType& lastVol) {
    int off = 1;
    for (int i = 0; i < this->m_lstOrderBuy.size(); i += off) {
      off = 1;
      OrderT* pOrder = this->m_lstOrderBuy[i];

      _procCandle(*pOrder, candle, lastVol);

      if (pOrder->curVolume <= 0) {
        this->_deleteOrder(pOrder->orderID, pOrder->side);
        off = 0;
      }

      if (lastVol <= 0) {
        return;
      }
    }
  }

  void _procCandleSellOrderList(CandleDataT& candle, VolumeType& lastVol) {
    int off = 1;
    for (int i = 0; i < this->m_lstOrderSell.size(); i += off) {
      off = 1;
      OrderT* pOrder = this->m_lstOrderSell[i];

      _procCandle(*pOrder, candle, lastVol);

      if (pOrder->curVolume <= 0) {
        this->_deleteOrder(pOrder->orderID, pOrder->side);
        off = 0;
      }

      if (lastVol <= 0) {
        return;
      }
    }
  }

 protected:
  CandleListT m_lstCandle;
  int m_curCandleIndex;
};

template <typename MoneyType, typename VolumeType>
class SimExchange : public Exchange<MoneyType, VolumeType> {
 public:
  typedef Exchange<MoneyType, VolumeType> ExchangeT;

 public:
  SimExchange() {}
  ~SimExchange() {}

 public:
 protected:
};

}  // namespace trading

#endif  // __TRADINGCORE_SIMEXCHANGE_H__