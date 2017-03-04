#ifndef _OCCURENCEDERIVER_H_
#define _OCCURENCEDERIVER_H_

#include "Database.h"

class Database;

using namespace std;

class OccurenceDeriver {
    int tableSize;
public:
    vector<vector<int>> table;

    OccurenceDeriver(Database &database);

    int GetNumOcc(int item) const;

    vector<int> GetTable(int item) const;

    vector<vector<int>> GetAllTable() const;

    void Clear();

    void Print(ostream &out);

};

#endif // _OCCURENCEDERIVER_H_
