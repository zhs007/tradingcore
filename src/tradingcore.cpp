#include "../include/tradingcore.h"
#include <time.h>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace trading {

static char s_defaultTimeZone[128];

void setTimeZone(const char* tz) { strcpy(s_defaultTimeZone, tz); }

const char* getTimeZone() { return s_defaultTimeZone; }

bool loadCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  const char* tz, CSVConfig& cfg) {
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
      curtime = str2time(strTime, tz);
      // tm curtm;
      // strptime(strTime, "%Y-%m-%d %H:%M:%S", &curtm);

      // curtm.tm_isdst = -1;
      // curtime = mktime(&curtm);
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

bool saveCSVInt64(CandleListInt64& lstCandle, const char* filename,
                  const char* tz, CSVConfig& cfg) {
  std::ofstream outfile;
  outfile.open(filename, std::ios::out | std::ios::trunc);

  outfile << cfg.head.curtime << ",";
  outfile << cfg.head.open << ",";
  outfile << cfg.head.close << ",";
  outfile << cfg.head.high << ",";
  outfile << cfg.head.low << ",";
  outfile << cfg.head.volume << ",";
  outfile << cfg.head.openInterest << std::endl;

  for (int i = 0; i < lstCandle.getLength(); ++i) {
    const CandleDataInt64& cd = lstCandle.get(i);
    time_t ct = cd.curtime;

    if (cfg.timeType == CSVTIME_FORMAT_STR) {
      char buf[128];
      time2str(buf, 128, ct, tz);
      // tm* curtm = localtime(&ct);
      // char buf[128];
      // strftime(buf, 128, "%Y-%m-%d %H:%M:%S", curtm);
      outfile << buf << ",";
    } else if (cfg.timeType == CSVTIME_TIMESTAMP) {
      outfile << ct << ",";
    }

    double open = cd.open / cfg.scalePrice;
    double close = cd.close / cfg.scalePrice;
    double high = cd.high / cfg.scalePrice;
    double low = cd.low / cfg.scalePrice;

    outfile << std::fixed << std::setprecision(2) << open << ",";
    outfile << std::fixed << std::setprecision(2) << close << ",";
    outfile << std::fixed << std::setprecision(2) << high << ",";
    outfile << std::fixed << std::setprecision(2) << low << ",";

    double vol = cd.volume / cfg.scaleVolume;
    double oi = cd.openInterest / cfg.scaleVolume;

    outfile << std::fixed << std::setprecision(2) << vol << ",";
    outfile << std::fixed << std::setprecision(2) << oi << std::endl;
  }

  outfile.close();

  return true;
}

bool saveOrderList2CSVInt64(OrderListInt64& lstOrder, const char* filename,
                            int scalePrice, int scaleVolume) {
  std::ofstream outfile;
  outfile.open(filename, std::ios::out | std::ios::trunc);

  outfile << "id,";
  outfile << "category,";
  outfile << "ordertype,";
  outfile << "orderside,";
  outfile << "createtime,";
  outfile << "price,";
  outfile << "volume,";
  outfile << "avgprice,";
  outfile << "lastvolume,";
  outfile << "tradetime";
  outfile << std::endl;

  for (auto it = lstOrder.begin(); it != lstOrder.end(); ++it) {
    const OrderInt64& co = **it;

    outfile << co.orderID << ",";
    outfile << co.categoryName << ",";
    outfile << getOrderTypeStr(co.orderType) << ",";
    outfile << getOrderSideStr(co.orderSide) << ",";
    outfile << co.ct << ",";

    double price = co.destPrice / scalePrice;
    double volume = co.destVolume / scaleVolume;
    double avgprice = co.avgPrice / scalePrice;
    double lastvolume = co.lastVolume / scaleVolume;

    outfile << std::fixed << std::setprecision(2) << price << ",";
    outfile << std::fixed << std::setprecision(2) << volume << ",";
    outfile << std::fixed << std::setprecision(2) << avgprice << ",";
    outfile << std::fixed << std::setprecision(2) << lastvolume << ",";

    outfile << std::fixed << std::setprecision(2) << co.ct << std::endl;
  }

  outfile.close();

  return true;
}

bool saveTradeList2CSVInt64(TradeListInt64& lstTrade, const char* filename,
                            int scalePrice, int scaleVolume) {
  std::ofstream outfile;
  outfile.open(filename, std::ios::out | std::ios::trunc);

  outfile << "id,";
  outfile << "category,";
  outfile << "orderid,";
  outfile << "side,";
  outfile << "tradetime,";
  outfile << "price,";
  outfile << "volume";
  outfile << std::endl;

  for (auto it = lstTrade.begin(); it != lstTrade.end(); ++it) {
    const TradeInt64& ct = **it;

    outfile << ct.tradeID << ",";
    outfile << ct.categoryName << ",";

    if (ct.tradeSide == TRADE_BUY) {
      outfile << ct.buyOrderID << ",";
    } else {
      outfile << ct.sellOrderID << ",";
    }

    outfile << getTradeSideStr(ct.tradeSide) << ",";
    outfile << ct.ct << ",";

    double price = ct.price / scalePrice;
    double volume = ct.vol / scaleVolume;

    outfile << std::fixed << std::setprecision(2) << price << ",";
    outfile << std::fixed << std::setprecision(2) << volume << std::endl;
  }

  outfile.close();

  return true;
}

}  // namespace trading