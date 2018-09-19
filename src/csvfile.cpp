#include "../include/csvfile.h"
#include <fstream>
#include <streambuf>

bool isWrap(std::string& str, int index, int& off) {
  if (str[index] == '\r' && index + 1 < str.length() &&
      str[index + 1] == '\n') {
    off = 2;
    return true;
  }

  return false;
}

bool isSeparate(std::string& str, int index) {
  if (str[index] == ',') {
    return true;
  }

  return false;
}

bool CSVFile::load(const char* filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    return false;
  }

  std::string str((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());

  std::vector<std::string> lsthead;
  std::vector<std::string> lstrow;
  bool headend = false;
  int off = 1;
  int curstart = 0;
  for (int i = 0; i < str.length(); i += off) {
    off = 1;

    if (!headend) {
      if (isWrap(str, i, off)) {
        headend = true;
        curstart = i + off;
      } else if (isSeparate(str, i)) {
        if (curstart == i) {
          std::string curstr = "__" + std::to_string(lsthead.size());
          lsthead.push_back(curstr);
        } else {
          std::string curstr = str.substr(curstart, i - curstart - 1);
          lsthead.push_back(curstr);
        }

        curstart = i + off;
      }
    } else {
      if (isWrap(str, i, off)) {
        Row* pRow = new Row();
        for (int i = 0; i < lsthead.size(); ++i) {
          if (i < lstrow.size()) {
            (*pRow)[lsthead[i].c_str()] = lstrow[i].c_str();
          } else {
            (*pRow)[lsthead[i].c_str()] = "";
          }
        }

        m_lst.push_back(pRow);

        curstart = i + off;
        lstrow.clear();
      } else if (isSeparate(str, i)) {
        if (curstart == i) {
          lstrow.push_back("");
        } else {
          std::string curstr = str.substr(curstart, i - curstart - 1);
          lstrow.push_back(curstr);
        }

        curstart = i + off;
      }
    }
  }
  return true;
}

void CSVFile::clear() {
  for (ListIter it = m_lst.begin(); it != m_lst.end(); ++it) {
    delete *it;
  }

  m_lst.clear();
}

const char* CSVFile::get(int y, const char* str) const {
  assert(y >= 0);
  assert(y < m_lst.size());

  return (*m_lst[y])[str].c_str();
}