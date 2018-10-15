#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include <algorithm>
#include <functional>
#include <vector>
#include "candle.h"
#include "category.h"
#include "indicator.h"
#include "indicatormgr.h"
#include "mulindicatordatamgr.h"
#include "order.h"
#include "ordermgr.h"
#include "trade.h"
#include "trademgr.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType>
class ExchangeCategory {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef std::vector<OrderT*> OrderList;
  typedef typename OrderList::iterator OrderListIter;
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef std::map<std::string, IndicatorT*> IndicatorMap;
  typedef typename IndicatorMap::iterator IndicatorMapIter;
  typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;
  typedef MulIndicatorDataMgr<ValueType> MulIndicatorDataMgrT;

 public:
  ExchangeCategory(OrderMgrT& mgrOrder, TradeMgrT& mgrTrade, const char* name,
                   const CategoryConfigT& cfg)
      : m_nameCategory(name),
        m_mgrTrade(mgrTrade),
        m_cfgCategory(cfg),
        m_mgrOrder(mgrOrder) {}
  virtual ~ExchangeCategory() {}

 public:
  virtual void onTick(WalletT& wallet, time_t bt, time_t ct) = 0;

  virtual void onCandle(WalletT& wallet, int candleIndex) = 0;

  virtual void onNewOrder(WalletT& wallet, OrderT& order, time_t ct) {
    order.categoryName = m_nameCategory;

    if (order.orderSide == ORDER_BUY) {
      _procBuyOrder(wallet, order, ct);
      if (order.lastVolume > 0) {
        _insBuyOrder(order);
      }

    } else {
      _procSellOrder(wallet, order, ct);
      if (order.lastVolume > 0) {
        _insSellOrder(order);
      }
    }
  }

 public:
  const char* getName() const { return m_nameCategory.c_str(); }

  // OrderT* newOrder(time_t ct) {
  //   OrderT* pOrder = m_mgrOrder.newOrder();
  //   pOrder->ct = ct;
  //   return pOrder;
  // }

  void newLimitOrder(WalletT& wallet, ORDER_SIDE side, MoneyType price,
                     VolumeType volume, time_t t) {
    OrderT& order = m_mgrOrder.newOrder();
    order.setLimitOrder(side, price, volume, t);

    onNewOrder(wallet, order, t);
  }

  void clearOrder(ORDER_SIDE side) {
    if (side == ORDER_BUY) {
      m_lstOrderBuy.clear();
    } else {
      m_lstOrderSell.clear();
    }
  }

  // void addOrder(OrderT& order) { onNewOrder(order, order.ct); }

