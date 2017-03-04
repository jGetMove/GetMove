#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include <set>
#include "OccurenceDeriver.h"

using namespace std;

struct Transaction {
    int id;               // transaction id
    vector<int> itemsets; // list of items
    int label;

    void clear() {
        id = 0;
        label = 0;
        itemsets.clear();
    }
};

class Database {
    int max_item;                 // max item;
    int min_item;                 // min item;
public:
    vector<Transaction> database;            // transaction database
    // bitmap representation

    Database();

    ~Database();

    //for #Objects testing
    void ReadFile(const string &filename, int &numObject);

    void FindMaxAndMinItem();

    vector<int> GetItemset();

    Transaction GetTransaction(int i) const;

    int GetMaxItem() const;

    int GetMinItem() const;

    int GetNumberOfTransaction() const;

    ///////////////////////////////////////////////

};

#endif // _TRANSACTION_H_
