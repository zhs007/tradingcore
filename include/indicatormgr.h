#ifndef __TRADINGCORE_INDICATORMGR_H__
#define __TRADINGCORE_INDICATORMGR_H__

#include <functional>
#include <map>
#include <vector>
#include "candle.h"
#include "indicator.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType>
class IndicatorMgr {
 public:
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef std::function<IndicatorT*(IndicatorParam& param,
                                    CandleListT& lstCandle)>
      FuncNewIndicatorT;
  typedef std::map<std::string, FuncNewIndicatorT> FuncNewIndicatorMap;
  typedef typename FuncNewIndicatorMap::iterator FuncNewIndicatorMapIter;

 public:
  IndicatorMgr() {}
  virtual ~IndicatorMgr() {}

 public:
  void regIndicator(const char* name, FuncNewIndicatorT funcNew) {
    m_map[name] = funcNew;
  }

  IndicatorT* newIndicator(const char* name, IndicatorParam& param,
                           CandleListT& lstCandle) {
    FuncNewIndicatorMapIter it = m_map.find(name);
    if (it != m_map.end()) {
      FuncNewIndicatorT func = it->second;
      return func(param, lstCandle);
    }

    return NULL;
  }

 protected:
  FuncNewIndicatorMap m_map;
};

// template <typename MoneyType, typename VolumeType, typename ValueType>
// static IndicatorMgr<ValueType, VolumeType, ValueType>* getIndicatorMgr() {
//   typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;

//   static IndicatorMgrT* pMgr = new IndicatorMgrT();

//   return pMgr;
// }

}  // namespace trading

#endif  // __TRADINGCORE_INDICATORMGR_H__