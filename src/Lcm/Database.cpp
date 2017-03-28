#include "../../include/Database.h"
#include <cmath>

/***************************************************************
* constructor
****************************************************************/
Database::Database() {}

/***************************************************************
* destructor
****************************************************************/
Database::~Database() {}

/***************************************************************
 * Database Reduction
 ***************************************************************/
void Database::Reduction(vector<int> &transactionList, int core_i) {
  RemovePrefix(transactionList, core_i);
}

/***************************************************************
 * Remove infrequent Items from database
 **************************************************************/
inline void Database::RemoveItemsbyFreq(vector<int> &transactionList, OccurenceDeriver &occ, int core_i, int min_sup) {
  for (int item = min_item; item < core_i; item++) {
    if (occ.GetNumOcc(item) < min_sup) {
      for (int list = 0; list < (int)transactionList.size(); list++) {
  Transaction &transaction = database[transactionList[list]];
  remove(transaction.itemsets.begin(), transaction.itemsets.end(), item);
      }
    }
  }
}

/****************************************************************
 * Remove items inclued in itemset from database
 ****************************************************************/
inline void Database::RemoveItems(vector<int> &itemset, vector<int> &transactionList, int core_i) {
  for (int iter = 0; iter < (int)itemset.size() && itemset[iter] < core_i; iter++) {
    int item = itemset[iter];
    for (int list = 0; list < (int)transactionList.size(); list++) {
      Transaction &transaction = database[transactionList[list]];
      remove(transaction.itemsets.begin(), transaction.itemsets.end(), item);
    }
  }
}

/***************************************************************
 * Remove infrequent items from database
 **************************************************************/
void Database::RemoveItemsbyFreq(int min_sup) {
  OccurenceDeriver occ(*this);
  for (int item = min_item; item <= max_item; item++) {
    if (occ.GetNumOcc(item) < min_sup) {
      for (int list = 0; list < (int)database.size(); list++) {
  Transaction &transaction = database[list];
  remove(transaction.itemsets.begin(), transaction.itemsets.end(), item);
      }
    }
  }
  FindMaxAndMinItem();
}

/*****************************************************************
 * Remove prefixies that have the same saffix
 *****************************************************************/
inline void Database::RemovePrefix(vector<int> &transactionList, int core_i) {
  const vector<int> iSuffList = CalcurateIsuffList(transactionList, core_i);
  for (int item = min_item; item < core_i; item++) {
    bool flag = true;
    for (int list = 0; list < (int)iSuffList.size(); list++) {
      Transaction &transaction = database[iSuffList[list]];
      int iter = 0;
      for (; iter < (int)transaction.itemsets.size(); iter++) {
  if (transaction.itemsets[iter] == item) break;
      }
      if (iter == (int)transaction.itemsets.size()) {
  flag = false;
  break;
      }
    }
    if (flag == false) {
      for (int list = 0; list < (int)iSuffList.size(); list++) {
  Transaction &transaction = database[iSuffList[list]];
  for (vector<int>::iterator iter = transaction.itemsets.begin(); iter != transaction.itemsets.end(); iter++) {
    if (*iter == item) {
      transaction.itemsets.erase(iter);
      break;
    }
  }
      }
    }
  }
}

/***************************************************************************************
 * calcurate i_suffix
 ***************************************************************************************/
inline vector<int> Database::CalcurateIsuffList(vector<int> &transactionList, int core_i) {
  vector<int> iSuffList = transactionList;

  vector<int> delList;
  for (int item = max_item; item >= (int)core_i; item--) {
    vector<int> newList;
    for (int list = 0; list < (int)iSuffList.size(); list++) {
      const Transaction &transaction = database[iSuffList[list]];

      if (binary_search(transaction.itemsets.begin(), transaction.itemsets.end(), item) == true) {
  newList.push_back(iSuffList[list]);
      }
    }
    if (newList.size() != 0) {
      iSuffList = newList;
      newList.clear();
    }
  }

  return iSuffList;
}

/****************************************************************
* read file from filename
*****************************************************************/
void Database::ReadFile(const string &filename) {
  string line;
  Transaction transaction;
  int item;
  int total_id = 0;
  cout << "******** gestion DB *********" << endl;
  cout << "ouverture du fichier de la BD : " << filename << endl;
  ifstream is(filename.c_str());
  while (getline (is, line)) {
    transaction.clear();
    istrstream istrs ((char *)line.c_str());
    while (istrs >> item) transaction.itemsets.push_back(item);
    sort(transaction.itemsets.begin(), transaction.itemsets.end());
    transaction.id = total_id++;
    database.push_back(transaction);
  }
  FindMaxAndMinItem();
}

