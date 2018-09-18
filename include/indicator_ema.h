#ifndef __TRADINGCORE_INDICATOR_EMA_H__
#define __TRADINGCORE_INDICATOR_EMA_H__

#include "indicator.h"

enum INDICATOR_INDEX_EMA { EMA_PRICE = 0, EMA_EMA = 1, EMA_VALUE_NUMS = 2 };

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType>
class Indicator_EMA
    : public Indicator<TimeType, PriceType, VolumeType, ValueType> {
 public:
  typedef CandleData<TimeType, PriceType, VolumeType> CandleDataT;
  typedef CandleList<TimeType, PriceType, VolumeType> CandleListT;
  typedef Indicator<TimeType, PriceType, VolumeType, ValueType> IndicatorT;
  typedef IndicatorDataMgr<ValueType, EMA_VALUE_NUMS> IndicatorDataMgrT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Indicator_EMA(int avgtimes, CandleListT& lstCandle,
                IndicatorDataMgrT& mgrIndicatorData)
      : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
    if (m_avgTimes <= 0) {
      m_avgTimes = 1;
    }
  }
  virtual ~Indicator_EMA() {}

 public:
  virtual bool build() {
    this->clear();

    if (m_avgTimes == 1) {
      for (int i = 0; i < this->m_lstCandle.getLength(); ++i) {
        CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(EMA_PRICE, cd.close);
        pDat->set(EMA_EMA, cd.close);
      }
    } else if (this->m_lstCandle.getLength() < m_avgTimes) {
      for (int i = 0; i < this->m_lstCandle.getLength(); ++i) {
        CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(EMA_PRICE, cd.close);
        pDat->set(EMA_EMA, -1);
      }
    } else {
      BaseIndicatorDataT* pPre = NULL;
      for (int i = 0; i < m_avgTimes; ++i) {
        CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(EMA_PRICE, cd.close);
        pDat->set(EMA_EMA, -1);
        pPre = pDat;
      }

      for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
        CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(EMA_PRICE, cd.close);
        if (pPre->get(EMA_EMA) == -1) {
          ValueType tp = 0;
          for (int j = 0; j < m_avgTimes; ++j) {
            BaseIndicatorDataT* pCurDat = this->getData(i - j - 1);
            tp += pCurDat->get(EMA_EMA);
          }

          pDat->set(EMA_EMA, tp / m_avgTimes);
        } else {
          pDat->set(EMA_EMA, pPre->get(EMA_EMA) +
                                 (pDat->get(EMA_PRICE) - pDat->get(EMA_PRICE)) *
                                     2 / (m_avgTimes + 1));
        }

        pPre = pDat;
      }
    }
  }

 protected:
  int m_avgTimes;
};

#endif  // __TRADINGCORE_INDICATOR_EMA_H__