#include "../include/tradingcore.h"

#include "../include/indicator_ema.h"
#include "../include/indicator_rsi.h"
#include "../include/indicator_sma.h"
#include "../include/indicator_smma.h"
#include "../include/indicator_wma.h"

namespace trading {

ContextInt64* initInt64() {
  ContextInt64* pCtx = new ContextInt64();

  pCtx->mgrIndicator.regIndicator("ema", newIndicator_EMA<int64_t, int64_t, int64_t>);
  pCtx->mgrIndicator.regIndicator("rsi", newIndicator_RSI<int64_t, int64_t, int64_t>);
  pCtx->mgrIndicator.regIndicator("sma", newIndicator_SMA<int64_t, int64_t, int64_t>);
  pCtx->mgrIndicator.regIndicator("smma", newIndicator_SMMA<int64_t, int64_t, int64_t>);
  pCtx->mgrIndicator.regIndicator("wma", newIndicator_WMA<int64_t, int64_t, int64_t>);  

  return pCtx;
}

void releaseInt64(ContextInt64* pCtx) {
  delete pCtx;
}

}  // namespace trading