/****************************************************************
* read file from filename
*****************************************************************/
void Database::ReadFile(const string &filename, int &numObject) {
  string line;
  Transaction transaction;
  int item;
  int total_id = 0;
  int objectTemp = 0; // for Odb testing

  ifstream is(filename.c_str());
  while (getline (is, line)) {  // pour chaque object du fichier
    /*if(objectTemp == numObject){
       break;
    }*/
    // On reinitialise la transaction
    transaction.clear();
    bool empty = false;

    istrstream istrs ((char *)line.c_str());
    while (istrs >> item){ // pour chaque cluster d'un object
      transaction.itemsets.push_back(item); // on ajoute le numero du cluster dans l'itemset de la transaction
      empty = true; // l'objet à au moins un cluster
    }

    if(empty == false){ // si l'object n'as pas de cluster alors il prends un cluster nommé 7777777
        transaction.itemsets.push_back(7777777);
    }

    // on classe les numéros des clusters associés à l'object par ordre croissant
    sort(transaction.itemsets.begin(), transaction.itemsets.end());
    transaction.id = total_id++; // on nomme la transaction
    database.push_back(transaction); // et on l'ajoute à la base.

  //     cout << "database transactions :" << endl;
  //    for (int j = 0; j < transaction.itemsets.size(); j++)
  //        {
  //            cout << transaction.itemsets[j] <<" ";
  //        }
    objectTemp++; // on incremente le nombre d'objets
  }
    numObject=objectTemp; // numobject prends le total du nombre d'objets
    //  cout << "\t Object temps = " << objectTemp << endl;
  FindMaxAndMinItem(); // Associe à max_item et min_item le cluster le plus haut et le plus bas.
}

/****************************************************************
* read file from filename
*****************************************************************/
void Database::ReadFile(const string &filename, const string &Object_filename) {
  string line;
  Transaction transaction;
  int item;
  int total_id = 0;
  int objectTemp = 1; // for Odb testing

  //read object ID from file////////////////
  vector<int> objectList; objectList.clear();
  ifstream ob(Object_filename.c_str());
  while (getline (ob, line)) {
    istrstream istrs ((char *)line.c_str());
    while (istrs >> item) objectList.push_back(item);
  }
    //////////////////////////////////////////

  ifstream is(filename.c_str());
  while (getline (is, line)) {
    bool flag = false;
    for(int j = 0; j < int(objectList.size()); j++){
       if(objectList[j] == objectTemp){
          flag = true;
       }
    }
    if(flag == true){
       transaction.clear();
       istrstream istrs ((char *)line.c_str());
       while (istrs >> item) transaction.itemsets.push_back(item);
       sort(transaction.itemsets.begin(), transaction.itemsets.end());
       transaction.id = total_id++;
       database.push_back(transaction);
    }
    objectTemp++;
  }
  FindMaxAndMinItem();
}

/*****************************************************************
* read file and label from filename
******************************************************************/
void Database::ReadFileAddLabel(const string &filename) {
  string line;
  Transaction transaction;
  int item;
  int total_id = 0;
  ifstream is(filename.c_str());
  while (getline (is, line)) {
    transaction.clear();
    istrstream istrs ((char *)line.c_str());
    while (istrs >> item) transaction.itemsets.push_back(item);
    transaction.id = total_id++;
    transaction.label = transaction.itemsets.back();
    transaction.itemsets.pop_back();
  }
}

/*******************************************************************
* find max and min item from database
********************************************************************/
void Database::FindMaxAndMinItem() {
  max_item = 0;
  min_item = 100000000;
  for (int i = 0; i < (int)database.size(); i++) { // pour chaque transaction
    Transaction &transaction = database[i];
    vector<int> &itemsets = transaction.itemsets; // pour chauque itemset de la transaction
    for (int j = 0; j < (int)itemsets.size(); j++) {// cherche le numero du cluster le plus éleve ou le plus bas
      if (itemsets[j] > max_item) max_item = itemsets[j];
      if (itemsets[j] < min_item) min_item = itemsets[j];
    }
  }
}

