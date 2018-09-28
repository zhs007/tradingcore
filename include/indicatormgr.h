#ifndef __TRADINGCORE_INDICATORMGR_H__
#define __TRADINGCORE_INDICATORMGR_H__

#include <functional>
#include <map>
#include <vector>
#include "candle.h"
#include "indicator.h"
#include "mulindicatordatamgr.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType>
class IndicatorMgr {
 public:
  typedef CandleList<MoneyType, VolumeType> CandleListT;
  typedef Indicator<MoneyType, VolumeType, ValueType> IndicatorT;
  typedef MulIndicatorDataMgr<ValueType> MulIndicatorDataMgrT;
  typedef std::function<IndicatorT*(IndicatorParam& param,
                                    CandleListT& lstCandle,
                                    MulIndicatorDataMgrT& mgrIndicatorDataMgr)>
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

  IndicatorT* newIndicator(MulIndicatorDataMgrT& mgr, const char* name, IndicatorParam& param,
                           CandleListT& lstCandle) {
    FuncNewIndicatorMapIter it = m_map.find(name);
    if (it != m_map.end()) {
      FuncNewIndicatorT func = it->second;
      return func(param, lstCandle, mgr);
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