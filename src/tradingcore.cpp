#include "../include/tradingcore.h"
#include <time.h>

namespace trading {

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  CSVConfig& cfg) {
  lstCandle.clear();
  CSVFile csv;
  bool loadok = csv.load(filename);
  if (!loadok) {
    return false;
  }

  for (int i = 0; i < csv.getLength(); ++i) {
    int64_t curtime;
    int64_t open, close, high, low;
    int64_t volume;
    int64_t openInterest;

    const char* strTime = csv.get(i, cfg.head.curtime.c_str());
    const char* strOpen = csv.get(i, cfg.head.open.c_str());
    const char* strClose = csv.get(i, cfg.head.close.c_str());
    const char* strHigh = csv.get(i, cfg.head.high.c_str());
    const char* strLow = csv.get(i, cfg.head.low.c_str());
    const char* strVolume = csv.get(i, cfg.head.volume.c_str());
    const char* strOpenInterest = csv.get(i, cfg.head.openInterest.c_str());

    if (cfg.timeType == CSVTIME_FORMAT_STR) {
      tm curtm;
      strptime(strTime, "%Y-%m-%d %H:%M:%S", &curtm);

      curtm.tm_isdst = -1;
      curtime = mktime(&curtm);
    } else if (cfg.timeType == CSVTIME_TIMESTAMP) {
      curtime = std::stoll(strTime);
    }

    double o = std::stod(strOpen);
    open = o * cfg.scalePrice;

    double c = std::stod(strClose);
    close = c * cfg.scalePrice;

    double h = std::stod(strHigh);
    high = h * cfg.scalePrice;

    double l = std::stod(strLow);
    low = l * cfg.scalePrice;

    double v = std::stod(strVolume);
    volume = v * cfg.scaleVolume;

    double oi = std::stod(strOpenInterest);
    openInterest = oi * cfg.scaleVolume;

    lstCandle.push(curtime, open, close, high, low, volume, openInterest);
  }

  return true;
}

}  // namespace trading