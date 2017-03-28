#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "../include/Config.h"
#include "../include/Lcm.h"
#include "../include/Database.h"



Config cfg;
/* Globals */
void Usage();
void Version();
void ParseParameters (int argc, char **argv);
void RandomSampling(int numObject);
void NestedBlock(const string &filename, const OccurenceDeriver &occ, int numObject);

string name;
vector<string> filenames;
bool           toggle_verbose       = false;
int            min_sup              = 1;    // minimum support for LCM
int            max_pat              = 0;    // maximum_itemset_size : default infinity;
int            min_t                = 0;    // minimum_itemset_size
int            num_block            = 1;    // number of blocks
int            numObject            = 0;    // number of objects
double         min_w                = 0.01; // min weight for group pattern
//int            percent              = 10;

using namespace std;
ofstream errorFile( "ErrorFile.log" );// redirecting errors for cfg.DEBUGging

/*****************************************************************************************
 * The main function
 *****************************************************************************************/
int main(int argc, char **argv)
{

    // cerr redirection in a log file
    if (cfg.DEBUG)
    {
        cerr.rdbuf( errorFile.rdbuf() );
    }

    if(cfg.DEBUG_PERSO){
        cerr.rdbuf (errorFile.rdbuf());
    }

    long int start, end;
    double cpu_time_used;

    ParseParameters(argc, argv);


    string name = filenames[0];
    string timeindex;
    std::size_t pos = name.find(".");
    std::string beginpos =name.substr (0,pos);
    std::string endpos = name.substr (pos,name.length());
    //name.replace(name.end()-4,name.end(),"");
    timeindex=beginpos + "timeindex" + endpos;


    // initialise output file
    string s1;
    string s2;
    string s3;
    stringstream out1;
    stringstream out2;
    stringstream out3;
    out1 << (min_sup);
    s1 = out1.str();
    out2 << (min_t);
    s2 = out2.str();
    out3 << (min_w);
    s3 = out3.str();


    filenames.push_back(timeindex);

    Database fullDB;

    // Pour chaque objet (x) du fichier importe les clusters (i) ou il est situé dans Database.transaction[x].itemset[i]
    // et mets à jour numObject suivant le nombre d'objets dans le fichier
    fullDB.ReadFile(filenames[0], numObject);

    cout << "\t\tDatabase description and parameters values"<< endl;
    cout << "\tInput File: " << filenames[0] << endl;
    cout << "\tAssociated Time Index FIle: " << timeindex << endl;
    string filename = beginpos + "sortie_" + s1 + "mint" + s2 + "minw" + s3 + endpos;
    cout << "\tOuput file: " << filename << endl;

    if (cfg.DEBUG)
    {    // affiche des trucs dans ErrorFile.log si débug plus d'autres infos

        cerr << "\t\tDatabase description and parameters values"<< endl;
        cerr << "\tInput File: " << filenames[0] << endl;
        cerr << "\tAssociated Time Index FIle: " << timeindex << endl;
        cerr << "\tErrorFile: ErrorFile.log" << endl;
        cerr << "\tOuputFile: " << filename << endl;
        cerr << endl;
        cerr << "\tNumber of objects: " << numObject << endl;
        cerr << "\tMaxitem: " << fullDB.GetMaxItem() << endl;
        cerr << "\tMinItem: " << fullDB.GetMinItem() << endl;
    }

    OccurenceDeriver fullOcc(fullDB); // trie dans OccurenceDeriver.table les transaction en fonction des clusters (l'inverse de Database.transactions)

    int numItemsLevel2 = 0;

    //create database directly from blocks
    vector<Transaction> transactionsets;
    for(int i = 0; i < numObject; i++)
    {
        Transaction transaction;
        transaction.clear();
        transactionsets.push_back(transaction);
    }


    if (cfg.DEBUG)
    {
        cerr << "\tDatabase Content:" << endl;
        for(std::size_t i=0; i<transactionsets.size(); ++i)
        {
            std::cerr << "\t\t" <<fullDB.GetTransaction(i).id << ' ';
            for(std::size_t j=0; j<fullDB.GetTransaction(i).itemsets.size(); ++j)
            {
                std::cerr << "C" << fullDB.GetTransaction(i).itemsets[j] << ' ';
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;
    }



    //for Compo algorithm (Mining Compression Movement Patterns for Moving Objects)
    //vector<vector<int> > _itemDecoding;

    ///////////////////////////////////////
    Lcm lcmTimeIndex(cout, min_sup, 0, 0);
    // initialise avec cout et min_sup, max_pat (maximum_itemset_size) et min_t (minimum_itemset_size)

    lcmTimeIndex.ReadItemTindexFile(filenames[1]);

    // initialisation des block de seconde dimension
    vector<vector<int> > numIDlevel2;
    numIDlevel2.clear();
    vector<vector<int> > timeIDlevel2;
    timeIDlevel2.clear();

    start = clock(); // début de l'algo
    long int loadFileTime=0; //compute load file time


    for(int i = 1; i <= num_block; i++)
    {
        std::string s;
        std::stringstream out;
        out << (i-1);
        s = out.str();
        filenames.push_back(filenames[0] + s + ".dat"); //"_MarketBasket.dat");


        if (cfg.DEBUG)
        {
            cerr << "Running lcm" << endl;
        }
        // set LCM parameter
        Lcm lcm(cout, min_sup, 0, 0);

        if (cfg.DEBUG)
        {
            cerr <<" lcm object construction" << endl;
        }

        lcm.id = i-1; // lcm.id = 0 car un seul block
        //start = clock();
        //lcm.ReadItemTindexFile(filenames[1]);

        if (cfg.DEBUG)
        {
            cerr << "GetMaxItem in DB = " << fullDB.GetMaxItem() << endl;
            cerr << "Running RunLcmNew ..." << endl;

        }


        lcm.RunLcmNew(fullDB, transactionsets, numItemsLevel2, lcmTimeIndex.item_temp, lcmTimeIndex.timeindex, numIDlevel2, timeIDlevel2);


    }

    //create database from transactions which are return from blocks
    Database Database2;


    for(int i =0; i < numObject; i++)
    {
        std::sort(transactionsets.at(i).itemsets.begin(), transactionsets.at(i).itemsets.end());
        transactionsets.at(i).id = i;
        Database2.database.push_back(transactionsets.at(i));

    }



    if (cfg.DEBUG)
    {
        cerr <<"DB fullDB: " << endl;
        for(std::size_t i=0; i<transactionsets.size(); ++i)
        {
            std::cerr << fullDB.GetTransaction(i).id << ' ';
            for(std::size_t j=0; j<fullDB.GetTransaction(i).itemsets.size(); ++j)
            {
                std::cerr << fullDB.GetTransaction(i).itemsets[j] << ' ';
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;

        cerr <<"Content of Database2: " << endl;
        for(std::size_t i=0; i<transactionsets.size(); ++i)
        {
            std::cerr << Database2.GetTransaction(i).id << ' ';
            for(std::size_t j=0; j<Database2.GetTransaction(i).itemsets.size(); ++j)
            {
                std::cerr << Database2.GetTransaction(i).itemsets[j] << ' ';
            }
            std::cerr << std::endl;
        }
        std::cerr << std::endl;

    }

    Database2.FindMaxAndMinItem();

    //create level 2 of LCM
    Lcm lcm(cout, min_sup, max_pat, min_t);
    lcm.numTimes=lcmTimeIndex.numTimes;


    lcm.num_itemset = 0; //count no.itemsets
    lcm.num_convoy  = 0; //count no.convoys
    lcm.num_GPattern  = 0; //count no.group patterns
    lcm.num_fCS  = 0; //count no.fuzzy closed swarms
    lcm.num_swarm  = 0; //count no. swarms
    lcm.min_w = min_w;
    lcm.numStrongGap = 0;//count no.fuzzy strong gap
    lcm.numMediumGap = 0;//count no.fuzzy medium gap
    lcm.numWeakGap = 0;//count no.fuzzy weak gap




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //out put to file


    if (cfg.DEBUG)
    {
        cerr <<"----------------------------------------"<<endl;
        cerr<<"creation du fichier de sortie"<<endl;
    }

    //string filename = name + "sortie_" + s1 + "mint" + s2 + "minw" + s3 + ".dat";
    lcm.myfile.open(filename.c_str(), ios::out);


    if(lcm.myfile.is_open())
    {
        //run the level 2 of LCM
        lcm.RunLcm(fullOcc, Database2, numIDlevel2, timeIDlevel2);

            cout << "No. Itemsets: " << lcm.num_itemset << endl;
            cout << "No. Convoys: " << lcm.num_convoy << endl;
            cout << "No. Group Patterns: " << lcm.num_GPattern << endl;
            cout << "No. f-Closed Swarm: " << lcm.num_fCS << endl;

    }

    // compute the CPU Runing Time///////////////////////
    end = clock();
    cpu_time_used = (double) (end - start - loadFileTime)/CLOCKS_PER_SEC;
    if(cpu_time_used > 0)
    {
        cout << cpu_time_used << endl;
    }
    /////////////////print out to file////////////////////
    lcm.myfile << "------------"<<endl;
    lcm.myfile << "Summary:" << endl;
    lcm.myfile << "RuningTime: " << cpu_time_used << endl;
    lcm.myfile << "NoItemsets: " << lcm.num_itemset << endl;
    lcm.myfile << "NoCS: " << lcm.num_swarm<< endl;
    lcm.myfile << "NofCS: " << lcm.num_fCS << endl;
    //cout << "NoWeakGap: " << lcm.numWeakGap << endl;
    //cout << "NoMediumGap: " << lcm.numMediumGap << endl;
    //cout << "NoStrongGap: " << lcm.numStrongGap << endl;
    lcm.myfile << "NoCV: " << lcm.num_convoy << endl;
    lcm.myfile << "NoGP: " << lcm.num_GPattern << endl;
    lcm.myfile.close();


    return 0;
}

/*****************************************************************************
 * Version
 ****************************************************************************/
void Version()
{
    cerr << "GeT_Move: An Efficient and Unifying Spatio-Temporal Pattern Mining Algorithm" << endl
         << "Written by Phan Nhat Hai" << endl << endl;
}

/***************************************************************************
 * Usage
 ***************************************************************************/
void Usage()
{
    cout << endl
         << "Usage: lcm min_sup max_pat min_t num_block inputfile min_w" << endl << endl
         << "\t min_sup stands for the minimal support [1..100], i.e. the number of objects embedded in the clusters" << endl
         << "\t maxp_pat stands for the maximal size of the itemsets (0: no constraints" << endl << endl
         << "\t num_block stands for the number of block for the incremental version (integer)" << endl
         << "\t inputfile: the name of the dataset. Caution: the timeindexfile must have the same name" << endl
         << "\t min_w: time constraints " << endl
         << "\t example: lcm 2 0 1 1 mini.dat 0.01"
         << endl;
    exit(0);
}

/*****************************************************************************
 * ParseParameters
 *****************************************************************************/
void ParseParameters (int argc, char **argv)
{
    if (argc == 1) Usage();
    filenames.clear();

    int argno = 1;
    min_sup = atoi(argv[argno]);

    argno = 2;
    max_pat = atoi(argv[argno]);


    argno = 3;
    min_t = atoi(argv[argno]);


    argno = 4;
    num_block = atoi(argv[argno]);


    argno = 5;
    filenames.push_back(argv[argno]);


    argno = 6;
    min_w = atoi(argv[argno]);

}

void RandomSampling(int numObject)
{
    srand((unsigned)time(0));
    int random_integer;
    vector<int> objectList;
    objectList.clear();
    for(int i = 10; i <= 100; i = i + 10)
    {
        objectList.clear();
        while(int(objectList.size()) < i*numObject/100)
        {
            random_integer = (rand()%numObject)+1;
            bool flag = false;
            for(int j = 0; j < int(objectList.size()); j++)
            {
                if(objectList[j] == random_integer)
                {
                    flag = true;
                }
            }
            if(flag == false)
            {
                objectList.push_back(random_integer);
            }
        }
        std::sort(objectList.begin(), objectList.end());
        string s1;
        stringstream out1;
        out1 << (i);
        s1 = out1.str();
        fstream file;
        string filename = "/home/phannhathai/Programming/IncrementalVersionLCM/OriginalFullSwain/randomSample/random" + s1 + ".dat";
        file.open(filename.c_str(), ios::out);
        for(int j = 0; j < int(objectList.size()); j++)
        {
            file << objectList[j] << " ";
        }
        file << endl;
        file.close();
    }
}

void NestedBlock(const string &filename, const OccurenceDeriver &occ, int numObject)
{
    string line;
    int item;
    vector<int> itemIndex;
    itemIndex.clear();
    ifstream is(filename.c_str());
    while (getline (is, line))
    {
        istrstream istrs ((char *)line.c_str());
        istrs >> item;
        istrs >> item; //get the second number
        itemIndex.push_back(item);
    }

    /////////create fully nested blocks////////////////////////
    vector<vector<int> > Blocks;
    vector<int> b;
    vector<int> SparseB;
    b.push_back(0);
    for(int i = 0; i < int(itemIndex.size())-1; i++)
    {
        vector<int> table1 = occ.GetTable(itemIndex[i]);
        vector<int> table2 = occ.GetTable(itemIndex[i+1]);
        int count = 0;
        for(int j = 0; j < int(table2.size()); j++)
        {
            for(int k = 0; k < int(table1.size()); k++)
            {
                if(table1[j] == table2[k])
                {
                    count++;
                }
            }
        }
        if(count == int(table2.size()))  //the next item is nested
        {
            b.push_back(i+1);
        }
        else if(count != int(table2.size()))  //the next item is not nested
        {
            if(int(b.size()) <= 2) //so small blocks will be merged into sparse block
            {
                for(int j = 0; j < int(b.size()); j++)
                {
                    SparseB.push_back(b[j]);
                }
                b.clear();
                b.push_back(i+1);
            }
            else if(int(b.size()) > 2)  //save block and create new one
            {
                Blocks.push_back(b);
                b.clear();
                b.push_back(i+1);
            }
        }
    }

    //compute the average number of item in fully nested Blocks//
    int count = 0;
    for(int i = 0; i < int(Blocks.size()); i++)
    {
        count += int(Blocks[i].size());
    }

    //////////////////////////////////////////////////////////


}
