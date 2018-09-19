#ifndef __TRADINGCORE_CANDLE_H__
#define __TRADINGCORE_CANDLE_H__

#include <assert.h>
#include <vector>

namespace trading {

template <typename PriceType, typename VolumeType>
struct CandleData {
  time_t curtime;
  PriceType open, close, high, low;
  VolumeType volume;
  VolumeType openInterest;

  CandleData(time_t ct, PriceType o, PriceType c, PriceType h, PriceType l,
             VolumeType v, VolumeType oi)
      : curtime(ct),
        open(o),
        close(c),
        high(h),
        low(l),
        volume(v),
        openInterest(oi) {}

  bool operator<(const CandleData& cd) const { return curtime < cd.curtime; }

  void format() {
    if (low > open) {
      low = open;
    }
    if (high < open) {
      high = open;
    }

    if (low > close) {
      low = close;
    }
    if (high < close) {
      high = close;
    }

    if (high < low) {
      std::swap(low, high);
    }

    if (volume < 0) {
      volume = 0;
    }

    if (openInterest < 0) {
      openInterest = 0;
    }
  }
};

template <typename PriceType, typename VolumeType>
class CandleList {
 public:
  typedef CandleData<PriceType, VolumeType> CandleDataT;
  typedef std::vector<CandleDataT> List;
  typedef typename std::vector<CandleDataT>::iterator ListIter;

 public:
  CandleList() : m_offTime(60) {}
  ~CandleList() {}

 public:
  void setTimeOff(time_t off) { m_offTime = off; }

  void push(time_t ct, PriceType o, PriceType c, PriceType h, PriceType l,
            VolumeType v, VolumeType oi) {
    CandleDataT cd(ct, o, c, h, l, v, oi);
    m_lst.push_back(cd);
  }

  void push(CandleDataT& cd) { m_lst.push_back(cd); }

  bool format() {
    if (m_lst.size() < 2) {
      return true;
    }
    std::sort(m_lst.begin(), m_lst.end());
    time_t bt = formatTime(m_lst[0].curtime);
    time_t et = formatTime(m_lst[m_lst.size() - 1].curtime);
    if (bt >= et) {
      return false;
    }

    time_t ct = bt;
    ListIter preit = m_lst.end();
    for (ListIter it = m_lst.begin(); it != m_lst.end(); ++it) {
      time_t cnt = formatTime(it->curtime);
      if (cnt != it->curtime) {
        it->curtime = cnt;
      }

      if (cnt > ct) {
        assert(preit != m_lst.end());
        do {
          CandleDataT cd(ct, preit->close, preit->close, preit->close,
                         preit->close, 0, preit->openInterest);
          preit = m_lst.insert(it, cd);
          it = preit + 1;
          ct += m_offTime;
        } while (ct < cnt);
      } else if (cnt < ct) {
        assert(preit != m_lst.end());
        CandleDataT cd(ct, preit->close, it->close, it->high, it->low,
                       it->volume, it->openInterest);

        cd.format();

        do {
          it = m_lst.erase(it);
          if (it == m_lst.end()) {
            break;
          }
          cnt = formatTime(it->curtime);
          if (cnt <= ct) {
            it->format();

            cd.close = it->close;
            cd.volume += it->volume;
            cd.openInterest = it->openInterest;

            if (cd.low > it->low) {
              cd.low = it->low;
            }
            if (cd.high < it->high) {
              cd.high = it->high;
            }
          }
        } while (cnt <= ct);

        if (it == m_lst.end()) {
          it = m_lst.insert(m_lst.end(), cd);
        } else {
          it = m_lst.insert(it, cd);
        }
      } else {
        ct += m_offTime;
      }

      formatCandle(preit, it);
      preit = it;
    }

    return true;
  }

  time_t formatTime(time_t ct) { return ct - (ct % m_offTime); }

  void formatCandle(ListIter preit, ListIter it) {
    if (preit != m_lst.end()) {
      if (it->open != preit->close) {
        it->open = preit->close;
      }
    }

    it->format();
  }

  int getLength() const { return m_lst.size(); }

  const CandleDataT& get(int index) const { return m_lst[index]; }

  void clear() { m_lst.clear(); }

 protected:
  time_t m_offTime;
  List m_lst;
};

}  // namespace trading

#endif  // __TRADINGCORE_CANDLE_H__