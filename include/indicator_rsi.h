#ifndef __TRADINGCORE_INDICATOR_RSI_H__
#define __TRADINGCORE_INDICATOR_RSI_H__

#include "indicator.h"

//!! https://en.wikipedia.org/wiki/Relative_strength_index

namespace trading {

enum INDICATOR_INDEX_RSI {
  RSI_PRICE = 0,
  RSI_CUR_U,
  RSI_CUR_D,
  RSI_SMMA_U,
  RSI_SMMA_D,
  RSI_RSI,
  RSI_VALUE_NUMS
};

template <typename MoneyType, typename VolumeType, typename ValueType>
class Indicator_RSI : public Indicator<MoneyType, VolumeType, ValueType> {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef IndicatorDataMgr<ValueType, RSI_VALUE_NUMS> IndicatorDataMgrT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Indicator_RSI(int avgtimes, CandleListT& lstCandle,
                IndicatorDataMgrT& mgrIndicatorData)
      : IndicatorT(lstCandle, mgrIndicatorData), m_avgTimes(avgtimes) {
    if (m_avgTimes <= 0) {
      m_avgTimes = 1;
    }
  }
  virtual ~Indicator_RSI() {}

 public:
  virtual bool build() {
    assert(m_avgTimes > 0);
    assert(this->m_lstCandle.getLength() < m_avgTimes);
    this->clear();

    BaseIndicatorDataT* pPre = NULL;
    for (int i = 0; i < m_avgTimes; ++i) {
      const CandleDataT& cd = this->m_lstCandle.get(i);
      BaseIndicatorDataT* pDat = this->pushNewData();
      pDat->set(RSI_PRICE, cd.close);
      pDat->set(RSI_SMMA_U, -1);
      pDat->set(RSI_SMMA_D, -1);
      pDat->set(RSI_RSI, -1);

      if (pPre != NULL) {
        if (pDat->get(RSI_PRICE) > pPre->get(RSI_PRICE)) {
          pDat->set(RSI_CUR_U, pDat->get(RSI_PRICE) - pPre->get(RSI_PRICE));
          pDat->set(RSI_CUR_D, 0);
        } else {
          pDat->set(RSI_CUR_D, pPre->get(RSI_PRICE) - pDat->get(RSI_PRICE));
          pDat->set(RSI_CUR_U, 0);
        }
      } else {
        pDat->set(RSI_CUR_U, -1);
        pDat->set(RSI_CUR_D, -1);
      }

      pPre = pDat;
    }

    if (m_avgTimes == 1) {
      for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
        const CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(RSI_PRICE, cd.close);

        if (pDat->get(RSI_PRICE) > pPre->get(RSI_PRICE)) {
          pDat->set(RSI_CUR_U, pDat->get(RSI_PRICE) - pPre->get(RSI_PRICE));
          pDat->set(RSI_CUR_D, 0);
        } else {
          pDat->set(RSI_CUR_D, pPre->get(RSI_PRICE) - pDat->get(RSI_PRICE));
          pDat->set(RSI_CUR_U, 0);
        }

        pDat->set(RSI_SMMA_U, pDat->get(RSI_CUR_U));
        pDat->set(RSI_SMMA_D, pDat->get(RSI_CUR_D));

        if (pDat->get(RSI_SMMA_D) == 0) {
          pDat->set(RSI_RSI, 100);
        } else {
          pDat->set(RSI_RSI, 100 - 100 / (1 + pDat->get(RSI_SMMA_U) /
                                                  pDat->get(RSI_SMMA_D)));
        }
      }
    } else {
      for (int i = m_avgTimes; i < this->m_lstCandle.getLength(); ++i) {
        const CandleDataT& cd = this->m_lstCandle.get(i);
        BaseIndicatorDataT* pDat = this->pushNewData();
        pDat->set(RSI_PRICE, cd.close);

        if (pPre->get(RSI_RSI) == -1) {
          ValueType tu = 0;
          ValueType td = 0;
          for (int j = 0; j < m_avgTimes; ++j) {
            BaseIndicatorDataT* pCurDat = this->getData(i - j - 1);
            tu += pCurDat->get(RSI_CUR_U);
            td += pCurDat->get(RSI_CUR_D);
          }

          pDat->set(RSI_SMMA_U, tu / m_avgTimes);
          pDat->set(RSI_SMMA_D, td / m_avgTimes);

          if (pDat->get(RSI_SMMA_D) == 0) {
            pDat->set(RSI_RSI, 100);
          } else {
            pDat->set(RSI_RSI, 100 - 100 / (1 + pDat->get(RSI_SMMA_U) /
                                                    pDat->get(RSI_SMMA_D)));
          }
        } else {
          pDat->set(RSI_SMMA_U, ((m_avgTimes - 1) * pPre->get(RSI_SMMA_U) +
                                 pDat->get(RSI_CUR_U)) /
                                    m_avgTimes);
          pDat->set(RSI_SMMA_D, ((m_avgTimes - 1) * pPre->get(RSI_SMMA_D) +
                                 pDat->get(RSI_CUR_D)) /
                                    m_avgTimes);

          if (pDat->get(RSI_SMMA_D) == 0) {
            pDat->set(RSI_RSI, 100);
          } else {
            pDat->set(RSI_RSI, 100 - 100 / (1 + pDat->get(RSI_SMMA_U) /
                                                    pDat->get(RSI_SMMA_D)));
          }
        }

        pPre = pDat;
      }
    }

    return true;
  }

 protected:
  int m_avgTimes;
};

template <typename MoneyType, typename VolumeType, typename ValueType>
Indicator<MoneyType, VolumeType, ValueType>* newIndicator_RSI(
    CandleList<MoneyType, VolumeType>& lstCandle) {
  typedef Indicator_RSI<MoneyType, VolumeType, ValueType> Indicator_RSIT;

  return new Indicator_RSIT(
      1, lstCandle, *(getIndicatorDataMgr<ValueType, RSI_VALUE_NUMS>()));
}

}  // namespace trading

#endif  // __TRADINGCORE_INDICATOR_RSI_H__