#ifndef __TRADINGCORE_CSVFILE_H__
#define __TRADINGCORE_CSVFILE_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>

class CSVFile {
 public:
  typedef std::map<std::string, std::string> Row;
  typedef std::vector<Row*> List;
  typedef List::iterator ListIter;

 public:
  CSVFile() : m_Width(0), m_Height(0) {}
  ~CSVFile() { clear(); }

 public:
  bool load(const char* filename);
  void clear();

 protected:
  int m_Width, m_Height;
  List m_lst;
};

#endif  // __TRADINGCORE_CSVFILE_H__