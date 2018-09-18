#include <vector>

template <typename TimeType, typename PriceType, typename VolumeType>
struct CandleData {
  TimeType curtime;
  PriceType open, close, high, low;
  VolumeType volume;
  VolumeType openInterest;

  CandleData(TimeType ct, PriceType o, PriceType c, PriceType h, PriceType l,
             VolumeType v, VolumeType oi)
      : curtime(ct),
        open(o),
        close(c),
        high(h),
        low(l),
        volume(v),
        openInterest(oi) {}

  bool operator<(const CandleData &cd) const { return curtime < cd.curtime; }
};

template <typename TimeType, typename PriceType, typename VolumeType>
class CandleList {
 public:
  typedef CandleData<TimeType, PriceType, VolumeType> CandleDataT;
  typedef std::vector<CandleDataT> List;

 public:
  CandleList() : m_offTime(60) {}
  ~CandleList() {}

 public:
  void setTimeOff(TimeType off) { m_offTime = off; }
  void push(TimeType ct, PriceType o, PriceType c, PriceType h, PriceType l,
            VolumeType v, VolumeType oi) {
    CandleDataT cd(ct, o, c, h, l, v, oi);
    m_lst.push_back(cd);
  }

  void format() {
    if (m_lst.size() < 2) {
      return;
    }
    std::sort(m_lst.begin(), m_lst.end());
    TimeType bt = m_lst[0].curtime;
    TimeType et = m_lst[m_lst.size() - 1].curtime;
  }

 protected:
  TimeType m_offTime;
  List m_lst;
};