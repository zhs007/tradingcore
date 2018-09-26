#ifndef __TRADINGCORE_CANDLE_H__
#define __TRADINGCORE_CANDLE_H__

#include <assert.h>
#include <vector>

namespace trading {

template <typename MoneyType, typename VolumeType>
struct CandleData {
  typedef CandleData<MoneyType, VolumeType> CandleDataT;

  time_t curtime;
  MoneyType open, close, high, low;
  VolumeType volume;
  VolumeType openInterest;

  CandleData(time_t ct, MoneyType o, MoneyType c, MoneyType h, MoneyType l,
             VolumeType v, VolumeType oi)
      : curtime(ct),
        open(o),
        close(c),
        high(h),
        low(l),
        volume(v),
        openInterest(oi) {}

  bool operator==(time_t ct) const { return curtime == ct; }

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

  void merge(CandleDataT& dat) {
    format();
    dat.format();

    if (dat.curtime > curtime) {
      close = dat.close;
    } else if (dat.curtime < curtime) {
      open = dat.open;
    }

    if (dat.low < low) {
      low = dat.low;
    }

    if (dat.high > high) {
      high = dat.high;
    }

    volume += dat.volume;
    if (dat.openInterest > openInterest) {
      openInterest = dat.openInterest;
    }
  }

  void mergeEx(CandleDataT& dat, bool btimeup) {
    format();
    dat.format();

    if (btimeup) {
      close = dat.close;
    } else {
      open = dat.open;
    }

    if (dat.low < low) {
      low = dat.low;
    }

    if (dat.high > high) {
      high = dat.high;
    }

    volume += dat.volume;
    if (dat.openInterest > openInterest) {
      openInterest = dat.openInterest;
    }
  }
};

template <typename MoneyType, typename VolumeType>
class CandleList {
 public:
  typedef CandleData<MoneyType, VolumeType> CandleDataT;
  typedef std::vector<CandleDataT> List;
  typedef typename std::vector<CandleDataT>::iterator ListIter;
  typedef typename std::vector<CandleDataT>::const_iterator ListConstIter;

 public:
  CandleList() : m_offTime(60) {}
  ~CandleList() {}

 public:
  void setTimeOff(time_t off) { m_offTime = off; }

  void push(time_t ct, MoneyType o, MoneyType c, MoneyType h, MoneyType l,
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
    for (ListIter it = m_lst.begin(); it != m_lst.end();) {
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

        formatCandle(preit, it);
        preit = it;
        ++it;
      } else if (cnt < ct) {
        assert(preit != m_lst.end());
        assert(preit->curtime == cnt);

        preit->mergeEx(*it, true);
        it = m_lst.erase(it);

        // CandleDataT cd(preit->curtime, preit->close, it->close, it->high,
        //                it->low, it->volume, it->openInterest);

        // cd.format();

        // do {
        //   it = m_lst.erase(it);
        //   if (it == m_lst.end()) {
        //     break;
        //   }
        //   cnt = formatTime(it->curtime);
        //   if (cnt < ct) {
        //     it->format();

        //     cd.close = it->close;
        //     cd.volume += it->volume;
        //     cd.openInterest = it->openInterest;

        //     if (cd.low > it->low) {
        //       cd.low = it->low;
        //     }
        //     if (cd.high < it->high) {
        //       cd.high = it->high;
        //     }
        //   }
        // } while (cnt < ct);

        // if (it == m_lst.end()) {
        //   it = m_lst.insert(m_lst.end(), cd);
        // } else {
        //   it = m_lst.insert(it, cd);
        // }
      } else {
        ct += m_offTime;

        formatCandle(preit, it);
        preit = it;
        ++it;
      }
    }

    return true;
  }

  time_t formatTime(time_t ct) {
    time_t ot = ct % m_offTime;
    if (ot > 0) {
      return ct - ot + m_offTime;
    }
    return ct;
  }

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

  const CandleDataT* findTime(time_t ct) const {
    ListConstIter it = std::find(m_lst.begin(), m_lst.end(), ct);
    if (it != m_lst.end()) {
      return &(*it);
    }

    return NULL;
  }

  const CandleDataT* findTimeEx(int& index, time_t ct) const {
    if (index < 0 || index >= m_lst.size()) {
      index = 0;
    }

    int pre = -1;

    for (; index < m_lst.size(); pre = index++) {
      if (m_lst[index].curtime == ct) {
        return &(m_lst[index]);
      } else if (m_lst[index].curtime > ct) {
        if (pre < 0) {
          return &(m_lst[index]);
        }

        // &(m_lst[pre]);
      }
    }

    return NULL;
  }

 protected:
  time_t m_offTime;
  List m_lst;
};

}  // namespace trading

#endif  // __TRADINGCORE_CANDLE_H__