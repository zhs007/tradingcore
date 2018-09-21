#ifndef __TRADINGCORE_PNL_H__
#define __TRADINGCORE_PNL_H__

#include <assert.h>
#include <vector>

namespace trading {

template <typename PercentType>
class PNL {
 public:
  typedef std::vector<PercentType> List;
  typedef typename std::vector<PercentType>::iterator ListIter;

 public:
  PNL() {}
  ~PNL() {}

 public:
  int getLength() const { return m_lst.size(); }

  PercentType get(int index) const { return m_lst[index]; }

  void clear() { m_lst.clear(); }

  void push(PercentType p) {
      m_lst.push_back(p);
  }

 protected:
  List m_lst;
};

}  // namespace trading

#endif  // __TRADINGCORE_PNL_H__