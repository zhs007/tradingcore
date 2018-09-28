#ifndef __TRADINGCORE_CONTEXT_H__
#define __TRADINGCORE_CONTEXT_H__

#include <map>
#include "indicator.h"
#include "indicatormgr.h"
#include "ordermgr.h"
#include "trademgr.h"
#include "mulindicatordatamgr.h"

namespace trading {

template <typename MoneyType, typename VolumeType, typename ValueType,
          typename PercentType>
struct Context {
  typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;
  typedef TradeMgr<MoneyType, VolumeType> TradeMgrT;
  typedef OrderMgr<MoneyType, VolumeType> OrderMgrT;
  typedef MulIndicatorDataMgr<ValueType> MulIndicatorDataMgrT;
  // typedef BaseIndicatorDataMgr<ValueType> BaseIndicatorDataMgrT;
  // typedef std::map<int, BaseIndicatorDataMgrT*> BaseIndicatorDataMgrMap;
  // typedef
  //     typename BaseIndicatorDataMgrMap::iterator BaseIndicatorDataMgrMapIter;

  IndicatorMgrT mgrIndicator;
  TradeMgrT mgrTrade;
  OrderMgrT mgrOrder;
  MulIndicatorDataMgrT mgrIndicatorData;
  // BaseIndicatorDataMgrMap mapBIDMgr;

  // void newIndicatorDataMgr(int valueNums) {
  //   mapBIDMgr[valueNums] = newIndicatorDataMgr<ValueType, valueNums>();
  // }

  // BaseIndicatorDataMgrT* getIndicatorDataMgr(int valueNums) {
  //   BaseIndicatorDataMgrMapIter it = mapBIDMgr[valueNums];
  //   if (it == mapBIDMgr.end()) {
  //     newIndicatorDataMgr(valueNums);

  //     it = mapBIDMgr[valueNums];
  //   }

  //   assert(it != mapBIDMgr.end());

  //   return *(it->second);
  // }
};

}  // namespace trading

#endif  // __TRADINGCORE_CONTEXT_H__