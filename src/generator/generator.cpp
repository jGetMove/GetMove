#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>
#include <set>
#include <list>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

int nb_dates;
int avg_nb_clusters;
int multi_clusters; // if =0 one cluster; =1 more than one
int nb_objects;
int nb_itemsets;
int support;
string filename;

vector<vector<int> > Itemsets;

int countDigits(int number) {
    if (number < 10) {
        return 1;
    }
    int count = 0;
    while (number > 0) {
        number /= 10;
        count++;
    }
    return count;
}

void parse (int argc, char **argv){
    if (argc != 8){
        cout << "usage: " << argv[0] << " nb_dates avg_nb_clusters multi_clusters nb_objects nb_itemsets support filename" << endl << endl;
        cout << "nb_dates: number of different dates" << endl;
        cout << "avg_nb_clusters: average number of clusters per date" << endl;
        cout << "multi_cluster: = 0 if one object can only belong to one cluster or 1 otherwise"<< endl ;
        cout << "nb_objects: number of objects that must be generated" << endl;
        cout << "nb_itemsets: minimal numnber of different itemsets that must be generated" << endl;
        cout << "support: number of occurrences of the different itemsets that must be generated" << endl;
        cout << "example: " << argv[0] << " 100 4 0 1000 5 20 mini.dat will generate two files. " << endl;
        cout << "One having the timeindex (minitimeindex.dat) and the other (mini.dat) having an array" << endl;
        cout << "of 100 times having for each objects (1000) an average number of clusters of 4 and" << endl;
        cout << "where an object can only belong to one cluster." << endl;
        exit(0);
    }
        nb_dates=atoi(argv[1]);
        avg_nb_clusters=atoi(argv[2]);
        multi_clusters=atoi(argv[3]);
        nb_objects=atoi(argv[4]);
        nb_itemsets=atoi(argv[5]);
        support = atoi (argv[6]);
        filename=argv[7];
    if (nb_itemsets > nb_objects) {
        cout << "It is not possible to generate " << nb_itemsets << " for " << nb_objects << endl;
        exit(0);
    }
    if (nb_itemsets*support > nb_objects){
        cout << "The support value " << support << " multiplied by the number of itemsets " << nb_itemsets << " is greater than the number of objects " << nb_objects << endl;
        exit(0);
    }

}

