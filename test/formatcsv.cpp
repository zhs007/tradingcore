#include <stdio.h>
// #include "../include/strategy_dsma.h"
#include "../include/tradingcore.h"

const char* DEFAULT_TIMEZONE = "Asia/Shanghai";

// - if ret == cur->curtime, add | overwrite | fill (pre->curtime, cur->curtime]
// - if ret == 0, discard
// - if pre == NULL && ret < cur->curtime, fill [ret, cur->curtime]
// - if ret >= pre->curtime && ret < cur->curtime, fill (pre->curtime, ret] & set
//   pre = NULL recheck
time_t isNeedFormatCandleData(trading::CandleDataInt64* pre,
                              trading::CandleDataInt64* cur) {
  assert(cur != NULL);

  tm curtm;
  trading::time2tm(&curtm, cur->curtime, DEFAULT_TIMEZONE);
  curtm.tm_sec = 0;

  cur->curtime = trading::tm2time(&curtm, DEFAULT_TIMEZONE);

  // tm pretm;
  // if (pre != NULL) {
  //   trading::time2tm(&pretm, pre->curtime, DEFAULT_TIMEZONE);

  //   if ((pretm.tm_hour == 23 && pretm.tm_min == 30) ||
  //       (pretm.tm_hour <= 11 && pretm.tm_min == 30) ||
  //       (pretm.tm_hour == 15 && pretm.tm_min == 0)) {
  //     pre = NULL;
  //   }
  // }

  if (pre == NULL) {
    // if curtm is start
    if ((curtm.tm_hour == 21 && curtm.tm_min == 0) ||
        (curtm.tm_hour == 9 && curtm.tm_min == 0) ||
        (curtm.tm_hour == 13 && curtm.tm_min == 30)) {
      return cur->curtime;
    }

    // if curtm in (21:00, 23:30]
    if ((curtm.tm_hour == 21 && curtm.tm_min > 0) ||
        (curtm.tm_hour > 21 && curtm.tm_hour < 23) ||
        (curtm.tm_hour == 23 && curtm.tm_min <= 30)) {
      curtm.tm_hour = 21;
      curtm.tm_min = 0;

      return trading::tm2time(&curtm, DEFAULT_TIMEZONE);
    }

    // if curtm in (9:00, 11:30]
    if ((curtm.tm_hour == 9 && curtm.tm_min > 0) ||
        (curtm.tm_hour > 9 && curtm.tm_hour < 11) ||
        (curtm.tm_hour <= 11 && curtm.tm_min <= 30)) {
      curtm.tm_hour = 9;
      curtm.tm_min = 0;

      return trading::tm2time(&curtm, DEFAULT_TIMEZONE);
    }

    // if curtm in (13:30, 15:00]
    if ((curtm.tm_hour == 13 && curtm.tm_min > 30) ||
        (curtm.tm_hour > 13 && curtm.tm_hour < 15) ||
        (curtm.tm_hour == 15 && curtm.tm_min == 0)) {
      curtm.tm_hour = 13;
      curtm.tm_min = 30;

      return trading::tm2time(&curtm, DEFAULT_TIMEZONE);
    }

    return 0;
  }

  if (cur->curtime == pre->curtime) {
    return cur->curtime;
  }

  tm pretm;
  trading::time2tm(&pretm, pre->curtime, DEFAULT_TIMEZONE);

  // if pretm in [21:00, 23:30]
  if ((pretm.tm_hour == 21 && pretm.tm_min >= 0) ||
      (pretm.tm_hour > 21 && pretm.tm_hour < 23) ||
      (pretm.tm_hour == 23 && pretm.tm_min <= 30)) {
    // if curtm in [21:00, 23:30]
    if ((curtm.tm_hour == 21 && curtm.tm_min >= 0) ||
        (curtm.tm_hour > 21 && curtm.tm_hour < 23) ||
        (curtm.tm_hour == 23 && curtm.tm_min <= 30)) {
      return cur->curtime;
    }

    pretm.tm_hour = 23;
    pretm.tm_min = 30;

    return trading::tm2time(&pretm, DEFAULT_TIMEZONE);
  }

  if ((pretm.tm_hour == 9 && pretm.tm_min >= 0) ||
      (pretm.tm_hour > 9 && pretm.tm_hour < 11) ||
      (pretm.tm_hour <= 11 && pretm.tm_min <= 30)) {
    if ((curtm.tm_hour == 9 && curtm.tm_min > 0) ||
        (curtm.tm_hour > 9 && curtm.tm_hour < 11) ||
        (curtm.tm_hour <= 11 && curtm.tm_min <= 30)) {
      return cur->curtime;
    }

    pretm.tm_hour = 11;
    pretm.tm_min = 30;

    return trading::tm2time(&pretm, DEFAULT_TIMEZONE);
  }

  if ((pretm.tm_hour == 13 && pretm.tm_min >= 30) ||
      (pretm.tm_hour > 13 && pretm.tm_hour < 15) ||
      (pretm.tm_hour == 15 && pretm.tm_min == 0)) {
    if ((curtm.tm_hour == 13 && curtm.tm_min >= 30) ||
        (curtm.tm_hour > 13 && curtm.tm_hour < 15) ||
        (curtm.tm_hour == 15 && curtm.tm_min == 0)) {
      return cur->curtime;
    }

    pretm.tm_hour = 15;
    pretm.tm_min = 0;

    return trading::tm2time(&pretm, DEFAULT_TIMEZONE);
  }

  return 0;
}

void formatLoadCSV(const char* fnsrc, const char* fddest) {
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

  bool loadok = trading::loadCSVInt64(lstCandle, fnsrc, DEFAULT_TIMEZONE, cfg);
  if (!loadok) {
    printf("testLoadCSV %s fail.", fnsrc);

    return;
  }

  printf("%s has %d rows", fnsrc, lstCandle.getLength());

  lstCandle.formatEx(isNeedFormatCandleData);

  printf("%s has %d rows", fnsrc, lstCandle.getLength());

  trading::saveCSVInt64(lstCandle, fddest, DEFAULT_TIMEZONE, cfg);
}

int main() {
  trading::setTimeZone(DEFAULT_TIMEZONE);

  trading::ContextInt64* pCtx = trading::initInt64();

  formatLoadCSV("samplecsv/TA601.csv", "samplecsv/TA601F.csv");

  return 0;
}