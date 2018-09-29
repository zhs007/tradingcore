#include <stdio.h>
// #include "../include/strategy_dsma.h"
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

void testTrader(trading::ContextInt64* pCtx) {
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
  trading::TraderInt64 trader(pCtx->mgrCategory);
  trading::SimExchangeInt64 exchange(pCtx->mgrCategory, "zss", orderlogic);
  // trading::SimExchangeCategoryInt64 pta1601("pta1601",
  // exchange.getCategoryConfigWithName("pta"), orderlogic);

  // exchange.addCategory("pta1601", "pta");
  exchange.newCategoryConfig("pta1601", 1, 1);
  exchange.addSimExchangeCategory(pCtx->mgrOrder, pCtx->mgrTrade, "pta1601",
                                  "samplecsv/TA601.csv", cfg);

  trading::IndicatorParam param;
  param.avgTime = 10;

  exchange.addIndicator(pCtx->mgrIndicatorData, pCtx->mgrIndicator, "sma",
                        "sma10", param);

  param.avgTime = 20;
  exchange.addIndicator(pCtx->mgrIndicatorData, pCtx->mgrIndicator, "sma",
                        "sma20", param);

  trader.addExchange(exchange);

  trader.initCategoryInfo("pta1601", 0, 0);

  trading::StrategyInt64_DSMA* pDSMA = new trading::StrategyInt64_DSMA();
  pDSMA->addExchangeCategory("pta1601");
  pDSMA->setMainCategory("pta1601");
  pDSMA->setFastIndicator("sma10");
  pDSMA->setSlowIndicator("sma20");
  pDSMA->setSafeTimeOff(30 * 60);

  trader.addStrategy(pDSMA);

  trader.startSimTrade();
}

int main() {
  trading::ContextInt64* pCtx = trading::initInt64();

  // testLoadCSV("samplecsv/noformat.csv");
  testTrader(pCtx);

  return 0;
}