#ifndef __TRADINGCORE_SIMEXCHANGE_H__
#define __TRADINGCORE_SIMEXCHANGE_H__

#include "candle.h"
#include "csvfile.h"
#include "exchange.h"
#include "utils.h"

namespace trading {

template <typename MoneyType, typename VolumeType>
class SimExchange : public Exchange<MoneyType, VolumeType> {
 public:
  typedef Exchange<MoneyType, VolumeType> ExchangeT;
  typedef CandleList<MoneyType, VolumeType> CandleListT;

 public:
  SimExchange() {}
  ~SimExchange() {}

 public:
  bool loadCandleCSVFile(const char* filename, CSVConfig& cfg) {
    m_lstCandle.clear();

    CSVFile csv;
    bool loadok = csv.load(filename);
    if (!loadok) {
      return false;
    }

    for (int i = 0; i < csv.getLength(); ++i) {
      time_t curtime;
      MoneyType open, close, high, low;
      VolumeType volume;
      VolumeType openInterest;

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

      m_lstCandle.push(curtime, open, close, high, low, volume, openInterest);
    }
  }

 protected:
  CandleListT m_lstCandle;
};

}  // namespace trading

#endif  // __TRADINGCORE_SIMEXCHANGE_H__