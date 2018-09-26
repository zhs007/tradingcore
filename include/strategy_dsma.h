#ifndef __TRADINGCORE_STRATEGY_DSMA_H__
#define __TRADINGCORE_STRATEGY_DSMA_H__

#include <string>
#include "candle.h"
#include "exchange.h"
#include "indicator_sma.h"
#include "pnl.h"
#include "strategy.h"
#include "utils.h"
#include "wallet.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
class Strategy_DSMA
    : public Strategy<MoneyType, VolumeType, ValueType, PercentType> {
 public:
  typedef Strategy<MoneyType, VolumeType, ValueType, PercentType> StrategyT;
  typedef PNL<PercentType> PNLT;
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef ExchangeCategory<MoneyType, VolumeType, ValueType> ExchangeCategoryT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  Strategy_DSMA()
      : StrategyT(), m_curindex(0), m_fast(NULL), m_slow(NULL), m_preTime(0) {}
  virtual ~Strategy_DSMA() {}

 public:
  virtual void onTick() {}

  virtual void onCandle(time_t ct) {
    ExchangeCategoryT* pEC = this->getExchangeCategory(m_mainCategory.c_str());
    if (pEC != NULL) {
      const CandleDataT* cd = pEC->findTimeEx(m_curindex, ct);

      if (m_fast == NULL) {
        m_fast = pEC->getIndicator(m_nameFast.c_str());
      }

      if (m_slow == NULL) {
        m_slow = pEC->getIndicator(m_nameSlow.c_str());
      }

      if (m_curindex > 0 && ct - m_preTime > 0 && ct - m_preTime <= 30 * 60) {
        const BaseIndicatorDataT* pSlow0 = m_slow->getData(m_curindex - 1);
        const BaseIndicatorDataT* pSlow1 = m_slow->getData(m_curindex);
        const BaseIndicatorDataT* pFast0 = m_fast->getData(m_curindex - 1);
        const BaseIndicatorDataT* pFast1 = m_fast->getData(m_curindex);

        ValueType s0, s1, f0, f1;
        s0 = pSlow0->get(SMA_SMA);
        s1 = pSlow1->get(SMA_SMA);
        f0 = pFast0->get(SMA_SMA);
        f1 = pFast1->get(SMA_SMA);

        if (s0 >= 0 && s1 >= 0 && f0 >= 0 && f1 >= 0) {
          if (s0 < f0 && s1 >= f1) {
            printf("do up ");
          } else if (s0 > f0 && s1 <= f1) {
            printf("do down");
          }
        }
      }

      m_preTime = ct;
    }
  }

 public:
  void setMainCategory(const char* name) { m_mainCategory = name; }

  void setFastIndicator(const char* name) { m_nameFast = name; }

  void setSlowIndicator(const char* name) { m_nameSlow = name; }

  void setSafeTimeOff(time_t ot) { m_safeTimeOff = ot; }

 protected:
  std::string m_mainCategory;
  std::string m_nameFast;
  std::string m_nameSlow;

  IndicatorT* m_fast;
  IndicatorT* m_slow;

  int m_curindex;

  time_t m_preTime;
  time_t m_safeTimeOff;
};

}  // namespace trading

#endif  // __TRADINGCORE_STRATEGY_DSMA_H__