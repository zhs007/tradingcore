#include "../include/csvfile.h"
#include <fstream>
#include <streambuf>

bool CSVFile::load(const char* filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    return false;
  }

  std::string str((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());

  bool headend = false;
  for (int i = 0; i < str.length(); ++i) {
    if (!headend) {
      if (str[i] == ' ') {
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