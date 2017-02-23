#ifndef _LCM_H_
#define _LCM_H_

#include <iostream>
#include <set>
#include <algorithm>
#include <cassert>
#include "Database.h"
#include "OccurenceDeriver.h"
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <math.h>
#include "Config.h"

using namespace std;


class Lcm {
  ostream &out;
public:

  Config cfg;
//
//    bool DEBUG;
//    bool FULLDEBUG;

  int min_sup;     // minimum support
  int maxItem;     // maximum item
  int minItem;     // minimum item
  int max_pat;     // maximum size of itemsets
  int min_t;       // minimum size of itemsets
  double min_w;    // minimum weight of group pattern
  bool bitmapFlag; // bit bitmapFlag

  int num_t;

  int num_convoy;
  int num_itemset;
  int num_GPattern;
  int num_fCS;
  int id;
  int numObjects;
  int numTimes;

  int numStrongGap;
  int numMediumGap;
  int numWeakGap;

//  int nbpassage;
    vector<vector<int> > occ_temp;
    bool printed;
    int id_iter;

  int num_swarm;

  fstream myfile;//spatio-temporal patterns results file

  //fstream FCI_binaryFile;//FCI binary file for Explicit combination experiments
  //vector<vector<unsigned long int> > FCIpartitions;

  vector<vector<int> > itemDecoding; //for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)

  vector<int> totalItem;

  ///modified code with item time index
  vector<int> item_temp;
  vector<int> timeindex;
  Lcm(ostream &out, int _min_sup, int _max_pat, int _min_t);
  ~Lcm();
  void RunLcmNew(Database &database, vector<Transaction> &transactionsets, int &numItems, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);
  void RunLcm(Database &database);
  void RunLcm(OccurenceDeriver &fullOcc, Database &database, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);//modified to put detect the over object in pattern detect
  void LcmIterNew(Database &database, vector<int> &itemsets, vector<int> &transactionList, OccurenceDeriver &occ, vector<int> &freqList, vector<Transaction> &transactionsets, int &numItems, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);
  void LcmIter(OccurenceDeriver &fullOcc, Database &database, vector<int> &itemsets, vector<int> &transactionList, OccurenceDeriver &occ, vector<int> &freqList, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);
  void PrintItemsetsNew(const vector<int> &itemsets, const OccurenceDeriver &occ, vector<Transaction> &transactionsets, int &numItems, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);
  void PrintItemsets(OccurenceDeriver &fullOcc, const vector<int> &itemsets, const OccurenceDeriver &occ, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID);
  void PrintItemsets(const vector<int> &itemsets, const OccurenceDeriver &occ);
  inline int CalcurateCoreI(const Database &database,const vector<int> &itemsets, const vector<int> &freqList);
  bool PpcTest(const Database &database, vector<int> &itemsets, vector<int> &transactionList, int item, vector<int> &newTransactionList);
  void MakeClosure(const Database &database, vector<int> &transactionList, vector<int> &q_sets, vector<int> &itemsets, int item);
  bool CheckItemInclusion(const Database &database, vector<int> &transactionList, int item);
  vector<int> CalcTransactionList(const Database &database, const vector<int> &transactionList, int item);
  void CalcTransactionList(const Database &database, const vector<int> &transactionList, int item, vector<int> &newTransactionList);
  void UpdateTransactionList(const Database &database, const vector<int> &transactionList, const vector<int> &q_sets, int item, vector<int> &newTransactionList);
  void UpdateFreqList(const Database &database, const vector<int> &transactionList, const vector<int> &gsub, vector<int> &freqList, int freq, vector<int> &newFreq);
  void UpdateOccurenceDeriver(const Database &database, const vector<int> &transactionList, OccurenceDeriver &occurence);
  void ReadItemTindexFile(const string &filename);
  void ConvoyDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets);
  void GroupPatternDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets);
  void rgPatternDetect(OccurenceDeriver &fullOcc);
  void fCloseSwarmDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets);
  void CloseSwarmDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets);
  int isIncluded(vector<int> &objectset,vector<int> &objectset2);
  double Degree(int &gap, int type);

  void ShowItemsetsInfos(vector<int> &itemsets, OccurenceDeriver &occ);
      void GenerateItemsets(Database &database,vector<int> &itemsets, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &generatedItemsets, vector<vector<int> > &generatedtimeID, vector<vector<int> > &generateditemID, int &sizeGenerated);

};
#endif // _LCM_H_