int main (int argc, char **argv){




    parse (argc, argv);
    string name = filename;

    cout << "\t\t\tGenerating data" << endl;
    cout << "\tOuput files:" << endl;
    filename+=".dat";
    cout << "\t\t" << filename << endl;
    std::size_t pos = filename.find(".");
    std::string begin = filename.substr (0,pos);
    std::string end = filename.substr (pos,filename.length());
    string timeindex=begin+"timeindex"+end;
    cout << "\t\t" << timeindex << endl;

    name = filename;
    pos = filename.find(".");
    begin = filename.substr (0,pos);
    end = filename.substr (pos,filename.length());
    name.replace(name.end()-4,name.end(),"");
    string itemsetindex = begin+"itemsets"+end;
    cout << "\t\t" << itemsetindex << endl;
    /* initialize random seed: */
    srand (time(NULL));

    ofstream fichier(timeindex.c_str(), ios::out | ios::trunc);   // on ouvre le fichier en lecture

    if(!fichier)  {
        cerr << "Unable to open: " << timeindex << endl;
        exit(-1);
    }



    cout << "Generating file: "<< timeindex << " ..." << endl;
    cout << "with " << nb_dates << " dates and an average of " << avg_nb_clusters << " clusters per date" << endl;
    int randomNbClusters;
    int item=0;
    for (int i = 1; i <= nb_dates; i++){
        randomNbClusters=rand() % avg_nb_clusters + 1;
        for (int j = 0; j < randomNbClusters; j++){
            //cout << "\t" << i << "\t" << item << endl;
            fichier << "\t" << i << "\t" << item++ << endl;
        }

    }
    fichier.close();


    vector <int> times;
    vector <int> previousClusters;
    vector <int> listofCluster;
    vector<vector<int> > clusters;
    ifstream fileTimes(timeindex.c_str(), ios::in);


    //read one space separated word from the input stream

    string ligne;
    std::string delimiter = "\t";
    std::string token;
    std::string previousToken="";
    int pas=0;
    std::string cluster;
    int currentDate = 1;
    while(getline(fileTimes, ligne))  // tant que l'on peut mettre la ligne dans "contenu"
    {
        if (pas==0) {// special case for the 1st iteration
            std::string s = ligne;
            s.erase(0, delimiter.length());
            token= s.substr(0, s.find(delimiter));
            currentDate=atoi(token.c_str());
            int pos = s.find(delimiter);
            s.erase(pos, delimiter.length());
            cluster= s.substr(pos, s.find(delimiter));
            listofCluster.push_back(atoi(cluster.c_str())); // save in listofCluster
            while (getline(fileTimes, ligne)){
                s = ligne;
                s.erase(0, delimiter.length());
                string token2= s.substr(0, s.find(delimiter));
                int currentDateLocal=atoi(token2.c_str());
                pos = s.find(delimiter);
                s.erase(pos, delimiter.length());
                string cluster2= s.substr(pos, s.find(delimiter));
                if (currentDate==currentDateLocal){
                    listofCluster.push_back(atoi(cluster2.c_str()));
                }
                if (currentDate!=currentDateLocal){
                    // same date update the vector
                    clusters.push_back(listofCluster);
                    listofCluster.clear();
                    previousToken=token2;
                    break;
                }


            }
            pas=1;
        }
        std::string s = ligne;
        s.erase(0, delimiter.length());
        token= s.substr(0, s.find(delimiter));
        currentDate=atoi(token.c_str());
        int pos = s.find(delimiter);
        s.erase(pos, delimiter.length());
        cluster= s.substr(pos, s.find(delimiter));

        listofCluster.push_back(atoi(cluster.c_str()));


        if (previousToken==token  ) {
                previousClusters.push_back(atoi(cluster.c_str()));
        }


        if (previousToken!=token) {
            clusters.push_back(previousClusters);
            previousClusters.clear();
            listofCluster.clear();
            previousClusters.push_back(atoi(cluster.c_str()));
            previousToken=token;
        }

    }
    fileTimes.close();
    clusters.push_back(previousClusters);// for the last date

    //cout << "Nb of generated date for the vector " << clusters.size() << endl;

    // generating itemsets
    vector <int> items;
    int randitems;
    int choice;
    srand (time(0));
    bool insert;

    int nb_of_generated_itemsets=0;
    while (nb_of_generated_itemsets != nb_itemsets){
        items.clear();insert=true;
        for (int k=0; k < (int)clusters.size() ; k++) {
            choice=rand() % 2;
            if (choice) {
                    int size= clusters[k].size();
                    randitems=rand()%size;
                    //cout << /*clusters[k][randitems] << */"  (" << randitems <<") " ;
                    items.push_back(clusters[k][randitems]);

            }
        }

        for (int j=0; j < (int)Itemsets.size();j++) {
            if (items==Itemsets[j]) insert=false;
        }
        if (insert){
        Itemsets.push_back(items);
        items.clear();
        nb_of_generated_itemsets++;
        }
    }


    cout << "Generating the itemset file: " << itemsetindex << endl;
    ofstream fileitemsets(itemsetindex.c_str(), ios::out | ios::trunc);   // on ouvre le fichier en lecture

    if(!fichier)  {
        cerr << "Unable to open: " << itemsetindex << endl;
        exit(-1);
    }
    for (int i=0; i < nb_itemsets; i++){
        for (int j=0; j < (int)Itemsets[i].size();j++ ){
            //cout << Itemsets[i][j] << " ";
            fileitemsets << Itemsets[i][j] << " ";
        }
        //cout << endl;
        fileitemsets << endl;
    }
    fileitemsets.close();


    cout << "Generating the data file: "<< filename << endl;

    ofstream fileObject(filename.c_str(), ios::out | ios::trunc);   // on ouvre le fichier en lecture

    if(!fichier)  {
        cerr << "Unable to open: " << filename << endl;
        exit(-1);
    }

    // init the file with the itemsets
    for (int i=0; i < nb_itemsets; i++){
        for(int s=0; s < support; s++){
            for (int j=0; j < (int)Itemsets[i].size();j++ ){
                //cout << Itemsets[i][j] << " ";
                fileObject << Itemsets[i][j] << " ";
            }
        //cout << endl;
        fileObject << endl;
        }
    }

    int randomcluster;
    int randomdate;

    int randominsert;
    /* initialize random seed: */
    srand (time(NULL));

    for (int i=(nb_itemsets*support) ; i < nb_objects; i++){
        if (multi_clusters==0) {
            for (int k=0;  k < (int)clusters.size() ; k++){
                randomdate=rand() % 2;
                randomcluster=rand() % clusters[k].size();
                if (randomdate ==1){
                  cout << clusters[k][randomcluster]  << ' ' ;
                  fileObject << clusters[k][randomcluster] << ' ';
                }
                //else {
                //    numDigits = countDigits(clusters[k][randomcluster]);
                //    for (int l=0 ; l < numDigits; l++){
                //        cout << ' ';
                //        fileObject << ' ';
                //    }
                //    cout << ' ';
                //    fileObject << ' ';
                //
                //}
            }

        }
        else {
            // Multi-clusters
            for (int k=0; k < (int)clusters.size(); k++){
                randomdate=1;//rand() % 2;
                    for (int m=0; m < (int)clusters[k].size(); m++){
                        randominsert=rand() % 2;
                        if (randominsert) {
                            if (randomdate ==1){
                                cout << clusters[k][m]  << ' ' ;
                                fileObject << clusters[k][m] << ' ';

                            }
                           // else {
                           //     numDigits = countDigits(clusters[k][m]);
                           //     for (int l=0 ; l < numDigits; l++){
                           //         cout << ' ';
                           //        fileObject << ' ';
                           //     }
                           //     cout << ' ';
                           //     fileObject << ' ';
                           //
                        }
                        }
              }
        }
        fileObject << endl;
        cout << endl;

    }
    fileObject.close();

    cout << endl << endl;
    cout << "\t\tThe files generated are: " << endl;
    cout << "\t\t\t" << filename << endl;
    cout << "\t\t\t" << timeindex << endl;
    cout << "\t\t\t" << itemsetindex << endl;

return 0;
}
