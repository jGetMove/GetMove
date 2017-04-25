#include "../../include/Lcm.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <math.h>


/************************************************************************
 * Constructor
 ************************************************************************/
Lcm::Lcm(ostream &_out, int _min_sup, int _max_pat, int _min_t)
    : out(_out), min_sup(_min_sup), max_pat(_max_pat), min_t(_min_t)
{

    if (cfg.DEBUG)
    {
        cerr << "construction d'un objet avec comme parametre cerr et minsup =" << _min_sup<<endl;
    }
    printed = false;
}

/************************************************************************
 * Destructor
 ************************************************************************/
Lcm::~Lcm() {}

/*************************************************************************
 * run lcm for incremental and parallel
 *************************************************************************/
void Lcm::RunLcmNew(Database &database, vector<Transaction> &transactionsets, int &numItems, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{


    if(cfg.DEBUG)
    {
        cerr<<"//////////////RunLcmNew///////////"<<endl;

    }


    OccurenceDeriver occ(database);
    maxItem = database.GetMaxItem();
    minItem = database.GetMinItem();

    if(cfg.DEBUG)
    {
        cerr << "\t  item max : " << maxItem << endl;
        cerr << "\t item min : " << minItem << endl;
        cerr << "\t  nb de transactions : " << database.GetNumberOfTransaction() << endl;
        cerr << "\t transactionlist : ";
    }


    vector<int> itemsets;
    vector<int> transactionList;

    for (int i = 0; i < database.GetNumberOfTransaction(); i++)
    {
        transactionList.push_back(i);

        if(cfg.DEBUG)
        {
            cerr << i <<" ";
        }
    }

    if(cfg.DEBUG)
    {
        cerr << endl;
        cerr<<"\t transactionsets size : "<< transactionsets.size() <<  endl;
        for (unsigned int i = 0; i < transactionsets.size(); i++)
        {
            cerr<<"\t transactionsets[" << i << "].itemsets.size ()" <<transactionsets[i].itemsets.size() << endl;
            for (unsigned int j = 0; j < transactionsets[i].itemsets.size(); j++)
            {
                cerr << transactionsets[i].itemsets[j] <<" ";
            }
        }
        cerr << endl;
    }


    totalItem = database.GetItemset(); // ensemble des clustersId utilisés dans database

    if(cfg.DEBUG)
    {
        cerr << "\t taille des itemsets " << totalItem.size() << endl;
        cerr <<"\t  la on lance LCMIterNew : "  << endl;
    }

    vector<int> freqList;

    LcmIterNew(database, itemsets, transactionList, occ, freqList, transactionsets, numItems, itemID, timeID, level2ItemID, level2TimeID);

    fstream t;
    string f = "test.dat";
    t.open(f.c_str(), ios::out);

    if(cfg.DEBUG)
    {
        occ.Print(t);
        cerr << "itemsets RunLcmNew size : "<< itemsets.size()<<endl;
        ShowItemsetsInfos(itemsets,occ);
        cerr<<"//////////////end RunLcmNew///////////"<<endl;
    }
}

/*************************************************************************
 * run lcm directly to timeID incremental and parallel
 *************************************************************************/
void Lcm::RunLcm(OccurenceDeriver &fullOcc, Database &database, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{

    if(cfg.DEBUG)
    {
        cerr<<"//////////////RunLcm///////////"<<endl;
    }

    OccurenceDeriver occ(database);
    maxItem = database.GetMaxItem();
    minItem = database.GetMinItem();

    if(cfg.DEBUG)
    {
        cerr <<"\t  item max : " << maxItem << endl;
        cerr << "\t item min : " << minItem << endl;
        cerr <<"\t  nb de transactions : " << database.GetNumberOfTransaction() << endl;
    }

    vector<int> itemsets;
    vector<int> transactionList;
    for (int i = 0; i < database.GetNumberOfTransaction(); i++)
    {
        transactionList.push_back(i);
    }

    totalItem = database.GetItemset();// retourne l'ensemble des items dans toutes les transaction de database
    vector<int> freqList;

    if(cfg.DEBUG)
    {
        cerr << "\t taille des itemsets " << totalItem.size() << endl;
        for (unsigned int j=0; j < freqList.size(); j++)
        {
            cerr << freqList[j];
        }
    }

    LcmIter(fullOcc, database, itemsets, transactionList, occ, freqList, level2ItemID, level2TimeID);
    rgPatternDetect(fullOcc);

    //    myfile<<"//////////////end RunLcm///////////"<<endl;
}



//void Lcm::InitListItemsets(vector<vector<int> > &generatedItemsets,vector<int> &itemsets, int begin, int end){
//
//}
void Lcm::GenerateItemsets(Database &database,vector<int> &itemsets, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &generatedItemsets, vector<vector<int> > &generatedtimeID, vector<vector<int> > &generateditemID, int &sizeGenerated) {

    // GenerateItemsets is defined for managing the multi-occurrences of an object in different clusters
    // From itemsets and according to the same time for a cluster it will generate a set of itemsets corresponding
    // to the real occurrence, i.e. without generating an itemset at the same time
    // ex :      T1           T2
    //        C1     C2 !  C3   C4
    //    O    1      1 !   1
    // will generate the two following itemsets (C1, C3) and (C2, C3)
    // Furthemore there is a verification with the transactions already available in the DB in order to remove itemsets already
    // existing. Note this is because there are some issues to find the right number of objects for the new generated itemsets since
    // they are computed at the beginning.
    // The issue to test if some new itemsets are generated in the process but not initially is not taken into account

    if (cfg.DEBUG) {
        cerr << "GenerateItemsets method" << endl;
    }

    // special case where there is no itemset the vector is initialisated with the empty itemset
    if (itemsets.size() == 0){
        sizeGenerated=0;
        generatedItemsets.push_back(itemsets);
        return;
    }

    // ListofDates will provide the timestamp of each itemset
    // NumberSame time: number of time that there are same timestamps
    // lasttime stands for the last time of the itemset
    vector<int> listofDates;
    int numberSameTime=0;
    int lastTime;
    for (unsigned int i = 0; i < itemsets.size(); i++) {
        listofDates.push_back(timeID[itemsets[i]]);
            if (i != itemsets.size()-1){
                 if (timeID[itemsets[i]]==timeID[itemsets[i+1]]){
                    numberSameTime++;
                }
            }
        lastTime=timeID[itemsets[i]];
    }

    generatedItemsets.clear();

    // General case where an object does not belong to multi-clusters
    // Basically we only have to return this itemset meaning that generatedItemset has only one element
    if (numberSameTime ==0) {
        if (cfg.DEBUG) {
            cerr << "Here an object does not belong to multi-clusters. It should have only one element" << endl;
            //for (unsigned int v = 0; v < generatedItemsets[0].size(); v++)
              //  cerr << generatedItemsets[0][v] << " - " << generatedtimeID[0][v] << endl;
        }
        if (cfg.DEBUG) {
            cerr << "generatedItemsets push_back itemsets" << endl;
        }
        generatedItemsets.push_back(itemsets);
        if (cfg.DEBUG) {
            cerr << "generatedtimeID push_back listofDates" << endl;
        }
        generatedtimeID.push_back(listofDates);
        if (cfg.DEBUG) {
            cerr << "generateditemID push_back itemID" << endl;
        }
        generateditemID.push_back(itemID);
            if (cfg.DEBUG) {
                cerr << "No multi-clusters: value of generatedItemsets (1 itemset)" << endl;
               for (unsigned int v = 0; v < generatedItemsets[0].size(); v++)
                   cerr << generatedItemsets[0][v] << " - " << generatedtimeID[0][v] << endl;
            }
        return;
        }

    // Part for managing multi-cluster occurrences for objects
    // PosDates will provide for each timestamp (starting at 0), the values of the corresponding item(s)
    // in the example: PosDates[0] = {C1, C2} ; ListofDates[1] = {C3}
    if (cfg.DEBUG) {
        cerr << "Here we manage multi-clusters" << endl;
    }
    vector<vector<int> > PosDates;
        for (int l =1; l <= lastTime; l++){
            vector<int> row;
            for (unsigned int i = 0; i < itemsets.size(); i++)
           {
               if (timeID[itemsets[i]]==l) {
                   row.push_back(itemsets[i]);
               }
           }
            PosDates.push_back(row);
        }

    if (cfg.DEBUG){
        cerr << "Content of PosDate" << endl;
        for (int l=0; l < lastTime;l++)
        { cerr << "PosDate size =  " << PosDates[l].size() << endl;
            for (unsigned int j=0; j < PosDates[l].size(); j++)
                cerr << PosDates[l][j] << " ";
            cerr << endl;
        }
    }
    // sizeGenerated stands for the number of potential itemsets to generate
    sizeGenerated=1;
    for (unsigned int k = 0; k < PosDates.size(); k++) {
        sizeGenerated*=PosDates[k].size();
    }



   // initialise the set of generated itemsets
    for (unsigned int itemsetIndex = 0; itemsetIndex < PosDates.size(); ++itemsetIndex )
    {
        vector<int> itemset = PosDates[itemsetIndex];

        if ( itemsetIndex == 0 )
        {
            for( vector<int>::iterator it=itemset.begin(); it!=itemset.end(); ++it )
            {
                vector<int> singleton (1, *it);
                generatedItemsets.push_back(singleton);
            }
        }
        else
        {
            vector<vector<int> > new_results ;

            for( vector<vector<int> >::iterator it=generatedItemsets.begin(); it!=generatedItemsets.end(); ++it )
            {
                vector<int> result = *it;
                for( vector<int>::iterator itItem=itemset.begin(); itItem!=itemset.end(); ++itItem )
                {
                    int item = *itItem;
                    vector<int> new_result (result);
                    new_result.push_back(item);
                    new_results.push_back(new_result);
                }
            }
            generatedItemsets = new_results;
        }
    }

    // Remove the itemsets already existing in the set of transactions
    int nb=database.GetNumberOfTransaction();
    vector <int> tempo;
    vector<vector<int> > CheckedItemsets;
    vector <int> currentItemsets;
    int nbitemsets = 0;
    bool insertok;
    for (unsigned int u = 0; u < generatedItemsets.size(); u++){
        insertok=true;
        currentItemsets.clear();
        currentItemsets=generatedItemsets[u];
        for (int i=0; i < nb ; i++){
            tempo=(database.GetTransaction(i).itemsets);
            if (tempo==generatedItemsets[u]) {
                insertok=false; break;
            }
        }
        if (insertok) {
            CheckedItemsets.push_back(currentItemsets);
            nbitemsets++;
        }
    }
    sizeGenerated=nbitemsets;
    if (cfg.DEBUG){
        cerr << "nb of generated itemsets " <<nbitemsets << endl;
        for (unsigned int u = 0; u < CheckedItemsets.size(); u++){
            for (unsigned int k=0; k < CheckedItemsets[u].size(); k++){
                cerr << CheckedItemsets[u][k] << " - " ;
            }
            cerr << endl;
        }
    }

    generatedItemsets.clear();
    generatedItemsets=CheckedItemsets;


    // updating list of dates
    listofDates.clear();
    for (int l=0; l < sizeGenerated;l++) {
        for (unsigned int u=0; u < generatedItemsets[l].size(); u++){
                for (unsigned int i = 0; i < itemsets.size(); i++){
                    if (generatedItemsets[l][u]==itemsets[i]){
                        listofDates.push_back(timeID[itemsets[i]]);
                    }

                }
        }
    }

    generatedtimeID.push_back(listofDates);
    generateditemID.push_back(itemID);
}
/*************************************************************************
 * main function for incremental and parallel
 *************************************************************************/
void Lcm::LcmIterNew(Database &database, vector<int> &itemsets, vector<int> &transactionList, OccurenceDeriver &occ, vector<int> &freqList, vector<Transaction> &transactionsets, int &numItems, vector<int> &itemID, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{
    if (cfg.DEBUG) cerr<<" LcmIterNew method"<<endl;

    vector<vector<int> > generatedItemsets;
    vector<vector<int> > generatedtimeID;
    vector<vector<int> > generateditemID;
    int sizeGenerated=1;
    GenerateItemsets(database,itemsets, itemID, timeID, generatedItemsets, generatedtimeID, generateditemID, sizeGenerated); // met itemsets (tableau vide) dans generatedItemsets


    for (unsigned int nbitemset=0; nbitemset < generatedItemsets.size(); nbitemset++){
        vector<int> timeIDTemp;
        timeIDTemp.push_back(id); // ajout dans timeIdtemp l'id du lcm
        vector<int> itemIDTemp;
        itemIDTemp.push_back(id); // ajout dans timeIdtemp l'id du lcm
        vector<int> itemsetsTemp;//for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)
        //Compress item and time into level 2
        if(cfg.DEBUG) ShowItemsetsInfos(generatedItemsets[nbitemset], occ); // affiche itemsets (qui est toujours vide)
        PrintItemsetsNew(generatedItemsets[nbitemset], occ, transactionsets, numItems, timeID, level2ItemID, level2TimeID); // print $ itemset(qui est toujours vide)

        if (cfg.DEBUG){
            cerr << "content of generatedItemset" << endl; // generatedItemsets[nbitemset].size() = 0 car itemsets est vide
            for (unsigned int i=0 ; i < generatedItemsets[nbitemset].size(); i++){
                       cerr << generatedItemsets[nbitemset][i] << " " << occ.GetNumOcc(generatedItemsets[nbitemset][i]) << endl;
            }
        }


        int core_i=CalcurateCoreI(database,generatedItemsets[nbitemset], freqList); // ne fait rien


      //Compute the frequency of each pattern P \cup {i}, i > core_i(P)
      // by Occurence deliver with P and Occ;

        vector<int>::iterator iter = lower_bound(totalItem.begin(), totalItem.end(), core_i); // met l'iterateur à 0

        vector<int> freq_i;

        for (int i = *iter; iter != totalItem.end(); iter++, i = *iter){ // i = clusterId
            if ((int)occ.table[i].size() >= min_sup && binary_search(generatedItemsets[nbitemset].begin(), generatedItemsets[nbitemset].end(), i) == false)
                freq_i.push_back(i);
        }
        // freq_i est equivalent à totalItem

        vector<int> newTransactionList;
        vector<int> q_sets;
        vector<int> newFreqList;

        for (vector<int>::iterator freq = freq_i.begin(); freq != freq_i.end(); freq++){ // pour chauqe ClusterID
            newTransactionList.clear();

            if (PpcTest(database, generatedItemsets[nbitemset], transactionList, *freq, newTransactionList)){
              // newTransactionList conteint mainteannt l'ensemble des transaction
              //return false quand l'itemset ne contient pas freq (l'item actuel) ET que newTransactionList contienne l'item actuel
                q_sets.clear();

                MakeClosure(database, newTransactionList, q_sets, generatedItemsets[nbitemset], *freq);

                if (max_pat == 0 || (int)q_sets.size() <= max_pat){
                    newTransactionList.clear();
                    UpdateTransactionList(database, transactionList, q_sets, *freq, newTransactionList);
                    UpdateOccurenceDeriver(database, newTransactionList, occ);
                    newFreqList.clear();
                    UpdateFreqList(database, transactionList, q_sets, freqList, *freq, newFreqList);

                    LcmIterNew(database, q_sets, newTransactionList, occ, newFreqList, transactionsets, numItems, itemID, timeID, level2ItemID, level2TimeID);
                }
            }
        }
    }// nb of itemsets in generatedItemsets

}


void Lcm::ShowItemsetsInfos(vector<int> &itemsets, OccurenceDeriver &occ)
{

    for (unsigned int i = 0; i < itemsets.size(); i++)
    {
        cerr << "------------"<<endl;
        for (unsigned int i = 0; i < itemsets.size(); i++)
        {
            vector<int> table = occ.GetTable(itemsets[i]);
            cerr<< " itemset : " <<itemsets[i]<<" objects : ";
            for (unsigned int k = 0; k < table.size(); k++)
            {
                cerr << table[k] << " ";
            }
            cerr <<endl;
        }
        cerr <<endl;

    }
}

/*************************************************************************
 * main function
 *************************************************************************/
void Lcm::LcmIter(OccurenceDeriver &fullOcc, Database &database, vector<int> &itemsets, vector<int> &transactionList, OccurenceDeriver &occ, vector<int> &freqList, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{

    if(cfg.DEBUG)
    {
        cerr<<"//////////////LcmIter///////////"<<endl;
        cerr << endl << endl;
    }

    int core_i = CalcurateCoreI(database,itemsets, freqList);

    if(cfg.DEBUG)
    {
        cerr << "taille itemset apres coreI" << itemsets.size() << " freqList.size() << " << freqList.size()<< endl;
    }


    //compute blocks of itemsets.
    vector<int> blocks;


    for(unsigned int i = 0; i < itemsets.size(); i++)
    {
        blocks.push_back((level2TimeID[itemsets[i]])[0]);
    }


    /*
      Compute the frequency of each pattern P \cup {i}, i > core_i(P)
      by Occurence deliver with P and Occ;
    */
    vector<int>::iterator iter = lower_bound(totalItem.begin(), totalItem.end(), core_i);
    vector<int> freq_i;
    for (int i = *iter; iter != totalItem.end(); iter++, i = *iter)
    {
        if ((int)occ.table[i].size() >= min_sup && binary_search(itemsets.begin(), itemsets.end(), i) == false)
            freq_i.push_back(i);

    }



    vector<int> newTransactionList;
    vector<int> q_sets;
    vector<int> newFreqList;
    int nbpassage=0;

    for (vector<int>::iterator freq = freq_i.begin(); freq != freq_i.end(); freq++)
    {
        if(cfg.DEBUG)
        {
            cerr << "###########################APPEL ITERATEUR = " << ++nbpassage << endl;
        }
        newTransactionList.clear();

        int blockOfItem = (level2TimeID[*freq])[0];

        if(cfg.DEBUG)
        {
            cerr << "BlockOfItem " << blockOfItem << endl;

            cerr<<"LcmIter original transactionList : ";
            for (unsigned int i = 0; i != transactionList.size(); i++)
            {
                cerr<<transactionList[i]<<" ";

            }
            cerr<<endl;

        }

        if (PpcTest(database, itemsets, transactionList, *freq, newTransactionList) && binary_search(blocks.begin(), blocks.end(), blockOfItem) == false /*&& lengthItemsets > min_t*/)
        {
            q_sets.clear();
            if(cfg.DEBUG)
            {
                cerr<<"LcmIter new transactionList : ";
                for (unsigned int i = 0; i != newTransactionList.size(); i++)
                {
                    cerr<<newTransactionList[i]<<" ";

                }
                cerr<<endl;
            }

            MakeClosure(database, newTransactionList, q_sets, itemsets, *freq);



            if ((max_pat == 0 || (int)q_sets.size() <= max_pat) /*&& lengthItemsets > min_t*/)
                //also detect min_t
            {
                if(cfg.DEBUG)
                {
                    cerr << "****************Attention ici appel recursif de LCMIter" << endl;
                }
                newTransactionList.clear();
                UpdateTransactionList(database, transactionList, q_sets, *freq, newTransactionList);

                if(cfg.DEBUG)
                {
                    occ.Print(cerr);
                    cerr << "****************Normalement ici il a mis a jour newTransactionList et appel update" << endl;
                    cerr << "****************Normalement ici il a mis a jour occ" << endl;
                    occ.Print(cerr);
                }

                newFreqList.clear();

                UpdateFreqList(database, transactionList, q_sets, freqList, *freq, newFreqList);
                LcmIter(fullOcc, database, q_sets, newTransactionList, occ, newFreqList, level2ItemID, level2TimeID);
            }
        }
    }

    if(cfg.DEBUG)
    {
        cerr << "CONTENU DE LEVEL2ITEMID FIN LCMITER" << endl;
        for (unsigned int y=0; y < level2ItemID.size(); y++)
        {
            for (unsigned int z=0; z<level2ItemID[y].size(); z++)
                cerr << level2ItemID[y][z];
            cerr << endl;
        }

        cerr << "CONTENU DE LEVEL2TIMEID FIN LCMITER" << endl;
        for (unsigned int y=0; y < level2TimeID.size(); y++)
        {
            for (unsigned int z=0; z<level2TimeID[y].size(); z++)
                cerr << level2TimeID[y][z];
            cerr << endl;
        }


        cerr << "A la fin de LCMITER occ vaut - il devrait etre comme avant "<< endl;
        occ.Print(cerr);
    }

    ///* AVANT
    if (printed == false)
    {
        PrintItemsets(fullOcc, itemsets, occ, level2ItemID, level2TimeID); // print frequent itemset
        printed = true;

    }

    if(cfg.DEBUG)
    {
        cerr<<"//////////////end LcmIter///////////"<<endl;
    }
}

/*************************************************************************
 * Update Freq List
 *************************************************************************/
inline void Lcm::UpdateFreqList(const Database &database, const vector<int> &transactionList, const vector<int> &gsub, vector<int> &freqList, int freq, vector<int> &newFreq)
{
    int iter = 0;
    if (freqList.size() > 0)
    {
        for (; iter < (int)gsub.size(); iter++)
        {

            if(cfg.DEBUG)
            {
                cerr<<"*********gsub : "<<gsub[iter]<<endl;
            }
            if (gsub[iter] >= freq) break;
            newFreq.push_back(freqList[iter]);
            if(cfg.DEBUG)
            {
                cerr<<"**********freqList : "<<freqList[iter]<<endl;
            }
        }

    }

    vector<int> newList;
    for (unsigned int i = 0; i < transactionList.size(); i++)
    {
        newList.push_back(transactionList[i]);
    }


    vector<int> newnewList;
    for (int i = iter; i < (int)gsub.size(); i++)
    {
        int item = gsub[i];
        int freqCount = 0;

        for (unsigned int list = 0; list < newList.size(); list++)
        {
            const Transaction &transaction = database.database[newList[list]];
            if (binary_search(transaction.itemsets.begin(), transaction.itemsets.end(), item) == true)
            {
                freqCount += 1;
                newnewList.push_back(newList[list]);
            }
        }
        newFreq.push_back(freqCount);
        newList = newnewList;
        newnewList.clear();
    }
}

/*************************************************************************
 * Update Transaction List
 *************************************************************************/
inline void Lcm::UpdateTransactionList(const Database &database, const vector<int> &transactionList, const vector<int> &q_sets, int item, vector<int> &newTransactionList)
{

    for (int i = 0; i < (int)transactionList.size(); i++)
    {

        const Transaction &transaction = database.database[transactionList[i]];
        int iter;
        for (iter = 0; iter < (int)q_sets.size(); iter++)
        {
            int q = q_sets[iter];
            if (q >= item)
            {
                if (binary_search(transaction.itemsets.begin(), transaction.itemsets.end(), q) == false)
                    break;
            }
        }
        if (iter == (int)q_sets.size())
            newTransactionList.push_back(transactionList[i]);
    }

}

/***************************************************************************
 * Print itemsets for incremental and Parallel
 ***************************************************************************/
inline void Lcm::PrintItemsetsNew(const vector<int> &itemsets, const OccurenceDeriver &occ, vector<Transaction> &transactionsets, int &numItems, vector<int> &timeID, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{


    if(cfg.DEBUG)
    {
        cerr<<"//////////////PrintItemsetsNew///////////"<<endl;
    }

    if ((int)itemsets.size() > min_t)   // only retrieve itemsets which have length is larger than min_t
    {

        if(itemsets[0]==7777777)
        {
            //do nothing;
        }
        else
        {

            if(cfg.DEBUG)
            {
                cerr<<"\t \t dans la boucle car itemsets.size() > min_t"<<endl;
            }


            //Compress item and time into level 2
            vector<int> timeIDTemp;
            timeIDTemp.push_back(id);
            vector<int> itemIDTemp;
            itemIDTemp.push_back(id);

            if(cfg.DEBUG)
            {
                cerr<<" \t **** (PrintItemsetsNew)itemset : " <<id<< "itemsets.size ()" << itemsets.size() <<endl;
                for (unsigned int k = 0; k < itemsets.size(); k++)
                {
                    cerr << itemsets[k] ;
                }
                cerr << endl;
            }

            //            cerr<<" timeID : " <<id<<endl;

            vector<int> itemsetsTemp;//for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)
            //Compress item and time into level 2
            for (unsigned int i = 0; i < itemsets.size(); i++)
            {
                itemsetsTemp.push_back(itemsets[i]);//for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)

                //Test index of itemID

                /////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////
                ///////////////////////////////////////////////////////////////////
                timeIDTemp.push_back(timeID[itemsets[i]]);

                if(cfg.DEBUG)
                {
                    cerr<<" itemset : " <<itemsets[i];
                    cerr<<" timeID : " <<timeID[itemsets[i]]<<endl;
                }
                //Test item index
                itemIDTemp.push_back(itemsets[i]);

            }
            // put time index of new item into time vector
            level2TimeID.push_back(timeIDTemp);
            // put item index of new item into time vector
            level2ItemID.push_back(itemIDTemp);
            // affichage pour voir si on a les bonnes infos

            //for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)
            itemDecoding.push_back(itemsetsTemp);

            const vector<int> &table = occ.GetTable(itemsets[itemsets.size() - 1]);

            if(cfg.DEBUG)
            {
                cerr << "\t table.size " << (int)table.size() << endl;
                cerr << "\t mise a jour tableau second niveau avec numItems = " << numItems << endl;
            }
            for (unsigned int i = 0; i < table.size(); i++)
            {
                //   out << table[i] << " ";
                //modified code - incremental parallel
                if(cfg.DEBUG)
                {
                    cerr << table[i] << " ";
                }
                transactionsets.at(table[i]).itemsets.push_back(numItems); //put new index of new item (second level) (not all the first level information)
                //////////////////////////
            }
            if(cfg.DEBUG)
            {
                cerr << endl;
            }
            numItems++;


        }

    }

    if(cfg.DEBUG)
    {
        cerr << "CONTENU DE LEVEL2ITEMID" << endl;
        for (unsigned int y=0; y < level2ItemID.size(); y++)
        {
            for (unsigned int z=0; z<level2ItemID[y].size(); z++)
                cerr << level2ItemID[y][z];
            cerr << endl;
        }

        cerr << "CONTENU DE LEVEL2TIMEID" << endl;
        for (unsigned int y=0; y < level2TimeID.size(); y++)
        {
            for (unsigned int z=0; z<level2TimeID[y].size(); z++)
                cerr << level2TimeID[y][z];
            cerr << endl;
        }

        cerr<<"//////////////end PrintItemsetsNew///////////"<<endl;
    }
}

/***************************************************************************
 * Print itemsets directly to timeID for incremental and parallel
 ***************************************************************************/
inline void Lcm::PrintItemsets(OccurenceDeriver &fullOcc, const vector<int> &itemsets, const OccurenceDeriver &occ, vector<vector<int> > &level2ItemID, vector<vector<int> > &level2TimeID)
{

    if(cfg.DEBUG)
    {
        cerr<<"//////////////PrintItemsets///////////"<<endl;
        cerr << "Pour verification contenu level2Item itemsets.size() = " << itemsets.size() << endl;
    }

    if ((int)itemsets.size() > 0)
    {
        //if ((int)itemsets.size() > min_t) { // only retrieve itemsets which have length is larger than min_t
        if(itemsets[0]==7777777)
        {
            //do nothing;
        }
        else
        {

            vector<int> timeBased;
            vector<int> itemBased;

            OccurenceDeriver occ3 = occ;

            if(cfg.DEBUG)
            {
                cerr << "verification du contenu de occ " << endl;
                occ3.Print(cerr);
            }


            vector<int> table = occ.GetTable(itemsets[itemsets.size() - 1]);
            vector<vector<int> > alltables = occ.GetAllTable();

            if(cfg.DEBUG)
            {
                cerr << "on met dans table le conenu de occ, la taille de table est table.size()" << table.size() << endl;



                cerr <<"all tables : "<<endl;
                for (unsigned int i = 0; i < alltables.size() ; i++)
                {
                    cerr <<"table "<<i<<" : ";
                    for (unsigned int j = 0; j < alltables[i].size(); j++)
                    {
                        cerr << alltables[i][j] << " ";

                    }
                    cerr<<endl;

                }
                cerr<<endl;
            }

            OccurenceDeriver occ2=occ;

            if(cfg.DEBUG)
            {

                occ2.Print(cerr);

                cerr << "Taille de alltables : " << alltables.size() << endl;
                cerr << "Taille de table " << table.size() << endl;
                cerr <<"table ["<<itemsets[itemsets.size() - 1]<<"] ";
                for (unsigned int i = 0; i < table.size(); i++)
                {
                  cerr << table[i] << " ";

                }
                cerr<<endl;
                cerr << "CONTENU DE LEVEL2ITEMID" << endl;
                for (unsigned int y=0; y < level2ItemID.size(); y++) {
                    for (unsigned int z=0; z<level2ItemID[y].size(); z++)
                        cerr << level2ItemID[y][z];
                    cerr << endl;
                }

                cerr << "CONTENU DE LEVEL2TIMEID" << endl;
                for (unsigned int y=0; y < level2TimeID.size(); y++) {
                    for (unsigned int z=0; z<level2TimeID[y].size(); z++)
                        cerr << level2TimeID[y][z];
                    cerr << endl;
                }
                cerr << "****************************************************Taille de alltables.size () " << alltables.size() << endl;
            }
            //// ON ENLEVE LA BOUCLE POUR VOIR
            for (unsigned int x = 0; x < alltables.size(); x++)
            {

                if(cfg.DEBUG)
                {
                    cerr << "x : " << x << " table [" << x<< "] : " ;

                    for (int u=0; u < (int)alltables[x].size(); u++)
                    {
                        cerr << alltables[x][u];
                    }
                    cerr << endl;
                    cerr << "taille itemset " << level2TimeID[x].size()-1 << "  "  <<endl;

                }

                table = occ.GetTable(x);

                timeBased.clear();
                itemBased.clear();

                if(cfg.DEBUG)
                {
                    cerr<<"level2time : ";
                }
                for(unsigned int j = 1; j < level2TimeID[x].size(); j++)
                {
                    if(cfg.DEBUG)
                    {
                        cerr<<level2TimeID[x][j]<<" ";
                    }
                    timeBased.push_back((level2TimeID[x])[j]);
                }
                if(cfg.DEBUG)
                {
                    cerr<<endl;
                }


                for(unsigned int j = 1; j < level2ItemID[x].size(); j++)
                {
                    if(cfg.DEBUG)
                    {
                        cerr<<level2ItemID[x][j]<<" ";
                    }
                    itemBased.push_back((level2ItemID[x])[j]);
                }
                if(cfg.DEBUG)
                {
                    cerr<<endl;
                }


                ////print closed swarm///////

                vector<int> Litemsets=level2ItemID[x];

                if((int)timeBased.size() >= min_t)
                {

                    cout << "------------"<<endl;
                    myfile << "newitemset" << endl;
                    myfile << "------------"<<endl;
                    cout<< "itemset: ";
                    myfile<< "itemset: ";
                    for(unsigned int j = 0; j < itemBased.size(); j++)
                    {

                        cout<<itemBased[j]<<" ";
                        myfile<<itemBased[j]<<" ";


                    }
                    //count num closed itemsets
                    num_itemset++;
                    //closed swarm

                    myfile <<endl<< "objects: ";
                    cout << "objects: ";
                    for ( int i = 0; i < (int)alltables[x].size(); i++)
                    {
                        cerr << alltables[x][i] << " ";
                        myfile << alltables[x][i] << " ";
                    }
                    myfile <<endl<< "Time: ";
                    cout << "Time: ";
                    for (unsigned int i = 0; i < timeBased.size(); i++)
                    {

                        myfile << timeBased[i] << " "; //normal mode
                        cout << timeBased[i] << " ";
                    }
                    myfile << endl;

                    cout << endl;


                    // ICI AVANT

                    myfile <<"Swarm"<< endl;
                    fCloseSwarmDetect(fullOcc, timeBased, itemBased, table);
                    CloseSwarmDetect(fullOcc, timeBased, itemBased, table);
                    myfile <<"-------------------"<< endl;
                    myfile <<"Convoy"<< endl;
                    ConvoyDetect(fullOcc, timeBased, itemBased, table);
                    myfile <<"-------------------"<< endl;
                    myfile <<"Group pattern"<< endl;
                    GroupPatternDetect(fullOcc, timeBased, itemBased, table);
                    myfile <<"-------------------"<< endl;
                    myfile << "end newitemset" << endl;

                } // if(timeBased.size() >= min_t)
            } // for (unsigned int x = 0; x < alltables.size(); x++)


        }


    }
    if(cfg.DEBUG)
    {
        cerr<<"//////////////end PrintItemsets///////////"<<endl;
    }

}

/*****************************************************************************
 * calculrate core_i
 *****************************************************************************/
inline int Lcm::CalcurateCoreI(const Database &database,const vector<int> &itemsets, const vector<int> &freqList)
{

    if(cfg.DEBUG)
    {
        cerr << "In CalcurateCoreI - managing itemsets: " << endl;
    }
    int nb=database.GetNumberOfTransaction();
    vector <int> tempo;

    for (int i=0; i < nb ; i++) // affiche si l'ensemble des itemsets contenus dans la database n'est pas itemset
    {

        tempo=(database.GetTransaction(i).itemsets);

        if(cfg.DEBUG)
        {
            if (tempo==itemsets)
            {
                cerr << "ATTENTION LA TRANSACTION EXISTE DEJA IL NE FAUT PAS L'INSERER A NOUVEAU" << endl;
            }
        }
    }
    //if (itemsets.size()>0) cerr<<itemsets[0] << endl;

    if(cfg.DEBUG)
    {
        for (unsigned int j=0; j < itemsets.size(); j++)
        {
            cerr << itemsets[j];
        }
        cerr << endl;
        for (unsigned int j=0; j < freqList.size(); j++)
        {
            cerr << freqList[j];
        }
        cerr << endl;
        cerr << "Itemsets.size " << itemsets.size() <<  endl;
        cerr << "freqList.size " << freqList.size() <<  endl;
    }

    if (itemsets.size() > 0)
    {
        int current = freqList[freqList.size() - 1];// car commence � z�ro

        if(cfg.DEBUG)
        {
            cerr << "current: " << current << "freqList.size " << freqList.size() << endl;
        }
        // for (unsigned int i = freqList.size()-2 ; i > 0; i--)
        for ( int i = freqList.size()-1  ; i >= 0; i--)
        {
            if(cfg.DEBUG)
            {
                cerr << "current " << current << "  i  " << i << "  freqList[" << i << "] = " <<freqList[i] << endl;
                cerr<<freqList[i] << endl;
                fflush(stdout);
            }

            if (current != freqList[i])
            {
                return freqList[i];
            }
        }

        if(cfg.DEBUG)
        {
            cerr << "on sort de la boucle" << endl;
            fflush(stdout);
        }

        return itemsets[0];
    }
    else
    {
        return 0;
    }

}


/**************************************************************************
 * Prefix Preseaving Test
 * Test whether p(i-1) is equal to q(i-1) or not
 **************************************************************************/
inline bool Lcm::PpcTest(const Database &database, vector<int> &itemsets, vector<int> &transactionList, int item, vector<int> &newTransactionList)
{
    // j_sets: set not including items which are included in itemsets.
    // make transactionList pointing to the indexes of database including P \cup item
    if(cfg.DEBUG)
    {
        cerr << "Test ppc : item " << item << endl;
    }
    CalcTransactionList(database, transactionList, item, newTransactionList); // mets dans newTransactionList l'ensemble des transactions contenant item (le custer id)

    // check j s.t j < i, j \notin P(i-1) is included in every transaction of T(P \cup {i})
    for (vector<int>::iterator j = totalItem.begin(); *j < item; j++)
    {
        if (binary_search(itemsets.begin(), itemsets.end(), *j) == false &&
                CheckItemInclusion(database, newTransactionList, *j) == true)
        {
            return false; // return false quand l'itemset ne contient pas l'item actuel ET que newTransactionList contienne l'item actuel
        }
    }
    return true;
}

/****************************************************************************
 * Make closure
 * make Q = Clo(P \cup {i}) subject to Q(i-1) = P(i-1)
 *****************************************************************************/
inline void Lcm::MakeClosure(const Database &database, vector<int> &transactionList, vector<int> &q_sets, vector<int> &itemsets, int item)
{
    // make transactionList pointting to the indexes of database inclding {P \cup item}
    // vector<int> &newTransactionList = CalcTransactionList(database, transactionList, item);
    // make Clo(P \cup {i})
    for (unsigned int i = 0; i < itemsets.size() && itemsets[i] < item; i++)
    {
        q_sets.push_back(itemsets[i]);
    }
    q_sets.push_back(item);

    vector<int>::iterator i = lower_bound(totalItem.begin(), totalItem.end(), item + 1);

    for (int iter = *i; i != totalItem.end(); i++, iter = *i)
    {
        if (CheckItemInclusion(database, transactionList, iter) == true)
        {
            q_sets.push_back(iter);
        }
    }
}

/********************************************************************************
 * CheckItemInclusion
 * Check whther item is included in the transactions pointed to transactionList
 ********************************************************************************/
inline bool Lcm::CheckItemInclusion(const Database &database, vector<int> &transactionList, int item)
{
    for (vector<int>::iterator iter = transactionList.begin(); iter != transactionList.end(); iter++)
    {
        const Transaction &transaction = database.database[*iter];
        if (binary_search(transaction.itemsets.begin(), transaction.itemsets.end(), item) == false) return false;
    }
    return true;
}


/*********************************************************************************
 *  Calcurate new transaction list
 *********************************************************************************/
inline void Lcm::CalcTransactionList(const Database &database, const vector<int> &transactionList, int item, vector<int> &newTransactionList)
{

    for (int list = 0; list < (int)transactionList.size(); list++)
    { // pour chaque transaction
        const Transaction &transaction = database.database[transactionList[list]]; // on récupère la transaction
        if (binary_search(transaction.itemsets.begin(), transaction.itemsets.end(), item) == true) // on verifie si l'id est dans l'itemset de la transaction
            newTransactionList.push_back(transactionList[list]); // is oui, on l'ajouter à la nouvelle transactionlist
    }
}

/***********************************************************************************
 * Update Occurence Deriver////////////////////////////////////////////////////////
 ***********************************************************************************/
inline void Lcm::UpdateOccurenceDeriver(const Database &database, const vector<int> &transactionList, OccurenceDeriver &occurence)
{
    occurence.Clear();
    for (int i = 0; i < (int)transactionList.size(); i++)
    {
        const Transaction &transaction = database.database[transactionList[i]];
        const vector<int> &itemsets = transaction.itemsets;
        for (int j = 0; j < (int)itemsets.size(); j++)
        {
            occurence.table[itemsets[j]].push_back(transactionList[i]);
        }
    }
}


/****************************************************************
* read item time index from filename
* mets dans timeindex[] le timeid et dans item_temp[] l'objectID
* numTimes = item_temp.size()
*****************************************************************/
void Lcm::ReadItemTindexFile(const string &filename)
{
    num_t=0;
    string line;
    int itemTemp;

    if(cfg.DEBUG)
    {
        cerr << "\t **** LECTURE FICHIER TIME_INDEX pour LCM ****\n" << filename <<endl;
    }
    ifstream is(filename.c_str());

    if(cfg.DEBUG)
    {
        cerr << "Time ID ----- Object ID"<<endl;
    }
    while (getline (is, line))
    {
        num_t++;

        istrstream istrs ((char *)line.c_str());
        istrs >> itemTemp;

        if(cfg.DEBUG)
        {
            cerr <<itemTemp << " ----- ";
        }
        timeindex.push_back(itemTemp);
        istrs >> itemTemp;
        if(cfg.DEBUG)
        {
            cerr << itemTemp << endl;
        }
        item_temp.push_back(itemTemp);
    }

    numTimes = item_temp.size();

}

/****************************************************************
* Detect convoy from time-based closed itemsets
*****************************************************************/
void Lcm::ConvoyDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets)
{


    cout <<"-----------------------------------------------"<<endl;
    cout <<"detection de convoi : "<<endl;
    int lastTime = -1;
    int firstTime = -1;
    int currentTime = -1;
    int currentIndex = -1;
    int lastIndex = -1;
    int firstIndex = -1;


    firstTime = timesets[0];
    firstIndex = 0;
    lastTime = firstTime;
    lastIndex = firstIndex;



    vector<int> correctObjectsets = fullOcc.GetTable(ItemSets[0]);
    vector<int> currentObjectsets = fullOcc.GetTable(ItemSets[0]);


    for(unsigned int i = 0; i < timesets.size(); i++)
    {


        currentObjectsets = fullOcc.GetTable(ItemSets[i]);


        currentTime = timesets[i];
        currentIndex = i;


        if(currentTime == (lastTime + 1))
        {
            vector<int> objectTemp;

            for(unsigned int j = 0; j < correctObjectsets.size(); j++)
            {
                if(binary_search(currentObjectsets.begin(), currentObjectsets.end(), correctObjectsets[j]) == true)
                {
                    objectTemp.push_back(correctObjectsets[j]);
                }
            }

            //objectTemp.push_back(currentObjectsets)

            correctObjectsets = objectTemp;
            lastTime = currentTime;
            lastIndex = currentIndex;

        }
        else if(currentTime > (lastTime + 1))
        {
            int temp1 = correctObjectsets.size();
            int temp2 = objectsets.size();

            myfile << "CV:" << endl;
            cout <<"CV:" << endl;
            if((lastTime - firstTime) >= min_t && (temp1 == temp2) )
            {

                myfile << "Object: ";
                cout <<"Object: ";

                for (unsigned int j = 0; j < correctObjectsets.size(); j++)
                {
                    myfile << correctObjectsets[j] << " ";

                    cout <<correctObjectsets[j] << " ";
                }
                cout << endl;
                myfile << endl;
                myfile << "Time: ";
                cout <<"Time: ";
                for(int j = firstIndex; j <= lastIndex; j++)
                {
                    myfile << timesets[j] << " ";
                    cout <<timesets[j] << " ";
                }
                myfile << endl;
                cout << endl;

                firstTime = currentTime;
                firstIndex = currentIndex;
                lastTime = currentTime;
                lastIndex = currentIndex;
                ////count num convoys
                num_convoy++;

            }
            else
            {
                firstTime = currentTime;
                firstIndex = currentIndex;
                lastTime = currentTime;
                lastIndex = currentIndex;
                correctObjectsets = currentObjectsets;
            }
        }
    }

    myfile << "CV:" << endl;
    cout <<"CV:" << endl;
    if((lastTime - firstTime) >= min_t)
    {
        myfile << "objects: ";
        cout <<"objects: ";

        for (unsigned int j = 0; j < correctObjectsets.size(); j++)
        {
            myfile << correctObjectsets[j] << " ";

            cout <<correctObjectsets[j] << " ";
        }
        cout << endl;
        myfile << endl;

        myfile << "Time: ";
        cout <<"Time: ";
        for(int j = firstIndex; j <= lastIndex; j++)
        {
            myfile << timesets[j] << " ";
            cout <<timesets[j] << " ";
        }
        myfile << endl;
        cout << endl;

        ////count num convoys
        num_convoy++;
    }

    cout <<"-----------------------------------------------"<<endl;
}

/****************************************************************
* Detect group patterns from time-based closed itemsets
*****************************************************************/
void Lcm::GroupPatternDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets)
{

    cout <<"detection de group pattern : "<<endl;
    int lastTime = -1;
    int firstTime = -1;
    int currentTime = -1;
    int currentIndex = -1;
    int lastIndex = -1;
    int firstIndex = -1;

    vector<int> startConvoy;
    vector<int> endConvoy;
    vector<int> startConvoyIndex;
    vector<int> endConvoyIndex;

    int numOverlapTimePoint = 0;

    //sort(timesets.begin(), timesets.end());
    //timesets.erase(unique(timesets.begin(), timesets.end()), timesets.end());
    //sort(ItemSets.begin(), ItemSets.end());
    //ItemSets.erase(unique(ItemSets.begin(), ItemSets.end()), ItemSets.end());

    firstTime = timesets[0];
    firstIndex = 0;
    lastTime = firstTime;
    lastIndex = firstIndex;

    vector<int> correctObjectsets = fullOcc.GetTable(ItemSets[0]);



    for(unsigned int i = 0; i < timesets.size(); i++)
    {
        //intersec current objectsets with next item.
        vector<int> currentObjectsets = fullOcc.GetTable(ItemSets[i]);
        currentTime = timesets[i];
        currentIndex = i;


        cerr<<"currentObjectsets : ";
        for (unsigned int i = 0; i < currentObjectsets.size(); i++)
        {
            cerr << currentObjectsets[i]<<" ";
        }
        cerr<<endl;

        cerr<<"correctObjectsets : ";
        for (unsigned int i = 0; i < correctObjectsets.size(); i++)
        {
            cerr << correctObjectsets[i]<<" ";
        }
        cerr<<endl;


        if(currentTime == (lastTime + 1))
        {

            vector<int> objectTemp;
            for(unsigned int j = 0; j < correctObjectsets.size(); j++)
            {
                if(binary_search(currentObjectsets.begin(), currentObjectsets.end(), correctObjectsets[j]) == true)
                {
                    objectTemp.push_back(correctObjectsets[j]);
                }
            }
            correctObjectsets = objectTemp;
            lastTime = currentTime;
            lastIndex = currentIndex;
        }
        else if(currentTime > (lastTime + 1))
        {

            int temp1 = correctObjectsets.size();
            int temp2 = objectsets.size();

            cerr<<"temp1 : "<<temp1<<endl;
            cerr<<"temp2 : "<<temp2<<endl;

            cerr << "currentTime < (lastTime + 1) "<<endl;
            cerr << "lastTime : "<<lastTime<<endl;
            cerr << "firstTime : "<<firstTime<<endl;
            cerr << "min_t : "<<min_t<<endl;
            cerr << "numOverlapTimePoint : "<<numOverlapTimePoint<<endl;

            if((lastTime - firstTime) >= min_t && (temp1 == temp2))
            {

                cerr << "(lastTime - firstTime) >= min_t && (temp1 == temp2)"<<endl;

                startConvoy.push_back(firstTime);
                startConvoyIndex.push_back(firstIndex);
                endConvoy.push_back(lastTime);
                endConvoyIndex.push_back(lastIndex);

                numOverlapTimePoint += lastTime - firstTime;

                cerr << "numOverlapTimePoint += "<<lastTime - firstTime<<endl;

                firstTime = currentTime;
                firstIndex = currentIndex;
                lastTime = currentTime;
                lastIndex = currentIndex;
            }
            else
            {
                firstTime = currentTime;
                firstIndex = currentIndex;
                lastTime = currentTime;
                lastIndex = currentIndex;
                correctObjectsets = currentObjectsets;
            }
        }
    }

    cerr<<"startConvoy : ";
    for(unsigned int i = 0; i < startConvoy.size(); i++)
    {
        cerr << startConvoy[i]<<" ";

    }
    cerr<<endl;

    cerr<<"startConvoyIndex : ";
    for(unsigned int j = 0; j <startConvoy.size(); j++)
    {
        cerr << startConvoy[j]<<" ";
    }
    cerr<<endl;

    cerr<<"endConvoyIndex : ";
    for(unsigned int j = 0; j <endConvoyIndex.size(); j++)
    {
        cerr << endConvoyIndex[j]<<" ";
    }
    cerr<<endl;


    double valid = (numOverlapTimePoint*1.0) / (numTimes)*1.0;

    cerr << "numOverlapTimePoint end : "<<numOverlapTimePoint<<endl;
    cerr << "valid : "<<valid<<endl;

    myfile << "GP:" << endl;
    cout << "GP:" << endl;
    if(valid >= min_w)
    {

        myfile << "objects: ";
        cout << "objects: ";
        for (unsigned int i = 0; i < objectsets.size(); i++)
        {
            myfile << objectsets[i] << " ";
            cout << objectsets[i] << " ";
        }
        cout << endl;
        myfile << endl;
        myfile << "Time: ";
        cout << "Time: ";


         for(unsigned int i = 0; i < timesets.size(); i++)
        {
                myfile << timesets[i] << " ";
                cout << timesets[i] << " ";
        }
        myfile << endl;
        cout << endl;

        num_GPattern++;
    }
}

/****************************************************************
* Check if an objectset is included in another one
* return 0 if not
* return 1 if included
* return 2 if equel
*****************************************************************/
int Lcm::isIncluded(vector<int> &objectset,vector<int> &objectset2)
{


    if(objectset2.size() < objectset.size())
    {
        return 0;
    }

    for (unsigned int i = 0; i < objectset.size(); i++)
    {

        for (unsigned int j = 0; j < objectset2.size(); j++)
        {

            if(objectset2[j]==objectset[i])
            {
                break;
            }
            else if(j==objectset2.size()-1)
            {
                return 0;
            }

        }
    }
    if(objectset.size()==objectset2.size())
    {
        return 2;

    }
    else
    {
        return 1;

    }

}

/****************************************************************
* Detect group patterns from time-based closed itemsets
*****************************************************************/
void Lcm::rgPatternDetect(OccurenceDeriver &fullOcc)
{
    myfile << "------------"<<endl;
    cout << "------------"<<endl;
    myfile<<"RGpattern: "<<endl;
    cout << "RGpattern: "<<endl;

    vector<vector<int> > alltables = fullOcc.GetAllTable();

    vector<int> time;
    int firsttime = 0;
    int lv2firsttime = -1;
    int lasttime = 0;

//        vector <int> index;
    vector <bool> way;
    vector <int> allfirsttime;
    vector <int> alllasttime;
    bool conv;
    bool dis;

    for (unsigned int i = 1; i < alltables.size(); i++)
    {

        if(isIncluded(alltables[i-1],alltables[i])==1)
        {
            conv = true;

            if(dis)
            {
                allfirsttime.push_back(firsttime);
                alllasttime.push_back(i-1);
                way.push_back(false);
                if((unsigned int)lv2firsttime < (unsigned int)i-1 && lv2firsttime!=-1)
                {
                    firsttime=lv2firsttime;
                }
                else
                {
                    firsttime=i-1;
                }
                lv2firsttime = -1;
                lasttime=i-1;
                dis = false;
            }
        }
        else if(isIncluded(alltables[i],alltables[i-1])==1)
        {
            dis = true;
            if(conv)
            {
                allfirsttime.push_back(firsttime);
                alllasttime.push_back(i-1);
                way.push_back(true);

                if((unsigned int)lv2firsttime < (unsigned int)i-1 && lv2firsttime!=-1)
                {
                    firsttime=lv2firsttime;
                }
                else
                {
                    firsttime=i-1;
                }
                lv2firsttime = -1;
                lasttime=i-1;
                conv = false;
            }
        }
        else if(isIncluded(alltables[i],alltables[i-1])==2 /*&& lv2firsttime ==999999999*/)
        {

            if(lv2firsttime == -1)
            {
                lv2firsttime = i-1;
            }
        }
        else
        {
            allfirsttime.push_back(firsttime);
            alllasttime.push_back(i-1);

            if(conv)
            {
                way.push_back(true);
            }
            else if(dis)
            {
                way.push_back(false);
            }
            else
            {
                way.push_back(NULL);
            }


            firsttime=i-1;
            lv2firsttime = -1;
            lasttime=i-1;
            conv = false;
            dis = false;
        }
    }

    allfirsttime.push_back(firsttime);
    alllasttime.push_back(alltables.size()-1);

    if(conv)
    {
        way.push_back(true);
    }
    else if(dis)
    {
        way.push_back(false);
    }
    else
    {
        way.push_back(NULL);
    }

    int max_;
    for(unsigned int i = 0; i < alllasttime.size(); i++)
    {
        if(way[i]==true)
        {
            max_ = alllasttime[i];
            myfile<<"convergent"<<endl;
            cout<<"convergent"<<endl;
        }
        else if(way[i]==false)
        {
            max_ = allfirsttime[i];
            myfile<<"divergent"<<endl;
            cout<<"divergent"<<endl;
        }

        myfile <<"objects: ";
        cout <<"objects: ";
        for (unsigned int k = 0; k < alltables[max_].size(); k++)
        {
            myfile << alltables[max_][k] <<" ";
            cout << alltables[max_][k] <<" ";
        }
        cout << endl;
        myfile << endl;
        myfile << "time: ";
        cout << "time: ";
        for(int k = allfirsttime[i]; k <= alllasttime[i]; k++)
        {
            myfile << k+1 << " ";
            cout << k+1 << " ";
        }
        myfile << endl;
        cout << endl;
    }
    cout << "------------"<<endl;
}


void Lcm::CloseSwarmDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets)
{
    int lastTime = -1;
    int firstTime = -1;
    int lastIndex = -1;
    int firstIndex = -1;


    cout <<"-----------------------------------------------"<<endl;
    cout <<"detection de close swarm : "<<endl;

    //int numOverlapTimePoint = 0;

    firstTime = timesets[0];
    firstIndex = 0;
    lastTime = firstTime;
    lastIndex = firstIndex;

    vector<int> correctObjectsets = fullOcc.GetTable(ItemSets[0]);

    cout<<"CS:" << endl;
    myfile <<"CS:" << endl;
    if(timesets[timesets.size()-1] - timesets[0] >= min_t)
    {

        cout<<"objects: ";
        myfile <<"objects: ";

        for (unsigned int j = 0; j < objectsets.size(); j++)
        {
            cerr<< objectsets[j] << " ";
            myfile << objectsets[j] << " ";
        }
        cout << endl;
        myfile << endl;
        cout << "Time: ";
        myfile << "Time: ";

        for(unsigned int j = 0; j < timesets.size(); j++)
        {
            cout << timesets[j] << " ";
            myfile << timesets[j] << " ";
        }
        cout << endl;
        myfile << endl;

        num_swarm++;
    }

}

/****************************************************************
* Detect fuzzy closed swarm from time-based closed itemsets
*****************************************************************/
void Lcm::fCloseSwarmDetect(OccurenceDeriver &fullOcc, vector<int> &timesets, vector<int> &ItemSets, vector<int> &objectsets)
{

    cout <<"-----------------------------------------------"<<endl;
    cout <<"detection de fuzzy close swarm : "<<endl;

    int lastTime = -1;
    int firstTime = -1;
    //int currentTime = -1;
    //int currentIndex = -1;
    int lastIndex = -1;
    int firstIndex = -1;

    vector<int> startConvoy;
    vector<int> endConvoy;
    vector<int> startConvoyIndex;
    vector<int> endConvoyIndex;

    //int numOverlapTimePoint = 0;



    firstTime = timesets[0];
    firstIndex = 0;
    lastTime = firstTime;
    lastIndex = firstIndex;

    vector<int> correctObjectsets = fullOcc.GetTable(ItemSets[0]);

    double strong = 0;
    double medium = 0;
    double weak = 0;
    int start = 0;
    int end = 0;
    //if(numOverlapTimePoint >= min_t){
    for(unsigned int i = 0; i < timesets.size() - 1; i++)
    {

        int gap = timesets[i+1] - timesets[i];
        if(gap <= 42 && gap >= 2)
        {
            strong += Degree(gap, 0);
            medium += Degree(gap, 1);
            weak += Degree(gap, 2);
            //cerr << strong/((end - start + 1)*1.0) << " " << medium/((end - start + 1)*1.0) << " " << weak/((end - start + 1)) << endl;
            if(strong/((end - start + 1)*1.0) >= min_w || medium/((end - start + 1)*1.0) >= min_w || weak/((end - start + 1)*1.0) >= min_w)
            {
                end++;
                //cerr << (end - start) << endl;
                vector<int> currentObjectsets = fullOcc.GetTable(ItemSets[i]);
                vector<int> objectTemp;
                for(unsigned int j = 0; j < correctObjectsets.size(); j++)
                {
                    if(binary_search(currentObjectsets.begin(), currentObjectsets.end(), correctObjectsets[j]) == true)
                    {
                        objectTemp.push_back(correctObjectsets[j]);
                    }
                }
                correctObjectsets = objectTemp;
            }
            else
            {
                int temp1 = correctObjectsets.size();
                int temp2 = objectsets.size();
                myfile << "fClSw:" << endl;
                cout << "fClSw:" << endl;
                if(end - start >= min_t && temp1 == temp2)
                {
                    myfile << "objects: ";
                    cout << "objects: ";
                    for (unsigned int j = 0; j < objectsets.size(); j++)
                    {
                        myfile << objectsets[j] << " ";
                        cout << objectsets[j] << " ";
                    }
                    cout << endl;
                    myfile << endl;
                    myfile << "Time: ";
                    cout << "Time: ";
                    for(int k = start; k <= end; k++)
                    {
                        myfile << timesets[k] << " ";
                        cout << timesets[k] << " ";
                    }
                    myfile << endl;
                    cout << endl;

                    num_fCS++;

                    //to compute the number of weak, medium, strong time gaps.
                    if(weak >= medium && weak >= strong)
                    {
                        numWeakGap++;
                    }
                    else if(medium >= weak && medium >= strong)
                    {
                        numMediumGap++;
                    }
                    else if(strong >= weak && strong >= medium)
                    {
                        numStrongGap++;
                    }
                    ////////////////////////////////////////////////////////////
                }
                correctObjectsets = fullOcc.GetTable(ItemSets[i+1]);
                start = i+1;
                end = i+1;
                strong = 0;
                medium = 0;
                weak = 0;
            }
        }
        else if(gap > 42)
        {
            int temp1 = correctObjectsets.size();
            int temp2 = objectsets.size();
            myfile << "fClSw:" << endl;
            cout << "fClSw:" << endl;
            if(end - start >= min_t && temp1 == temp2)
            {
                myfile << "objects: ";
                cout << "objects: ";
                for (int j = 0; j < (int)objectsets.size(); j++)
                {
                    myfile << objectsets[j] << " ";
                    cout << objectsets[j] << " ";
                }
                cout << endl;
                myfile << endl;
                myfile << "Time: ";
                cout << "Time: ";
                for(int k = start; k <= end; k++)
                {
                    myfile << timesets[k] << " ";
                    cout << timesets[k] << " ";
                }
                myfile << endl;
                cout << endl;
                num_fCS++;
                //to compute the number of weak, medium, strong time gaps.
                if(weak >= medium && weak >= strong)
                {
                    numWeakGap++;
                }
                else if(medium >= weak && medium >= strong)
                {
                    numMediumGap++;
                }
                else if(strong >= weak && strong >= medium)
                {
                    numStrongGap++;
                }
                ////////////////////////////////////////////////////////////
            }
            correctObjectsets = fullOcc.GetTable(ItemSets[i+1]);
            start = i+1;
            end = i+1;
            strong = 0;
            medium = 0;
            weak = 0;
        }
        else if(gap == 1)
        {
            end++;
            vector<int> currentObjectsets = fullOcc.GetTable(ItemSets[i]);
            vector<int> objectTemp;
            for(unsigned int j = 0; j < correctObjectsets.size(); j++)
            {
                if(binary_search(currentObjectsets.begin(), currentObjectsets.end(), correctObjectsets[j]) == true)
                {
                    objectTemp.push_back(correctObjectsets[j]);
                }
            }
            correctObjectsets = objectTemp;
        }
    }

    int temp1 = correctObjectsets.size();
    int temp2 = objectsets.size();

    myfile << "fClSw:" << endl;
    cout << "fClSw:" << endl;
    if((end - start + 1) >= min_t && temp1 == temp2)
    {
        myfile << "objects: ";
        cout << "objects: ";
        for (unsigned int j = 0; j < objectsets.size(); j++)
        {
            myfile << objectsets[j] << " ";
            cout << objectsets[j] << " ";
        }
        cout << endl;
        myfile << endl;
        myfile << "Time: ";
        cout << "Time: ";
        for(int k = start; k <= end; k++)
        {
            myfile << timesets[k] << " ";
            cout << timesets[k] << " ";
        }
        myfile << endl;
        cout << endl;
        num_fCS++;
        //to compute the number of weak, medium, strong time gaps.
        if(weak >= medium && weak >= strong)
        {
            numWeakGap++;
        }
        else if(medium >= weak && medium >= strong)
        {
            numMediumGap++;
        }
        else if(strong >= weak && strong >= medium)
        {
            numStrongGap++;
        }
    }
    //}

}

/****************************************************************
* Detect fuzzy closed swarm from time-based closed itemsets
*****************************************************************/
double Lcm::Degree(int &gap, int type)
{
    if(type == 0 && gap <= 22)
    {
        double value = (gap-22)*(-1.0)/((22-2)*1.0);
        return value;
    }
    else if(type == 1 && gap >= 2 && gap <= 42)
    {
        if(gap <= 22)
        {
            double value = (gap-2)*1.0/((22-2)*1.0);
            return value;
        }
        else if(gap > 22 && gap <= 42)
        {
            double value = (gap-42)*-1.0/((42-22)*1.0);
            return value;
        }
    }
    else if(type == 2 && gap >= 22 && gap <= 42)
    {
        double value = (gap-22)*1.0/((22-2)*1.0);
        return value;
    }
    else
    {
        return 0.0;
    }
}
