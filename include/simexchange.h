#ifndef __TRADINGCORE_SIMEXCHANGE_H__
#define __TRADINGCORE_SIMEXCHANGE_H__

#include "candle.h"
#include "csvfile.h"
#include "exchange.h"
#include "order.h"
#include "orderlogic.h"
#include "ordermgr.h"
#include "trade.h"
#include "trademgr.h"
#include "utils.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType>
class SimExchangeCategory
    : public ExchangeCategory<MoneyType, VolumeType, ValueType> {
 public:
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
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
  typedef OrderLogic<MoneyType, VolumeType> OrderLogicT;

 public:
  SimExchangeCategory(OrderMgrT& mgrOrder, TradeMgrT& mgrTrade,
                      const char* name, const CategoryConfigT& cfg,
                      OrderLogicT& orderLogic)
      : ExchangeCategoryT(mgrOrder, mgrTrade, name, cfg),
        m_curCandleIndex(0),
        m_orderLogic(orderLogic) {}
  virtual ~SimExchangeCategory() {}

 public:
  virtual void onTick(WalletT& wallet, time_t bt, time_t ct) {
    for (int i = m_curCandleIndex; i < this->m_lstCandle.getLength(); ++i) {
      const CandleDataT& cd = this->m_lstCandle.get(i);
      if (cd.curtime > ct) {
        return;
      }

      printf("%d %ld %ld \n", i, cd.curtime, ct);

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

  virtual void onCandle(WalletT& wallet, int candleIndex) {
    assert(candleIndex >= 0 && candleIndex < this->m_lstCandle.getLength());

    const CandleDataT& cd = this->m_lstCandle.get(candleIndex);
    // if (cd.curtime > ct) {
    // return;
    // }

    // printf("%d %ld %ld \n", candleIndex, cd.curtime, ct);

    // if (cd.curtime > bt && cd.curtime <= ct) {
    VolumeType lastVol = cd.volume;

    if (lastVol > 0) {
      _procCandleBuyOrderList(cd, lastVol);
    }

    if (lastVol > 0) {
      _procCandleSellOrderList(cd, lastVol);
    }
    // }

    m_curCandleIndex = candleIndex;
  }

 public:
  bool loadCandleCSVFile(const char* filename, CSVConfig& cfg) {
    this->m_lstCandle.clear();

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
        curtime = str2time(strTime);
        // printf("%s", strTime);
        // printf("%ld ", curtime);
        // tm curtm;
        // strptime(strTime, "%Y-%m-%d %H:%M:%S", &curtm);

        // curtm.tm_isdst = -1;
        // curtime = mktime(&curtm);
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

      this->m_lstCandle.push(curtime, open, close, high, low, volume,
                             openInterest);
    }

    return true;
  }

 protected:
  // void _procCandle(OrderT& order, CandleDataT& candle, VolumeType& lastVol) {
  //   if (order.side == ORDER_BUY) {
  //     _procCandleBuyOrder(order, candle, lastVol);
  //   } else {
  //     _procCandleSellOrder(order, candle, lastVol);
  //   }
  // }

  // void _procCandleBuyOrder(OrderT& order, CandleDataT& candle,
  //                          VolumeType& lastVol) {
  //   assert(order.side == ORDER_BUY);

  //   if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
  //     VolumeType vol = std::min(order.curVolume, lastVol);

  //     order.procTransaction(this->m_cfgCategory, order.destPrice, vol);

  //     lastVol -= vol;
  //   }
  // }

  // void _procCandleSellOrder(OrderT& order, CandleDataT& candle,
  //                           VolumeType& lastVol) {
  //   assert(order.side == ORDER_SELL);

  //   if (order.destPrice >= candle.low && order.destPrice <= candle.high) {
  //     VolumeType vol = std::min(order.curVolume, lastVol);

  //     order.procTransaction(this->m_cfgCategory, order.destPrice, vol);

  //     lastVol -= vol;
  //   }
  // }

  void _procCandleBuyOrderList(const CandleDataT& candle, VolumeType& lastVol) {
    int off = 1;
    for (int i = 0; i < this->m_lstOrderBuy.size(); i += off) {
      off = 1;
      OrderT* pOrder = this->m_lstOrderBuy[i];

      MoneyType curp = -1;
      VolumeType bv = lastVol;
      m_orderLogic.procCandle(this->m_cfgCategory, *pOrder, candle, lastVol,
                              curp);
      if (lastVol < bv) {
        assert(curp > 0);
        TradeT& trade = this->_newTrade();
        // use dest price
        trade.price = curp;

        // use min vol
        trade.vol = bv - lastVol;

        trade.tradeSide = TRADE_BUY;

        trade.buyOrderID = pOrder->orderID;
        trade.sellOrderID = 0;

        pOrder->lstTrade.push_back(trade);
      }

      if (pOrder->lastVolume <= 0) {
        this->_deleteOrder(pOrder->orderID, pOrder->orderSide);
        off = 0;
      }

      if (lastVol <= 0) {
        return;
      }
    }
  }

  void _procCandleSellOrderList(const CandleDataT& candle,
                                VolumeType& lastVol) {
    int off = 1;
    for (int i = 0; i < this->m_lstOrderSell.size(); i += off) {
      off = 1;
      OrderT* pOrder = this->m_lstOrderSell[i];

      MoneyType curp = -1;
      VolumeType bv = lastVol;
      m_orderLogic.procCandle(this->m_cfgCategory, *pOrder, candle, lastVol,
                              curp);
      if (lastVol < bv) {
        assert(curp > 0);
        TradeT& trade = this->_newTrade();
        // use dest price
        trade.price = curp;

        // use min vol
        trade.vol = bv - lastVol;

        trade.tradeSide = TRADE_SELL;

        trade.sellOrderID = pOrder->orderID;
        trade.buyOrderID = 0;

        pOrder->lstTrade.push_back(trade);
      }

      if (pOrder->lastVolume <= 0) {
        this->_deleteOrder(pOrder->orderID, pOrder->orderSide);
        off = 0;
      }

      if (lastVol <= 0) {
        return;
      }
    }
  }

 protected:
  // CandleListT m_lstCandle;
  int m_curCandleIndex;
  OrderLogicT& m_orderLogic;
};

template <typename MoneyType, typename VolumeType, typename ValueType>
class SimExchange : public Exchange<MoneyType, VolumeType, ValueType> {
 public:
  typedef Exchange<MoneyType, VolumeType, ValueType> ExchangeT;
  typedef OrderLogic<MoneyType, VolumeType> OrderLogicT;
  typedef SimExchangeCategory<MoneyType, VolumeType, ValueType>
      SimExchangeCategoryT;
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;

 public:
  SimExchange(const char* name, OrderLogicT& orderLogic)
      : ExchangeT(name), m_orderLogic(orderLogic) {}
  ~SimExchange() {}

 public:
  void addSimExchangeCategory(OrderMgrT& mgrOrder, TradeMgrT& mgrTrade,
                              const char* codeCategory,
                              const char* nameCategory, const char* filename,
                              CSVConfig& cfg) {
    SimExchangeCategoryT* pSEC = new SimExchangeCategoryT(
        mgrOrder, mgrTrade, nameCategory,
        this->getCategoryConfigWithName(nameCategory), m_orderLogic);
    this->m_mapCategory[nameCategory] = pSEC;

    pSEC->loadCandleCSVFile(filename, cfg);
  }

  // void start(WalletT& wallet, time_t bt, time_t et, time_t ot) {
  //   for (time_t ct = bt; ct <= et; ct += ot) {
  //     this->onTick(wallet, ct);
  //   }
  // }

 protected:
  OrderLogicT& m_orderLogic;
};

}  // namespace trading

#endif  // __TRADINGCORE_SIMEXCHANGE_H__