  // do not in foreach iterator!!!
  void _deleteOrder(OrderID orderID, ORDER_SIDE side) {
    if (side == ORDER_BUY) {
      for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();
           ++it) {
        if (orderID == (*it)->orderID) {
          m_lstOrderBuy.erase(it);

          return;
        }
      }
    } else {
      for (OrderListIter it = m_lstOrderSell.begin();
           it != m_lstOrderSell.end(); ++it) {
        if (orderID == (*it)->orderID) {
          m_lstOrderSell.erase(it);

          return;
        }
      }
    }
  }
  // void pushOrder(OrderT* pOrder, time_t ct) {
  //   if (pOrder->orderSide == ORDER_BUY) {
  //     _insBuyOrder(pOrder);
  //   } else {
  //     _insSellOrder(pOrder);
  //   }
  // }

  void addIndicator(MulIndicatorDataMgrT& mgrMulIndicatorData,
                    IndicatorMgrT& mgr, const char* code, const char* name,
                    IndicatorParam& param) {
    // auto pMgr = getIndicatorMgr<MoneyType, VolumeType, ValueType>();

    auto pIndicator =
        mgr.newIndicator(mgrMulIndicatorData, code, param, m_lstCandle);
    if (pIndicator != NULL) {
      pIndicator->build();

      m_mapIndicator[name] = pIndicator;
    }
  }

  IndicatorT* getIndicator(const char* name) {
    IndicatorMapIter it = m_mapIndicator.find(name);
    if (it != m_mapIndicator.end()) {
      return (it->second);
    }

    return NULL;
  }

  const CandleDataT* findTimeEx(int& index, time_t ct) const {
    return m_lstCandle.findTimeEx(index, ct);
  }

  int getCandleLength() const { return m_lstCandle.getLength(); }

  const CandleDataT* getCandleData(int index) const {
    const CandleDataT& cd = m_lstCandle.get(index);
    return &cd;
  }

  void _buildOrderList(OrderList& lst) {
    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();
         ++it) {
      lst.push_back(*it);
    }

    for (OrderListIter it = m_lstOrderSell.begin(); it != m_lstOrderSell.end();
         ++it) {
      lst.push_back(*it);
    }

    // std::sort(lst.begin(), lst.end(), FuncOrderCmp);
  }

 protected:
  // void _deleteOrder(OrderT& order) {
  //   m_mgrOrder.deleteOrder(&order);
  // }

  void _insBuyOrder(OrderT& order) {
    assert(order.orderSide == ORDER_BUY);

    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();
         ++it) {
      if (order.destPrice > (*it)->destPrice) {
        m_lstOrderBuy.insert(it, &order);

        return;
      }
    }

    m_lstOrderBuy.push_back(&order);
  }

  void _insSellOrder(OrderT& order) {
    assert(order.orderSide == ORDER_SELL);

    for (OrderListIter it = m_lstOrderSell.begin(); it != m_lstOrderSell.end();
         ++it) {
      if (order.destPrice < (*it)->destPrice) {
        m_lstOrderSell.insert(it, &order);

        return;
      }
    }

    m_lstOrderSell.push_back(&order);
  }

  void _procBuyOrder(WalletT& wallet, OrderT& order, time_t ct) {
    assert(order.orderSide == ORDER_BUY);

    for (OrderListIter it = m_lstOrderSell.begin();
         it != m_lstOrderSell.end();) {
      if (order.destPrice >= (*it)->destPrice) {
        _procTransaction(wallet, order, **it, ct);

        if ((*it)->lastVolume <= 0) {
          it = m_lstOrderSell.erase(it);
        } else {
          ++it;
        }

        if (order.lastVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procSellOrder(WalletT& wallet, OrderT& order, time_t ct) {
    assert(order.orderSide == ORDER_SELL);

    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();) {
      if (order.destPrice <= (*it)->destPrice) {
        _procTransaction(wallet, order, **it, ct);

        if ((*it)->lastVolume <= 0) {
          it = m_lstOrderBuy.erase(it);
        } else {
          ++it;
        }

        if (order.lastVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procTransaction(WalletT& wallet, OrderT& src, OrderT& dest, time_t ct) {
    assert(src.orderSide != dest.orderSide);
    assert(src.lastVolume > 0);
    assert(dest.lastVolume > 0);

    TradeT& trade = _newTrade();

    // use dest price
    trade.price = dest.destPrice;

    // use min vol
    trade.vol = std::min(src.lastVolume, dest.lastVolume);

    // use src side
    if (src.orderSide == ORDER_BUY) {
      trade.tradeSide = TRADE_BUY;

      trade.buyOrderID = src.orderID;
      trade.sellOrderID = dest.orderID;
    } else {
      trade.tradeSide = TRADE_SELL;

      trade.sellOrderID = src.orderID;
      trade.buyOrderID = dest.orderID;
    }

    src.procTransaction(m_cfgCategory, trade.price, trade.vol);
    dest.procTransaction(m_cfgCategory, trade.price, trade.vol);

    src.lstTrade.push_back(trade);
    dest.lstTrade.push_back(trade);

    trade.ct = ct;

    onTrade(wallet, trade);
  }

  TradeT& _newTrade() { return m_mgrTrade.newTrade(); }

  void onTrade(WalletT& wallet, TradeT& trade) {
    trade.categoryName = m_nameCategory;
    
    if (trade.tradeSide == TRADE_BUY) {
      wallet.chgCategoryVolume(m_nameCategory.c_str(), trade.price, trade.vol);
    } else {
      wallet.chgCategoryVolume(m_nameCategory.c_str(), trade.price, -trade.vol);
    }
  }

 protected:
  OrderMgrT& m_mgrOrder;
  TradeMgrT& m_mgrTrade;
  const CategoryConfigT& m_cfgCategory;

  std::string m_nameCategory;

  OrderList m_lstOrderBuy;
  OrderList m_lstOrderSell;

  CandleListT m_lstCandle;
  IndicatorMap m_mapIndicator;
};

template <typename MoneyType, typename VolumeType, typename ValueType>
class Exchange {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef CategoryInfo<MoneyType, VolumeType> CategoryInfoT;
  typedef CategoryMgr<MoneyType, VolumeType> CategoryMgrT;
  // typedef std::map<std::string, std::string> CategoryCodeMap;
  // typedef typename CategoryCodeMap::iterator CategoryCodeMapIter;
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
  typedef std::map<std::string, ExchangeCategoryT*> ExchangeCategoryMap;
  typedef typename ExchangeCategoryMap::iterator ExchangeCategoryMapIter;
  typedef std::function<void(ExchangeCategoryT&)> FuncForEachECT;
  typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;
  typedef MulIndicatorDataMgr<ValueType> MulIndicatorDataMgrT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef std::vector<OrderT*> OrderList;
  typedef typename OrderList::iterator OrderListIter;
  typedef FuncOrderCmp<MoneyType, VolumeType> FuncOrderCmpT;

 public:
  Exchange(CategoryMgrT& mgrCategory, const char* name)
      : m_mgrCategory(mgrCategory), m_nameExchange(name) {}
  ~Exchange() {}

 public:
  CategoryConfigT& newCategoryConfig(const char* code, VolumeType unit,
                                     MoneyType price) {
    CategoryConfigT& cfg = m_mgrCategory.newCategory(code);

    cfg.setUnitPrice(unit, price);

    return cfg;
  }

  // void addCategory(const char* name, const char* code) {
  //   m_mapCategoryCode[name] = code;
  // }

  const CategoryConfigT& getCategoryConfig(const char* code) {
    // CategoryCodeMapIter it = m_mapCategoryCode.find(name);
    // assert(it != m_mapCategoryCode.end());

    const CategoryConfigT* pCfg = m_mgrCategory.getConfig(code);
    assert(pCfg != NULL);

    return *pCfg;
  }

  void addExchangeCategory(ExchangeCategoryT& ec) {
    m_mapCategory[ec.getName()] = &ec;
  }

  void onTick(WalletT& wallet, time_t bt, time_t ct) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      it->second->onTick(wallet, bt, ct);
    }
  }

  void onCandle(WalletT& wallet, int candleIndex) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      it->second->onCandle(wallet, candleIndex);
    }
  }

  int getCandleLength() {
    int len = -1;
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      int cl = it->second->getCandleLength();
      if (len == -1) {
        len = cl;
      }

      assert(len == cl);
    }

    return len;
  }

  const char* getName() const { return m_nameExchange.c_str(); }

  // CategoryMgrT& getCategoryMgr() { return m_mgrCategory; }

  void addIndicator(MulIndicatorDataMgrT& mgrMulIndicatorData,
                    IndicatorMgrT& mgr, const char* code, const char* name,
                    IndicatorParam& param) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      it->second->addIndicator(mgrMulIndicatorData, mgr, code, name, param);
    }
  }

  void forEach(FuncForEachECT func) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      func(*it->second);
    }
  }

  void buildOrderList(OrderList& lst) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      (it->second)->_buildOrderList(lst);
    }

    FuncOrderCmpT func;
    std::sort(lst.begin(), lst.end(), func);
  }

 protected:
  CategoryMgrT& m_mgrCategory;
  // CategoryCodeMap m_mapCategoryCode;
  ExchangeCategoryMap m_mapCategory;
  std::string m_nameExchange;
};

}  // namespace trading

#endif  // __TRADINGCORE_EXCHANGE_H__