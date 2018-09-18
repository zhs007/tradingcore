#ifndef __TRADINGCORE_INDICATOR_WMA_H__
#define __TRADINGCORE_INDICATOR_WMA_H__

#include "indicator.h"

enum INDICATOR_INDEX_WMA {
  WMA_PRICE = 0,
  WMA_TOTAL,
  WMA_NUMERATOR,
  WMA_WMA,
  WMA_VALUE_NUMS
};

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType>
class Indicator_WMA
    : public Indicator<TimeType, PriceType, VolumeType, ValueType> {
 public:
  typedef CandleData<TimeType, PriceType, VolumeType> CandleDataT;
  typedef CandleList<TimeType, PriceType, VolumeType> CandleListT;
  typedef Indicator<TimeType, PriceType, VolumeType, ValueType> IndicatorT;
  typedef IndicatorDataMgr<ValueType, WMA_VALUE_NUMS> IndicatorDataMgrT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Indicator_WMA(int avgtimes, CandleListT& lstCandle,
                IndicatorDataMgrT& mgrIndicatorData)
      : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
    if (m_avgTimes <= 0) {
      m_avgTimes = 1;
    }

    m_denominator = m_avgTimes * (m_avgTimes + 1) / 2;
  }
  virtual ~Indicator_WMA() {}

 public:
  virtual bool build() {
    assert(m_avgTimes > 1);
    assert(this->m_lstCandle.getLength() < m_avgTimes);
    this->clear();

    BaseIndicatorDataT* pPre = NULL;
    for (int i = 0; i < m_avgTimes; ++i) {
      CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(WMA_PRICE, cd.close);
      pDat->set(WMA_TOTAL, -1);
      pDat->set(WMA_NUMERATOR, -1);
      pDat->set(WMA_WMA, -1);

      pPre = pDat;
    }

    for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
      CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(WMA_PRICE, cd.close);
      if (pPre->get(WMA_WMA) == -1) {
        ValueType tp = 0;
        ValueType tn = 0;
        for (int j = 0; j < m_avgTimes; ++j) {
          BaseIndicatorDataT* pCurDat = this->getData(i - j - 1);
          tp += pCurDat->get(WMA_PRICE);
          tn += pCurDat->get(WMA_PRICE) * (m_avgTimes - j);
        }

        pDat->set(WMA_TOTAL, tp);
        pDat->set(WMA_NUMERATOR, tn);

        pDat->set(WMA_WMA,
                  pDat->get(WMA_NUMERATOR) / m_denominator);
      } else {
              curcache[INDICATORCACHEINDEX_WMA.TOTAL] = lastcache[INDICATORCACHEINDEX_WMA.TOTAL] + curcache[INDICATORCACHEINDEX_WMA.PRICE] - lstcache[lstcache.length - 1 - avgtimes][INDICATORCACHEINDEX_WMA.PRICE];
    curcache[INDICATORCACHEINDEX_WMA.NUMERATOR] = lastcache[INDICATORCACHEINDEX_WMA.NUMERATOR] + avgtimes * curcache[INDICATORCACHEINDEX_WMA.PRICE] - lstcache[INDICATORCACHEINDEX_WMA.TOTAL];
    curcache[INDICATORCACHEINDEX_WMA.DENOMINATOR] = lastcache[INDICATORCACHEINDEX_WMA.DENOMINATOR];

        BaseIndicatorDataT* pCurDat = this->getData(i - m_avgTimes - 1);

        pDat->set(WMA_TOTAL, pPre->get(WMA_TOTAL) + pDat->get(WMA_PRICE) - pCurDat->get(WMA_PRICE));
        pDat->set(WMA_NUMERATOR, pPre->get(WMA_NUMERATOR) + pDat->get(WMA_PRICE) - pCurDat->get(WMA_PRICE));
      }

      pPre = pDat;
    }
  }

 protected:
  int m_avgTimes;
  ValueType m_denominator;
};

#endif  // __TRADINGCORE_INDICATOR_WMA_H__