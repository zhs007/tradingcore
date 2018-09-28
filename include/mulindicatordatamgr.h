#ifndef __TRADINGCORE_MULINDICATORDATAMGR_H__
#define __TRADINGCORE_MULINDICATORDATAMGR_H__

#include <functional>
#include <map>
#include <vector>
#include "indicator.h"

namespace trading {

template <typename ValueType>
class MulIndicatorDataMgr {
 public:
  typedef BaseIndicatorDataMgr<ValueType> BaseIndicatorDataMgrT;
  typedef std::map<int, BaseIndicatorDataMgrT*> IndicatorDataMgrMap;
  typedef typename IndicatorDataMgrMap::iterator IndicatorDataMgrMapIter;
  typedef IndicatorDataMgr<ValueType, 1> IndicatorDataMgrT_1;
  typedef IndicatorDataMgr<ValueType, 2> IndicatorDataMgrT_2;
  typedef IndicatorDataMgr<ValueType, 3> IndicatorDataMgrT_3;
  typedef IndicatorDataMgr<ValueType, 4> IndicatorDataMgrT_4;
  typedef IndicatorDataMgr<ValueType, 5> IndicatorDataMgrT_5;
  typedef IndicatorDataMgr<ValueType, 6> IndicatorDataMgrT_6;
  typedef IndicatorDataMgr<ValueType, 7> IndicatorDataMgrT_7;
  typedef IndicatorDataMgr<ValueType, 8> IndicatorDataMgrT_8;
  typedef IndicatorDataMgr<ValueType, 9> IndicatorDataMgrT_9;
  typedef IndicatorDataMgr<ValueType, 10> IndicatorDataMgrT_10;
  typedef IndicatorDataMgr<ValueType, 11> IndicatorDataMgrT_11;
  typedef IndicatorDataMgr<ValueType, 12> IndicatorDataMgrT_12;
  typedef IndicatorDataMgr<ValueType, 13> IndicatorDataMgrT_13;
  typedef IndicatorDataMgr<ValueType, 14> IndicatorDataMgrT_14;
  typedef IndicatorDataMgr<ValueType, 15> IndicatorDataMgrT_15;

 public:
  MulIndicatorDataMgr() {}
  ~MulIndicatorDataMgr() {}

 public:
  BaseIndicatorDataMgrT* _newIndicatorDataMgr(int valueNums) {
    switch (valueNums) {
      case 1:
        return new IndicatorDataMgrT_1();
      case 2:
        return new IndicatorDataMgrT_2();
      case 3:
        return new IndicatorDataMgrT_3();
      case 4:
        return new IndicatorDataMgrT_4();
      case 5:
        return new IndicatorDataMgrT_5();
      case 6:
        return new IndicatorDataMgrT_6();
      case 7:
        return new IndicatorDataMgrT_7();
      case 8:
        return new IndicatorDataMgrT_8();
      case 9:
        return new IndicatorDataMgrT_9();
      case 10:
        return new IndicatorDataMgrT_10();
      case 11:
        return new IndicatorDataMgrT_11();
      case 12:
        return new IndicatorDataMgrT_12();
      case 13:
        return new IndicatorDataMgrT_13();
      case 14:
        return new IndicatorDataMgrT_14();
      case 15:
        return new IndicatorDataMgrT_15();
      default:
        assert(false);
    }
    return NULL;
  }

  BaseIndicatorDataMgrT* getIndicatorDataMgr(int valueNums) {
    IndicatorDataMgrMapIter it = m_map.find(valueNums);
    if (it == m_map.end()) {
      m_map[valueNums] = _newIndicatorDataMgr(valueNums);
      //   new IndicatorDataMgr<ValueType, ValueNums>();  //
      //   newIndicatorDataMgr<ValueType,
      // valueNums>();

      it = m_map.find(valueNums);
    }

    assert(it != m_map.end());

    return (it->second);
  }

 protected:
  IndicatorDataMgrMap m_map;
};

// template <typename MoneyType, typename VolumeType, typename ValueType>
// static IndicatorMgr<ValueType, VolumeType, ValueType>* getIndicatorMgr() {
//   typedef IndicatorMgr<ValueType, VolumeType, ValueType> IndicatorMgrT;

//   static IndicatorMgrT* pMgr = new IndicatorMgrT();

//   return pMgr;
// }

}  // namespace trading

#endif  // __TRADINGCORE_MULINDICATORDATAMGR_H__