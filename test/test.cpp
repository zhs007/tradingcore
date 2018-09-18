#include <stdio.h>
#include "../include/candle.h"
#include "../include/indicator.h"

void testCandle() {
    CandleList<int64_t, int64_t, int64_t> lst;
}

void testIndicator() {
    IndicatorDataMgr<int64_t, 2>* pMgr = getIndicatorDataMgr<int64_t, 2>();
}

int main() {
  testCandle();

  return 0;
}