/*******************************************************************
* GetItemset()
* renvoie tous les clusters contenus dans database (dans toutes les transaction)
********************************************************************/
vector<int> Database::GetItemset() {
    set<int> totalItem; // liste chainée

    for (int i = 0; i < (int)database.size(); i++) {
        Transaction &transaction = database[i];
        vector<int> &itemsets = transaction.itemsets;

        for (int j = 0; j < (int)itemsets.size(); j++) {
            totalItem.insert(itemsets[j]);
        }
    }
    vector<int> allItem;

    for (set<int>::iterator iter = totalItem.begin(); iter != totalItem.end(); iter++) {
        allItem.push_back(*iter);
    }
    return allItem;
}

/*********************************************************************
* get ID of transaction
**********************************************************************/
int Database::GetId(int i) const {
  return database[i].id;
}

/**********************************************************************
*  get max_item
***********************************************************************/
int Database::GetMaxItem() const {
  return max_item;
}

/***********************************************************************
* get min_item
************************************************************************/
int Database::GetMinItem() const {
  return min_item;
}


/*************************************************************************
* get transaction
**************************************************************************/
Transaction Database::GetTransaction(int i) const {
  return database[i];
}

/************************************************************************
* get the number of transactions in database
*************************************************************************/
int Database::GetNumberOfTransaction() const {
  return (int)database.size();
}

/************************************************************************
* print database
*************************************************************************/
void Database::Print(ostream &out) {
  out << "ID itemsets" << endl;
  for (int i = 0; i < (int)database.size(); i++) {
  Transaction &transaction = database[i];
  vector<int> &itemsets = transaction.itemsets;
  out << transaction.id << " ";
  for (int j = 0; j < (int)itemsets.size(); j++) {
  out << itemsets[j] << " ";
  }
  out << endl;
  }
  out << "max:" << max_item << " " << "min:" << min_item << endl;
}

/**************************************************************************
* normalize labels
* \sum_i y_i = 0
***************************************************************************/
void Database::NormalizeLabels() {
  double sum = 0;
  for (int i = 0; i < (int) database.size(); i++) {
    sum += database[i].label;
  }

  for (int i = 0; i < (int) database.size(); i++) {
    database[i].label = database[i].label - (double)sum/(double)(database.size());
  }
}

/**************************************************************************
 * MakeBitmap
 **************************************************************************/
void Database::MakeBitmap() {
  bitmap.resize(database.size());

  for (int i = 0; i < (int)database.size(); i++) {
    const Transaction &transaction = database[i];
    const vector<int> &itemset = transaction.itemsets;
    bitmap[i].resize(max_item - min_item + 1, 0);
    for (int j = 0; j < (int)itemset.size(); j++) {
      bitmap[i][itemset[j]-min_item] = 1;
    }
  }
}

/**************************************************************************
 * MakeBitmap
 **************************************************************************/
void Database::PrintBitmap(ostream &out) {
  for (int i = 0; i < (int)bitmap.size(); i++) {
    for (int j = 0; j < (int)bitmap[i].size(); j++) {
      out << bitmap[i][j] << " ";
    }
    out << endl;
  }
}

/**************************************************************************
 * GetBitmap
 **************************************************************************/
unsigned short Database::GetBitmap(int i, int item) {
  return bitmap[i][item];
}

////////////////for Gene Visualization///////////////////////////////
/****************************************************************
* read file from filename
*****************************************************************/
void Database::loadGeneData(vector<string> &GeneData) {
  string line;
  for(int i = 0; i < 5; i++){
     //vector<double> temp;
     vector<string> temp;
     logFC.push_back(temp);
  }
  //double logfc;
  string logfc;
  string gID;
  string gName;
  for(int i = 0;i < (int)GeneData.size();i++){
     ifstream is(GeneData[i].c_str());
     getline (is, line);
     while (getline (is, line)) {
        istrstream istrs ((char *)line.c_str());
        istrs >> gID;
        int index = -1;
        for(int j = 0; j < (int)geneID.size(); j++){
            if(gID == geneID[j]){
                index = j;
                break;
            }
        }
        if(index >= 0){
            istrs >> logfc;
            logFC[i][index] = logfc;
        }else{
            geneID.push_back(gID);
            istrs >> logfc;
            istrs >> gName;
            geneName.push_back(gName);
            for(int a = 0; a < 5; a++){
                logFC[a].push_back("-100");
            }
            logFC[i][(int)logFC[i].size()-1] = logfc;
        }
     }
  }
}
