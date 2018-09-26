#include "../include/tradingcore.h"

#include "../include/indicator_ema.h"
#include "../include/indicator_rsi.h"
#include "../include/indicator_sma.h"
#include "../include/indicator_smma.h"
#include "../include/indicator_wma.h"

namespace trading {

void initInt64() {
  auto pMgr = getIndicatorMgr<int64_t, int64_t, int64_t>();

  pMgr->regIndicator("ema", newIndicator_EMA<int64_t, int64_t, int64_t>);
  pMgr->regIndicator("rsi", newIndicator_RSI<int64_t, int64_t, int64_t>);
  pMgr->regIndicator("sma", newIndicator_SMA<int64_t, int64_t, int64_t>);
  pMgr->regIndicator("smma", newIndicator_SMMA<int64_t, int64_t, int64_t>);
  pMgr->regIndicator("wma", newIndicator_WMA<int64_t, int64_t, int64_t>);
}

}  // namespace trading
