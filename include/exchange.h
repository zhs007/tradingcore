#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include "category.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class ExchangeCategory {
 public:
  ExchangeCategory() {}
  virtual ~ExchangeCategory() {}

 public:
 protected:
};

template <typename MoneyType, typename VolumeType>
class Exchange {
 public:
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef CategoryInfo<MoneyType, VolumeType> CategoryInfoT;
  typedef CategoryMgr<MoneyType, VolumeType> CategoryMgrT;
  typedef std::map<std::string, std::string> CategoryCodeMap;
  typedef ExchangeCategory<MoneyType, VolumeType> ExchangeCategoryT;
  typedef std::map<std::string, ExchangeCategoryT> ExchangeCategoryMap;

 public:
  Exchange() {}
  ~Exchange() {}

 public:
  CategoryConfigT& newCategory(const char* code, VolumeType unit,
                               MoneyType price) {
    CategoryConfigT& cfg = m_mgrCategory.newCategory(code);

    cfg.setUnitPrice(unit, price);

    return cfg;
  }

 protected:
  CategoryMgrT m_mgrCategory;
  CategoryCodeMap m_mapCategoryCode;
  ExchangeCategoryMap m_mapCategory;
};

}  // namespace trading

#endif  // __TRADINGCORE_EXCHANGE_H__