#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include <vector>
#include "category.h"
#include "order.h"
#include "trade.h"
#include "trademgr.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class ExchangeCategory {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef Order<MoneyType, VolumeType> OrderT;
  typedef std::vector<OrderT*> OrderList;
  typedef typename OrderList::iterator OrderListIter;
  typedef Trade<MoneyType, VolumeType> TradeT;
  typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;

 public:
  ExchangeCategory(const char* name, CategoryConfigT& cfg)
      : m_nameCategory(name),
        m_mgrTrade(*(getTradeMgr<MoneyType, VolumeType>())),
        m_cfgCategory(cfg) {}
  virtual ~ExchangeCategory() {}

 public:
  virtual void onTick(time_t ct, WalletT& wallet) = 0;

 public:
  const char* getName() const { return m_nameCategory.c_str(); }

  void pushOrder(OrderT* pOrder, time_t ct) {
    if (pOrder->orderSide == ORDER_BUY) {
      _insBuyOrder(pOrder);
    } else {
      _insSellOrder(pOrder);
    }
  }

 protected:
  void _insBuyOrder(OrderT* pOrder) {
    assert(pOrder->orderSide == ORDER_BUY);

    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();
         ++it) {
      if (pOrder->destPrice > (*it)->destPrice) {
        m_lstOrderBuy.insert(pOrder);

        return;
      }
    }

    m_lstOrderBuy.push_back(pOrder);
  }

  void _insSellOrder(OrderT* pOrder) {
    assert(pOrder->orderSide == ORDER_SELL);

    for (OrderListIter it = m_lstOrderSell.begin(); it != m_lstOrderSell.end();
         ++it) {
      if (pOrder->destPrice < (*it)->destPrice) {
        m_lstOrderSell.insert(pOrder);

        return;
      }
    }

    m_lstOrderSell.push_back(pOrder);
  }

  void _procBuyOrder(OrderT* pOrder, time_t ct) {
    assert(pOrder->orderSide == ORDER_BUY);

    for (OrderListIter it = m_lstOrderSell.begin();
         it != m_lstOrderSell.end();) {
      if (pOrder->destPrice >= (*it)->destPrice) {
        _procTransaction(pOrder, *it, ct);

        if ((*it)->curVolume <= 0) {
          it = m_lstOrderSell.erase(it);
        } else {
          ++it;
        }

        if (pOrder->curVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procSellOrder(OrderT* pOrder, time_t ct) {
    assert(pOrder->orderSide == ORDER_SELL);

    for (OrderListIter it = m_lstOrderBuy.begin(); it != m_lstOrderBuy.end();) {
      if (pOrder->destPrice <= (*it)->destPrice) {
        _procTransaction(pOrder, *it, ct);

        if ((*it)->curVolume <= 0) {
          it = m_lstOrderBuy.erase(it);
        } else {
          ++it;
        }

        if (pOrder->curVolume <= 0) {
          return;
        }
      } else {
        return;
      }
    }
  }

  void _procTransaction(OrderT* pSrc, OrderT* pDest, time_t ct) {
    assert(pSrc->orderSide != pDest->orderSide);
    assert(pSrc->curVolume > 0);
    assert(pDest->curVolume > 0);

    TradeT& trade = _newTrade();

    // use dest price
    trade.price = pDest->destPrice;

    // use min vol
    trade.vol = std::min(pSrc->curVolume, pDest->curVolume);

    // use src side
    if (pSrc->orderSide == ORDER_BUY) {
      trade.tradeSide = TRADE_BUY;

      trade.buyOrderID = pSrc->orderID;
      trade.sellOrderID = pDest->orderID;
    } else {
      trade.tradeSide = TRADE_SELL;

      trade.sellOrderID = pSrc->orderID;
      trade.buyOrderID = pDest->orderID;
    }

    pSrc->procTransaction(m_cfgCategory, trade.price, trade.vol);
    pDest->procTransaction(m_cfgCategory, trade.price, trade.vol);

    trade.ct = ct;
  }

  TradeT& _newTrade() { return m_mgrTrade.newTrade(); }

 protected:
  TradeMgrT& m_mgrTrade;
  CategoryConfigT& m_cfgCategory;

  std::string m_nameCategory;

  OrderList m_lstOrderBuy;
  OrderList m_lstOrderSell;
};

template <typename MoneyType, typename VolumeType>
class Exchange {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef CategoryInfo<MoneyType, VolumeType> CategoryInfoT;
  typedef CategoryMgr<MoneyType, VolumeType> CategoryMgrT;
  typedef std::map<std::string, std::string> CategoryCodeMap;
  typedef typename CategoryCodeMap::iterator CategoryCodeMapIter;
  typedef ExchangeCategory<MoneyType, VolumeType> ExchangeCategoryT;
  typedef std::map<std::string, ExchangeCategoryT*> ExchangeCategoryMap;
  typedef typename ExchangeCategoryMap::iterator ExchangeCategoryMapIter;

 public:
  Exchange() {}
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

  CategoryConfigT& getCategoryConfigWithName(const char* name) {
    CategoryCodeMapIter it = m_mapCategoryCode.find(name);
    assert(it != m_mapCategoryCode.end());

    CategoryConfigT* pCfg = m_mgrCategory.getConfig(it->second.c_str());
    assert(pCfg != NULL);

    return *pCfg;
  }

  void onTick(time_t ct, WalletT& wallet) {
    for (ExchangeCategoryMapIter it = m_mapCategory.begin();
         it != m_mapCategory.end(); ++it) {
      it->second->onTick(ct, wallet,
                         getCategoryConfigWithName(it->second->getName()));
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