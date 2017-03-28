#include "../../include/OccurenceDeriver.h"

/*************************************************************************
* Constructor
**************************************************************************/
OccurenceDeriver::OccurenceDeriver(Database &database) {
  tableSize = database.GetMaxItem() + 1; // tableSize = max_cluster_id + 1
  table.resize(tableSize);

  int size = database.GetNumberOfTransaction();
  for (int i = 0; i < (int)size; i++) {
    // Pour chaque transaction
    const Transaction &transaction = database.GetTransaction(i);

    // pour chauque itemset de la transaction.
    vector<int> itemsets = transaction.itemsets;
    for (vector<int>::iterator iter = itemsets.begin(); iter != itemsets.end(); iter++) {
      // je rajoute [idTransaction] a   table[idCluster]
      //		for (int j = 0; j < (int)itemsets.size(); j++) {
      table[*iter].push_back(i);
      //cout<<"occurence table : "<<i<<endl;
    }
  }
}

/***************************************************************************
* get the number of occurence of item
****************************************************************************/
int OccurenceDeriver::GetNumOcc(int item) const {
  return (int)table[item].size();
}

/****************************************************************************
* clear table
*****************************************************************************/
void OccurenceDeriver::Clear() {
  for (int i = 0; i < (int)table.size(); i++)
    table[i].clear();
}

/******************************************************************************
* Print Occurence Deriver
*******************************************************************************/
void OccurenceDeriver::Print(ostream &out) {

}

/********************************************************************************
 * Push
 *******************************************************************************/
void OccurenceDeriver::Push(int item, int id) {
  table[item].push_back(id);
}

/*********************************************************************************
 * GetTable
 *********************************************************************************/
vector<int> OccurenceDeriver::GetTable(int item) const {
  return table[item];
}


/*********************************************************************************
 * GetAllTable
 *********************************************************************************/
vector<vector<int> > OccurenceDeriver::GetAllTable() const {
  return table;
}
