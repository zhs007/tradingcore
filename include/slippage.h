#ifndef __TRADINGCORE_SLIPPAGE_H__
#define __TRADINGCORE_SLIPPAGE_H__

#include <assert.h>
#include <vector>
#include "candle.h"
#include "utils.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename PercentType,
          typename PercentProcT>
class Slippage {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;

 public:
  Slippage() {}
  virtual ~Slippage() {}

 public:
  virtual PercentType countCandleSlippage(CandleDataT& candle,
                                          VolumeType vol) = 0;

 protected:
};

template <typename MoneyType, typename VolumeType, typename PercentType,
          typename PercentProcT>
class Slippage_Simple
    : Slippage<MoneyType, VolumeType, PercentType, PercentProcT> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;

 public:
  Slippage_Simple() : m_defaultVal(0) {}
  virtual ~Slippage_Simple() {}

 public:
  void setDefault(PercentType val) { m_defaultVal = val; }

 public:
  virtual PercentType countCandleSlippage(CandleDataT& candle, VolumeType vol) {
    return m_defaultVal;
  }

 protected:
  PercentType m_defaultVal;
};

template <typename MoneyType, typename VolumeType, typename PercentType,
          typename PercentProcT>
Slippage<MoneyType, VolumeType, PercentType, PercentProcT>* newSimpleSlippage(
    PercentType val) {
  typedef Slippage<MoneyType, VolumeType, PercentType, PercentProcT> SlippageT;
  typedef Slippage_Simple<MoneyType, VolumeType, PercentType, PercentProcT>
      Slippage_SimpleT;

  Slippage_SimpleT* pSlippage = new Slippage_SimpleT();
  pSlippage->setDefault(val);

  return pSlippage;
}

}  // namespace trading

#endif  // __TRADINGCORE_SLIPPAGE_H__