#ifndef __TRADINGCORE_EXCHANGE_H__
#define __TRADINGCORE_EXCHANGE_H__

#include "category.h"

namespace trading {

template <typename PriceType, typename VolumeType>
class Exchange {
 public:
  typedef CategoryConfig<PriceType, VolumeType> CategoryConfigT;
  typedef CategoryInfo<PriceType, VolumeType> CategoryInfoT;
  typedef CategoryMgr<PriceType, VolumeType> CategoryMgrT;

 public:
  Exchange() {}
  ~Exchange() {}

 public:
  CategoryConfigT& newCategory(const char* code, VolumeType unit,
                               PriceType price) {
    CategoryConfigT& cfg = m_mgrCategory.newCategory(code);

    cfg.setUnitPrice(unit, price);

    return cfg;
  }

 protected:
  CategoryMgrT m_mgrCategory;
};

}  // namespace trading

#endif  // __TRADINGCORE_EXCHANGE_H__