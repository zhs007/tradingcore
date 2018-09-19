#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include "category.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class ExchangeCategory {
 public:
  typedef Wallet<MoneyType, VolumeType> WalletT;
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;

 public:
  ExchangeCategory(const char* name) : m_nameCategory(name) {}
  virtual ~ExchangeCategory() {}

 public:
  virtual void onTick(time_t ct, WalletT& wallet, CategoryConfigT& cfg) = 0;

 public:
  const char* getName() const { return m_nameCategory.c_str(); }

 protected:
  std::string m_nameCategory;
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