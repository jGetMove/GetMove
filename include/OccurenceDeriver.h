#ifndef _OCCURENCEDERIVER_H_
#define _OCCURENCEDERIVER_H_

#include <iostream>
#include <vector>
#include "Database.h"

class Database;
using namespace std;

class OccurenceDeriver {
  int tableSize;
public:
  vector<vector<int> > table;
  OccurenceDeriver(Database &database);
  int GetNumOcc(int item) const;
  vector<int> GetTable(int item) const;
  vector<vector<int> > GetAllTable() const;
  void Clear();
  void Print(ostream &out);
  void Push(int item, int table);
};
#endif // _OCCURENCEDERIVER_H_
