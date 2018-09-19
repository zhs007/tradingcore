#ifndef __TRADINGCORE_WALLET_H__
#define __TRADINGCORE_WALLET_H__

#include <map>
#include <string>
#include "category.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class Wallet {
 public:
  typedef CategoryInfo<MoneyType, VolumeType> CategoryInfoT;
  typedef std::pair<std::string, CategoryInfoT> CategoryPair;
  typedef std::map<std::string, CategoryInfoT> CategoryMap;
  typedef CategoryMgr<MoneyType, VolumeType> CategoryMgrT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;

 public:
  Wallet(CategoryMgrT& mgr)
      : m_mgr(mgr), m_money(0), m_deposit(0), m_totalFee(0) {}
  ~Wallet() {}

 public:
  MoneyType countTotalValue() {}

  void setMoney(MoneyType money) { m_money = money; }

  void initCategoryInfo(const char* code, VolumeType vol, MoneyType avgPrice) {
    CategoryPair p;
    p.first = code;
    p.second.vol = vol;
    p.second.avgPrice = avgPrice;

    m_mapCategory.insert(p);
  }

  void chgMoney(MoneyType off) { m_money += off; }

  void chgCategoryVolume(const char* code, MoneyType curprice, VolumeType off) {
    if (m_mapCategory.has(code)) {
      const CategoryConfigT* pCfg = m_mgr.getConfig(code);
      if (pCfg == NULL) {
        return;
      }

      m_mapCategory[code].chgVolume(*pCfg, curprice, off);
    }
  }

 protected:
  CategoryMgrT& m_mgr;
  MoneyType m_money;
  CategoryMap m_mapCategory;
  MoneyType m_deposit;
  MoneyType m_totalFee;
};

}  // namespace trading

#endif  // __TRADINGCORE_WALLET_H__