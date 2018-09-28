#ifndef __TRADINGCORE_CATEGORY_H__
#define __TRADINGCORE_CATEGORY_H__

#include <map>
#include <string>

namespace trading {

enum FEE_TYPE { FEE_FREE = 0, FEE_NORMAL, FEE_MIN, FEE_MAX, FEE_MINMAX };

template <typename MoneyType, typename VolumeType>
struct AvgPriceResult {
  MoneyType avgPrice;
  VolumeType lastVolume;

  AvgPriceResult(MoneyType p, VolumeType v) : avgPrice(p), lastVolume(v) {}
};

template <typename MoneyType, typename VolumeType>
struct CategoryConfig {
  typedef AvgPriceResult<MoneyType, VolumeType> AvgPriceResultT;

  std::string code;

  VolumeType unit;
  MoneyType price;

  // deposit
  // base unit is 1/1000000
  int64_t perDeposit;

  // fee
  // base unit is 1/1000000
  FEE_TYPE feeType;
  int feeMaker;
  int feeTaker;
  MoneyType feeMax;
  MoneyType feeMin;

  CategoryConfig() {
    unit = 0;
    price = 0;

    perDeposit = 0;

    setFee(FEE_FREE);
  }

  MoneyType countValue(VolumeType vol) const {
    return std::abs(vol * price / unit);
  }

  MoneyType countValueEx(MoneyType curprice, VolumeType vol) const {
    return std::abs(vol * curprice / unit);
  }

  MoneyType countAvgPrice(MoneyType cost, VolumeType vol) const {
    return std::abs(unit * cost / vol);
  }

  AvgPriceResultT countAvgPriceEx(MoneyType oldPrice, VolumeType oldVol,
                                  MoneyType curPrice, VolumeType curVol) const {
    AvgPriceResultT ap(oldPrice, oldVol);

    if (curVol == 0) {
      return ap;
    }

    if (oldVol == 0) {
      ap.avgPrice = curPrice;
      ap.lastVolume = curVol;

      return ap;
    }

    // if curVol, oldVol > 0 or curVol, oldVol < 0
    if ((curVol > 0) == (oldVol > 0)) {
      MoneyType old = countValueEx(oldPrice, oldVol);
      MoneyType cur = countValueEx(curPrice, curVol);

      ap.avgPrice = countAvgPrice(old + cur, oldVol + curVol);
      ap.lastVolume += curVol;

      return ap;
    }

    // if left
    if (std::abs(oldVol) >= std::abs(curVol)) {
      ap.lastVolume += curVol;

      if (ap.lastVolume == 0) {
        ap.avgPrice = 0;
      }

      return ap;
    }

    // if change side
    ap.lastVolume += curVol;

    ap.avgPrice = curPrice;

    return ap;
  }

  void setUnitPrice(VolumeType u, MoneyType p) {
    unit = u;
    price = p;
  }

  void setDeposit(int64_t per) { perDeposit = per; }

  void setFee(FEE_TYPE ft, int fmaker = 0, int ftaker = 0, MoneyType fmin = 0,
              MoneyType fmax = 0) {
    feeType = ft;
    feeMaker = fmaker;
    feeTaker = ftaker;
    feeMax = fmax;
    feeMin = fmin;
  }

  MoneyType countFee(MoneyType cost) {
    if (feeType == FEE_NORMAL) {
      return cost * 1000000 / feeTaker;
    } else if (feeType == FEE_MIN) {
      MoneyType cur = cost * 1000000 / feeTaker;
      if (cur < feeMin) {
        return feeMin;
      }
      return cur;
    } else if (feeType == FEE_MAX) {
      MoneyType cur = cost * 1000000 / feeTaker;
      if (cur > feeMax) {
        return feeMax;
      }
      return cur;
    } else if (feeType == FEE_MINMAX) {
      MoneyType cur = cost * 1000000 / feeTaker;
      if (cur < feeMin) {
        return feeMin;
      }
      if (cur > feeMax) {
        return feeMax;
      }

      return cur;
    }

    return 0;
  }

  MoneyType countDeposit(MoneyType curprice, VolumeType vol) {
    return countValueEx(curprice, vol) * 1000000 / perDeposit;
  }
};

template <typename MoneyType, typename VolumeType>
struct CategoryInfo {
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef AvgPriceResult<MoneyType, VolumeType> AvgPriceResultT;

  std::string code;
  VolumeType vol;
  MoneyType avgPrice;

  MoneyType curPrice;

  void chgVolume(CategoryConfigT& cfg, MoneyType price, VolumeType off) {
    AvgPriceResultT ap = cfg.countAvgPriceEx(avgPrice, vol, price, off);

    vol = ap.lastVolume;
    avgPrice = ap.avgPrice;
    //   if (off == 0) {
    //     return;
    //   }

    //   if (vol == 0) {
    //     vol = off;
    //     avgPrice = price;
    //   } else {
    //     if ((off > 0) == (vol > 0)) {
    //       avgPrice = cfg.countAvgPriceEx(avgPrice, vol, price, off);

    //       // MoneyType old = cfg.countValueEx(avgPrice, vol);
    //       // MoneyType cur = cfg.countValueEx(price, off);

    //       vol += off;
    //       // avgPrice = cfg.countAvgPrice(old + cur, vol);
    //     } else {
    //       if (std::abs(vol) >= std::abs(off)) {
    //         vol += off;

    //         if (vol == 0) {
    //           avgPrice = 0;
    //         }
    //       } else {
    //         vol += off;

    //         avgPrice = price;
    //       }
    //     }
    //   }
  }
};

template <typename MoneyType, typename VolumeType>
class CategoryMgr {
 public:
  typedef CategoryConfig<MoneyType, VolumeType> CategoryConfigT;
  typedef std::pair<std::string, CategoryConfigT> CategoryConfigPair;
  typedef std::map<std::string, CategoryConfigT> CategoryConfigMap;
  typedef typename CategoryConfigMap::iterator CategoryConfigMapIterT;
  typedef
      typename CategoryConfigMap::const_iterator CategoryConfigMapConstIterT;

 public:
  CategoryMgr() {}
  ~CategoryMgr() {}

 public:
  MoneyType countCategoryValue(const char* code, VolumeType vol,
                               MoneyType price) const {
    CategoryConfigMapConstIterT it = m_map.find(code);
    if (it != m_map.end()) {
      const CategoryConfigT& cfg = (it->second);
      return cfg.countValueEx(vol, price);
    }

    return 0;
  }

  const CategoryConfigT* getConfig(const char* code) const {
    CategoryConfigMapConstIterT it = m_map.find(code);
    if (it != m_map.end()) {
      return &(it->second);
    }

    return NULL;
  }

  CategoryConfigT& newCategory(const char* code) {
    CategoryConfigPair p;
    p.first = code;

    m_map.insert(p);

    return m_map[code];
  }

 protected:
  CategoryConfigMap m_map;
};

}  // namespace trading

#endif  // __TRADINGCORE_CATEGORY_H__