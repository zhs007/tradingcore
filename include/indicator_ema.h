#ifndef __TRADINGCORE_INDICATOR_EMA_H__
#define __TRADINGCORE_INDICATOR_EMA_H__

#include "indicator.h"

enum INDICATOR_INDEX_EMA { PRICE = 0, EMA = 1, VALUE_NUMS = 2 };

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType>
class Indicator_EMA
    : public Indicator<TimeType, PriceType, VolumeType, ValueType> {
        public:
        typedef CandleData<TimeType, PriceType, VolumeType> CandleDataT;
        typedef CandleList<TimeType, PriceType, VolumeType> CandleListT;
        typedef Indicator<TimeType, PriceType, VolumeType, ValueType> IndicatorT;
        typedef IndicatorDataMgr<ValueType, VALUE_NUMS> IndicatorDataMgrT;
 public:
  Indicator_EMA(int avgtimes, CandleListT& lstCandle, IndicatorDataMgrT& mgrIndicatorData) : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
      if (m_avgTimes <= 0) {
          m_avgTimes = 1;
      }
  }
  virtual ~Indicator_EMA() {}

 public:
  virtual bool build() {
    for (int i = 0; i < this->m_lstCandle.getLength(); ++i) {
        CandleDataT& cd = this->m_lstCandle.get(i);
    }
  }

 protected:
 int m_avgTimes;
};

#endif // __TRADINGCORE_INDICATOR_EMA_H__