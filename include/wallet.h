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
  typedef typename CategoryMap::iterator CategoryMapIter;

 public:
  Wallet(CategoryMgrT& mgr)
      : m_mgr(mgr), m_money(0), m_deposit(0), m_totalFee(0) {}
  ~Wallet() {}

 public:
  MoneyType countTotalValue() {
    MoneyType totalvalue = 0;
    for (CategoryMapIter it = m_mapCategory.begin(); it != m_mapCategory.end();
         ++it) {
      totalvalue += m_mgr.countCategoryValue(
          it->second.code.c_str(), it->second.vol, it->second.curPrice);
    }

    return totalvalue;
  }

  void setMoney(MoneyType money) { m_money = money; }

  void initCategoryInfo(const char* name, const char* code, VolumeType vol,
                        MoneyType avgPrice) {
    CategoryPair p;
    p.first = name;
    p.second.code = code;
    p.second.vol = vol;
    p.second.avgPrice = avgPrice;

    m_mapCategory.insert(p);
  }

  void chgMoney(MoneyType off) { m_money += off; }

  void chgCategoryVolume(const char* name, MoneyType curprice, VolumeType off) {
    if (m_mapCategory.has(name)) {
      CategoryInfoT& ci = m_mapCategory[name];

      const CategoryConfigT* pCfg = m_mgr.getConfig(ci.code.c_str());
      if (pCfg == NULL) {
        return;
      }

      ci.chgVolume(*pCfg, curprice, off);
    }
  }

  void chgCategoryPrice(const char* name, MoneyType curprice) {
    CategoryMapIter it = m_mapCategory.find(name);
    if (it != m_mapCategory.end()) {
      CategoryInfoT& ci = it->second;

      ci.curPrice = curprice;
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