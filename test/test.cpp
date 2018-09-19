#include <stdio.h>
#include "../include/tradingcore.h"

void testCandle() {
  // CandleList<int64_t, int64_t, int64_t> lst;
}

void testIndicator() {
  // IndicatorDataMgr<int64_t, 2>* pMgr = getIndicatorDataMgr<int64_t, 2>();
}

void testLoadCSV(const char* filename) {
    trading::CandleListInt64 lstCandle;

    trading::CSVConfig cfg;
    cfg.head.curtime = "__0";
    cfg.head.open = "open";
    cfg.head.close = "close";
    cfg.head.high = "high";
    cfg.head.low = "low";
    cfg.head.volume = "volume";
    cfg.head.openInterest = "oi";

    cfg.scalePrice = 100;
    cfg.scaleVolume = 100;

    bool loadok = trading::loadCSVInt64(lstCandle, filename, cfg);
    if (!loadok) {
        printf("testLoadCSV %s fail.", filename);

        return ;
    }

    printf("%s has %d rows", filename, lstCandle.getLength());
}

int main() {
  testLoadCSV("samplecsv/TA601.csv");

  return 0;
}