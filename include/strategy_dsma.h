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
  Strategy_DSMA() : StrategyT(), m_fast(NULL), m_slow(NULL), m_preTime(0) {}
  virtual ~Strategy_DSMA() {}

 public:
  virtual void onTick() {}

  virtual void onCandle(int candleIndex) {
    ExchangeCategoryT* pEC = this->getExchangeCategory(m_mainCategory.c_str());
    if (pEC != NULL) {
      const CandleDataT* cd = pEC->getCandleData(candleIndex);
      if (cd != NULL) {
        time_t ct = cd->curtime;

        if (m_fast == NULL) {
          m_fast = pEC->getIndicator(m_nameFast.c_str());
        }

        if (m_slow == NULL) {
          m_slow = pEC->getIndicator(m_nameSlow.c_str());
        }

        if (candleIndex > 0 && ct - m_preTime > 0 &&
            ct - m_preTime <= 30 * 60) {
          const BaseIndicatorDataT* pSlow0 = m_slow->getData(candleIndex - 1);
          const BaseIndicatorDataT* pSlow1 = m_slow->getData(candleIndex);
          const BaseIndicatorDataT* pFast0 = m_fast->getData(candleIndex - 1);
          const BaseIndicatorDataT* pFast1 = m_fast->getData(candleIndex);

          ValueType s0, s1, f0, f1;
          s0 = pSlow0->get(SMA_SMA);
          s1 = pSlow1->get(SMA_SMA);
          f0 = pFast0->get(SMA_SMA);
          f1 = pFast1->get(SMA_SMA);

          if (s0 >= 0 && s1 >= 0 && f0 >= 0 && f1 >= 0) {
            if (s0 < f0 && s1 >= f1) {
              char buf[128];
              time2str(buf, 128, cd->curtime);

              pEC->clearOrder(ORDER_SELL);
              pEC->newLimitOrder(ORDER_BUY, cd->close * 1.1, 100, ct);
              printf("%s do up \n", buf);
            } else if (s0 > f0 && s1 <= f1) {
              char buf[128];
              time2str(buf, 128, cd->curtime);

              pEC->clearOrder(ORDER_BUY);
              pEC->newLimitOrder(ORDER_SELL, cd->close * 0.9, 100, ct);
              printf("%s do down \n", buf);
            }
          }
        }

        m_preTime = ct;
      }
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

  // int m_curindex;

  time_t m_preTime;
  time_t m_safeTimeOff;
};

}  // namespace trading

#endif  // __TRADINGCORE_STRATEGY_DSMA_H__