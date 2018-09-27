#ifndef __TRADINGCORE_CONTEXT_H__
#define __TRADINGCORE_CONTEXT_H__

#include "indicatormgr.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
struct Context {
  typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;

  IndicatorMgrT mgrIndicator;
};

}  // namespace trading

#endif  // __TRADINGCORE_CONTEXT_H__