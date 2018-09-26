#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include <vector>
#include "candle.h"
#include "category.h"
#include "indicator.h"
#include "indicatormgr.h"
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
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef std::map<std::string, IndicatorT*> IndicatorMap;

 public:
  ExchangeCategory(const char* name, const CategoryConfigT& cfg)
      : m_nameCategory(name),
        m_mgrTrade(*(getTradeMgr<MoneyType, VolumeType>())),
        m_cfgCategory(cfg),
        m_mgrOrder(*(getOrderMgr<MoneyType, VolumeType>())) {}
  virtual ~ExchangeCategory() {}

 public:
  virtual void onTick(WalletT& wallet, time_t bt, time_t ct) = 0;

  virtual void onNewOrder(OrderT& order, time_t ct) {
    if (order.orderSide == ORDER_BUY) {
      _procBuyOrder(order, ct);
      if (order.curVolume > 0) {
        _insBuyOrder(order);
      }

    } else {
      _procSellOrder(order, ct);
      if (order.curVolume > 0) {
        _insBuyOrder(order);
      }
    }
  }

 public:
  const char* getName() const { return m_nameCategory.c_str(); }

  void newOrder(OrderT& order, time_t ct) { onNewOrder(order, ct); }

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

  void addIndicator(const char* name) {
    auto pMgr = getIndicatorMgr<MoneyType, VolumeType, ValueType>();

    auto pIndicator = pMgr->newIndicator(name, m_lstCandle);
    if (pIndicator != NULL) {
      pIndicator->build();

      m_mapIndicator[name] = pIndicator;
    }
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
        m_lstOrderSell.insert(&order);

        return;
      }
    }

    m_lstOrderSell.push_back(&order);
  }

  void _procBuyOrder(OrderT& order, time_t ct) {
    assert(order.orderSide == ORDER_BUY);

    for (OrderListIter it = m_lstOrderSell.begin();
         it != m_lstOrderSell.end();) {
      if (order.destPrice >= (*it)->destPrice) {
        _procTransaction(order, **it, ct);

        if ((*it)->curVolume <= 0) {
          it = m_lstOrderSell.erase(it);
        } else {
          ++it;
        }

        if (order.curVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procSellOrder(OrderT& order, time_t ct) {
    assert(order.orderSide == ORDER_SELL);

    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();) {
      if (order.destPrice <= (*it)->destPrice) {
        _procTransaction(order, **it, ct);

        if ((*it)->curVolume <= 0) {
          it = m_lstOrderBuy.erase(it);
        } else {
          ++it;
        }

        if (order.curVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procTransaction(OrderT& src, OrderT& dest, time_t ct) {
    assert(src.orderSide != dest.orderSide);
    assert(src.curVolume > 0);
    assert(dest.curVolume > 0);

    TradeT& trade = _newTrade();

    // use dest price
    trade.price = dest.destPrice;

    // use min vol
    trade.vol = std::min(src.curVolume, dest.curVolume);

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

    trade.ct = ct;
  }

  TradeT& _newTrade() { return m_mgrTrade.newTrade(); }

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
  typedef std::map<std::string, std::string> CategoryCodeMap;
  typedef typename CategoryCodeMap::iterator CategoryCodeMapIter;
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
  typedef std::map<std::string, ExchangeCategoryT*> ExchangeCategoryMap;
  typedef typename ExchangeCategoryMap::iterator ExchangeCategoryMapIter;

 public:
  Exchange(const char* name) : m_nameExchange(name) {}
  ~Exchange() {}

 public:
  CategoryConfigT& newCategoryConfig(const char* code, VolumeType unit,
                                     MoneyType price) {
    CategoryConfigT& cfg = m_mgrCategory.newCategory(code);

    cfg.setUnitPrice(unit, price);

    return cfg;
  }

  void addCategory(const char* name, const char* code) {
    m_mapCategoryCode[name] = code;
  }

  const CategoryConfigT& getCategoryConfigWithName(const char* name) {
    CategoryCodeMapIter it = m_mapCategoryCode.find(name);
    assert(it != m_mapCategoryCode.end());

    const CategoryConfigT* pCfg = m_mgrCategory.getConfig(it->second.c_str());
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
      // it->second->onTick(ct, wallet,
      //                    getCategoryConfigWithName(it->second->getName()));
    }
  }

  const char* getName() const { return m_nameExchange.c_str(); }

  CategoryMgrT& getCategoryMgr() { return m_mgrCategory; }

  void addIndicator(const char* name) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      it->second->addIndicator(name);
    }
  }

 protected:
  CategoryMgrT m_mgrCategory;
  CategoryCodeMap m_mapCategoryCode;
  ExchangeCategoryMap m_mapCategory;
  std::string m_nameExchange;
};

}  // namespace trading

#endif  // __TRADINGCORE_EXCHANGE_H__