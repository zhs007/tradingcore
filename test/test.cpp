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
  cfg.timeType = trading::CSVTIME_FORMAT_STR;

  bool loadok = trading::loadCSVInt64(lstCandle, filename, cfg);
  if (!loadok) {
    printf("testLoadCSV %s fail.", filename);

    return;
  }

  printf("%s has %d rows", filename, lstCandle.getLength());

  lstCandle.format();

  printf("%s has %d rows", filename, lstCandle.getLength());

  trading::saveCSVInt64(lstCandle, "output.csv", cfg);
}

void testTrader() {
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
  cfg.timeType = trading::CSVTIME_FORMAT_STR;

  trading::OrderLogic_Simple2Int64 orderlogic;
  trading::TraderInt64 trader;
  trading::SimExchangeInt64 exchange("zss", orderlogic);
  // trading::SimExchangeCategoryInt64 pta1601("pta1601",
  // exchange.getCategoryConfigWithName("pta"), orderlogic);

  exchange.addCategory("pta1601", "pta");
  exchange.newCategoryConfig("pta", 1, 1);
  exchange.addSimExchangeCategory("pta", "pta1601", "samplecsv/TA601.csv", cfg);

  trader.addExchange(exchange);

  trader.startSimTrade(trading::str2time("2015-04-21 21:01:00"),
                       trading::str2time("2016-01-15 15:00:00"), 60);
}

int main() {
  testLoadCSV("samplecsv/noformat.csv");
  testTrader();

  return 0;
}