#ifndef __TRADINGCORE_INDICATOR_SMMA_H__
#define __TRADINGCORE_INDICATOR_SMMA_H__

#include "indicator.h"

//!! https://en.wikipedia.org/wiki/Moving_average#Modified_moving_average

namespace trading {

enum INDICATOR_INDEX_SMMA { SMMA_PRICE = 0, SMMA_SMMA, SMMA_VALUE_NUMS };

template <typename MoneyType, typename VolumeType, typename ValueType>
class Indicator_SMMA : public Indicator<MoneyType, VolumeType, ValueType> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef IndicatorDataMgr<ValueType, SMMA_VALUE_NUMS> IndicatorDataMgrT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Indicator_SMMA(int avgtimes, CandleListT& lstCandle,
                 IndicatorDataMgrT& mgrIndicatorData)
      : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
    if (m_avgTimes <= 0) {
      m_avgTimes = 1;
    }
  }
  virtual ~Indicator_SMMA() {}

 public:
  virtual bool build() {
    assert(m_avgTimes > 1);
    assert(this->m_lstCandle.getLength() < m_avgTimes);
    this->clear();

    BaseIndicatorDataT* pPre = NULL;
    for (int i = 0; i < m_avgTimes; ++i) {
      CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(SMMA_PRICE, cd.close);
      pDat->set(SMMA_SMMA, -1);
      pPre = pDat;
    }

    for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
      CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(SMMA_PRICE, cd.close);
      if (pPre->get(SMMA_SMMA) == -1) {
        ValueType tp = 0;
        for (int j = 0; j < m_avgTimes; ++j) {
          BaseIndicatorDataT* pCurDat = this->getData(i - j - 1);
          tp += pCurDat->get(SMMA_PRICE);
        }

        pDat->set(SMMA_SMMA, tp / m_avgTimes);
      } else {
        BaseIndicatorDataT* pCurDat = this->getData(i - m_avgTimes - 1);
        pDat->set(SMMA_SMMA, ((m_avgTimes - 1) * pPre->get(SMMA_SMMA) +
                              pDat->get(SMMA_PRICE)) /
                                 m_avgTimes);
      }

      pPre = pDat;
    }
  }

 protected:
  int m_avgTimes;
};

template <typename MoneyType, typename VolumeType, typename ValueType>
Indicator<MoneyType, VolumeType, ValueType>* newIndicator_SMMA(
    CandleList<MoneyType, VolumeType>& lstCandle) {
  typedef Indicator_SMMA<MoneyType, VolumeType, ValueType> Indicator_SMMAT;

  return new Indicator_SMMAT(
      1, lstCandle, *(getIndicatorDataMgr<ValueType, SMMA_VALUE_NUMS>()));
}

}  // namespace trading

#endif  // __TRADINGCORE_INDICATOR_SMMA_H__