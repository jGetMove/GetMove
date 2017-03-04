#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <strstream>
#include "../../include/Database.h"
#include "../../include/OccurenceDeriver.h"

/***************************************************************
* constructor
****************************************************************/
Database::Database() {}

/***************************************************************
* destructor
****************************************************************/
Database::~Database() {}

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
    while (getline(is, line)) {  // pour chaque object du fichier
        /*if(objectTemp == numObject){
           break;
        }*/
        // On reinitialise la transaction
        transaction.clear();
        bool empty = false;

        istrstream istrs((char *) line.c_str());
        while (istrs >> item) { // pour chaque cluster d'un object
            transaction.itemsets.push_back(item); // on ajoute le numero du cluster dans l'itemset de la transaction
            empty = true; // l'objet à au moins un cluster
        }

        if (!empty) { // si l'object n'as pas de cluster alors il prends un cluster nommé 7777777
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
    numObject = objectTemp; // numobject prends le total du nombre d'objets
    //  cout << "\t Object temps = " << objectTemp << endl;
    FindMaxAndMinItem(); // Associe à max_item et min_item le cluster le plus haut et le plus bas.
}

/*******************************************************************
* find max and min item from database
********************************************************************/
void Database::FindMaxAndMinItem() {
    max_item = 0;
    min_item = 100000000;
    for (int i = 0; i < (int) database.size(); i++) { // pour chaque transaction
        Transaction &transaction = database[i];
        vector<int> &itemsets = transaction.itemsets; // pour chauque itemset de la transaction
        for (int j = 0; j < (int) itemsets.size(); j++) {// cherche le numero du cluster le plus éleve ou le plus bas
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

    for (int i = 0; i < (int) database.size(); i++) {
        Transaction &transaction = database[i];
        vector<int> &itemsets = transaction.itemsets;

        for (int j = 0; j < (int) itemsets.size(); j++) {
            totalItem.insert(itemsets[j]);
        }
    }
    vector<int> allItem;

    for (set<int>::iterator iter = totalItem.begin(); iter != totalItem.end(); iter++) {
        allItem.push_back(*iter);
    }
    return allItem;
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
    return (int) database.size();
}

////////////////for Gene Visualization///////////////////////////////
