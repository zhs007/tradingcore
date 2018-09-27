#ifndef __TRADINGCORE_INDICATOR_SMA_H__
#define __TRADINGCORE_INDICATOR_SMA_H__

#include "indicator.h"

//!! https://en.wikipedia.org/wiki/Moving_average

namespace trading {

enum INDICATOR_INDEX_SMA { SMA_PRICE = 0, SMA_SMA, SMA_VALUE_NUMS };

template <typename MoneyType, typename VolumeType, typename ValueType>
class Indicator_SMA : public Indicator<MoneyType, VolumeType, ValueType> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef IndicatorDataMgr<ValueType, SMA_VALUE_NUMS> IndicatorDataMgrT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Indicator_SMA(int avgtimes, CandleListT& lstCandle,
                IndicatorDataMgrT& mgrIndicatorData)
      : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
    if (m_avgTimes <= 0) {
      m_avgTimes = 1;
    }
  }
  virtual ~Indicator_SMA() {}

 public:
  virtual bool build() {
    assert(m_avgTimes > 1);
    assert(this->m_lstCandle.getLength() > m_avgTimes);

    this->clear();

    BaseIndicatorDataT* pPre = NULL;
    for (int i = 0; i < m_avgTimes; ++i) {
      const CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(SMA_PRICE, cd.close);
      pDat->set(SMA_SMA, -1);
      pPre = pDat;
    }

    for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
      const CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(SMA_PRICE, cd.close);
      if (pPre->get(SMA_SMA) == -1) {
        ValueType tp = 0;
        for (int j = 0; j < m_avgTimes; ++j) {
          const BaseIndicatorDataT* pCurDat = this->getData(i - j - 1);
          tp += pCurDat->get(SMA_PRICE);
        }

        pDat->set(SMA_SMA, tp / m_avgTimes);
      } else {
        const BaseIndicatorDataT* pCurDat = this->getData(i - m_avgTimes - 1);
        pDat->set(SMA_SMA, pPre->get(SMA_SMA) +
                               pDat->get(SMA_PRICE) / m_avgTimes -
                               pCurDat->get(SMA_PRICE) / m_avgTimes);
      }

      pPre = pDat;
    }

    return true;
  }

 protected:
  int m_avgTimes;
};

template <typename MoneyType, typename VolumeType, typename ValueType>
Indicator<MoneyType, VolumeType, ValueType>* newIndicator_SMA(
    IndicatorParam& param, CandleList<MoneyType, VolumeType>& lstCandle) {
  typedef Indicator_SMA<MoneyType, VolumeType, ValueType> Indicator_SMAT;

  return new Indicator_SMAT(
      param.avgTime, lstCandle,
      *(getIndicatorDataMgr<ValueType, SMA_VALUE_NUMS>()));
}

}  // namespace trading

#endif  // __TRADINGCORE_INDICATOR_SMA_H__