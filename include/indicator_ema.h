#include "indicator.h"

template <typename TimeType, typename PriceType, typename VolumeType,
          typename ValueType, int ValueNums>
class Indicator_EMA : public Indicator<TimeType, PriceType, VolumeType, ValueType, ValueNums> {
 public:
  Indicator_EMA() {}
  virtual ~Indicator_EMA() {}

 public:
  virtual bool build() {
      
  }

 protected:
};