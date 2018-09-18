#ifndef __TRADINGCORE_INDICATOR_H__
#define __TRADINGCORE_INDICATOR_H__

#include <vector>
#include "candle.h"

template <typename ValueType>
class BaseIndicatorData {
 public:
  BaseIndicatorData() {}
  virtual ~BaseIndicatorData() {}

 public:
  virtual ValueType get(int index) = 0;
  virtual void set(int index, ValueType val) = 0;
};

template <typename ValueType, int ValueNums>
class IndicatorData : public BaseIndicatorData<ValueType> {
 public:
  IndicatorData() {}
  virtual ~IndicatorData() {}

 public:
  virtual ValueType get(int index) {
    assert(index >= 0);
    assert(index < ValueNums);

    return m_val[index];
  }

  virtual void set(int index, ValueType val) {
    assert(index >= 0);
    assert(index < ValueNums);

    m_val[index] = val;
  }

 protected:
  ValueType m_val[ValueNums];
};

template <typename ValueType>
class BaseIndicatorDataMgr {
 public:
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;

 public:
  BaseIndicatorDataMgr() {}
  ~BaseIndicatorDataMgr() {}

 public:
  virtual BaseIndicatorDataT* newData() = 0;

  virtual void deleteData(BaseIndicatorDataT* data) = 0;

 protected:
};

template <typename ValueType, int ValueNums>
class IndicatorDataMgr : public BaseIndicatorDataMgr<ValueType> {
 public:
  typedef IndicatorData<ValueType, ValueNums> IndicatorDataT;
  typedef std::vector<IndicatorDataT*> List;

 public:
  IndicatorDataMgr() : m_cacheNums(128) {}
  ~IndicatorDataMgr() {}

 public:
  IndicatorDataT* newData() {
    if (m_lst.empty()) {
      for (int i = 0; i < m_cacheNums; ++i) {
        m_lst.push_back(new IndicatorDataT());
      }
    }

    IndicatorDataT* pData = m_lst.back();
    m_lst.pop_back();
    return pData;
  }

  void deleteData(IndicatorDataT* data) { m_lst.push_back(data); }

 protected:
  int m_cacheNums;
  List m_lst;
};

template <typename ValueType, int ValueNums>
static IndicatorDataMgr<ValueType, ValueNums>* getIndicatorDataMgr() {
  static IndicatorDataMgr<ValueType, ValueNums>* pMgr =
      new IndicatorDataMgr<ValueType, ValueNums>();
  return pMgr;
}

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType>
class Indicator {
 public:
  typedef CandleList<TimeType, PriceType, VolumeType> CandleListT;
  typedef BaseIndicatorData<ValueType> BaseIndicatorDataT;
  typedef std::vector<BaseIndicatorDataT*> List;
  typedef typename std::vector<BaseIndicatorDataT*>::iterator ListIter;
  typedef BaseIndicatorDataMgr<ValueType> BaseIndicatorDataMgrT;

 public:
  Indicator(CandleListT& lstCandle, BaseIndicatorDataMgrT& mgrIndicatorData)
      : m_lstCandle(lstCandle), m_mgrIndicatorData(mgrIndicatorData) {}
  virtual ~Indicator() {}

 public:
  virtual bool build() = 0;

  BaseIndicatorDataT* newData() { return m_mgrIndicatorData.newData(); }

  BaseIndicatorDataT* pushNewData() {
    BaseIndicatorDataT* pDat = m_mgrIndicatorData.newData();
    m_lst.push_back(pDat);
    return pDat;
  }

  void deleteData(BaseIndicatorDataT* pData) {
    m_mgrIndicatorData.deleteData(pData);
  }

  void clear() {
    for (ListIter it = m_lst.begin(); it != m_lst.end(); ++it) {
      deleteData(*it);
    }

    m_lst.clear();
  }

  BaseIndicatorDataT* getData(int index) {
    assert(index >= 0);
    assert(index < m_lst.size());

    return m_lst[index];
  }

 protected:
  CandleListT& m_lstCandle;
  List m_lst;
  BaseIndicatorDataMgrT& m_mgrIndicatorData;
};

#endif  // __TRADINGCORE_INDICATOR_H__