#include <vector>
#include "candle.h"

template <typename ValueType, int ValueNums>
struct IndicatorData {
  ValueType val[ValueNums];
};

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType, int ValueNums>
class Indicator {
 public:
  typedef CandleList<TimeType, PriceType, VolumeType> CandleListT;
  typedef IndicatorData<ValueType, ValueNums> IndicatorDataT;
  typedef std::vector<IndicatorDataT> List;
  typedef typename std::vector<IndicatorDataT>::iterator ListIter;

 public:
  Indicator() {}
  virtual ~Indicator() {}

 public:
  virtual bool build() = 0;

 protected:
  List m_lst;
};