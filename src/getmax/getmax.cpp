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


string filename;

vector<vector<int> > Itemsets;
vector<vector<int> > GoodItemsets;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

bool mySortFunction(const vector<int>& inner1, const vector<int>& inner2) {
    return inner1.size() < inner2.size();
}

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
    if (argc != 2){
        cout << "usage: " << argv[0] << " filename" << endl << endl;
        exit(0);
    }
        filename=argv[1];
}

int main (int argc, char **argv){




    parse (argc, argv);
    ifstream fileTimes(filename.c_str(), ios::in);


    cout << "\t\t\tExtracting Maximal itemsets and trajectories from: " << filename << endl << endl;

    string ligne;
    string itemset="itemset:";
    string newitemset="end newitemset";
    std::string delimiter = " ";
    std::string token;
    std::string cluster;
    vector <int> items;
    vector <string> removeitemsets;

    int nb=0;
    while(getline(fileTimes, ligne))
    {
        std::size_t found = ligne.find(itemset);
        if (found!=std::string::npos){
            items.clear();
            string name = ligne;
            name.erase(0,itemset.length()+1);
            cluster= name;
            std::istringstream buf(cluster);
            for(std::string token; getline(buf, token, ' '); ){
                items.push_back(atoi(token.c_str()));
            }
            Itemsets.push_back(items);
            nb++;
        }
    }
    fileTimes.close();

    sort(Itemsets.begin(), Itemsets.end(), mySortFunction);


    bool Result1;
    for (int i=0; i < (int)Itemsets.size(); i++){
            for (int j=i+1; j < (int)Itemsets.size(); j++){
                Result1 = includes(Itemsets[j].begin(), Itemsets[j].end(), Itemsets[i].begin(), Itemsets[i].end());
                if(Result1){
                    removeitemsets.push_back(patch::to_string(i));
                    break;
                }
            }

    }


    bool insert;
    vector <int> thegooditems;
    for (int i =0; i < (int)Itemsets.size(); i++){
        insert=true;
        for (int j = 0; j < (int)removeitemsets.size(); j++) {
            if (patch::to_string(i)==removeitemsets[j]) {
              insert=false; break;
            }
        }
        if (insert) {
            for (int k=0; k < (int)Itemsets[i].size(); k++) thegooditems.push_back(Itemsets[i][k]);
            GoodItemsets.push_back(thegooditems);
            thegooditems.clear();
        }
    }

    cout << "The generated maximal itemsets are: " << endl;
    for (int i=0; i < (int)GoodItemsets.size(); i++){
        for (int j=0; j < (int)GoodItemsets[i].size(); j++)
            cout << GoodItemsets[i][j] << " ";
        cout << endl;
    }

    string nameoutputfile;

    std::size_t pos = filename.find(".");
    std::string begin = filename.substr (0,pos);
    std::string end = filename.substr (pos,filename.length());
    nameoutputfile=begin+"Max"+end;
    ofstream fichier(nameoutputfile.c_str(), ios::out | ios::trunc);
    ifstream filetoTake(filename.c_str(), ios::in);
    if(!filetoTake)  {
        cerr << "Unable to open: " << filename << endl;
        exit(-1);
    }

    vector <int> currentitems;
    ligne.clear();
    int nbitemsets=0;
    int fCS=0;
    int CS=0;
    int CV=0;
    int GP=0;
    string tirets="------------";
    string RGpattern= "RGpattern:";
    string newfCS="fClSw";
    string summary="Summary:";
    string newCS="CS";
    string newCV="CV";
    string newGP="GP";
    string terms;
    std::size_t found;
    cout << "writing the file: " << nameoutputfile << " ..." << endl;
    while(getline(filetoTake, ligne))
    {
        // find the RGPatterns
        found = ligne.find(RGpattern);
        if (found!=std::string::npos){
            fichier << "------------" << endl;
            fichier << ligne << endl;
            while(getline(filetoTake, ligne)){
                found=ligne.find(tirets);
                if (found!=std::string::npos){
                    fichier << "------------" << endl;
                    break;
                }
                fichier << ligne << endl;
            }
        }

        found = ligne.find(summary);
        // find the final summary
        if (found!=std::string::npos){
            fichier << "Summary:" << endl;
            fichier << "NoItemsets: " << nbitemsets << endl;
            fichier << "NoCS: " << CS << endl;
            fichier << "NofClSw: " << fCS << endl;
            fichier << "NoCV: " << CV << endl;
            fichier << "NoGP:"  << GP << endl;
            fichier.close();
            filetoTake.close();
            break;
        }
        found = ligne.find(itemset);
        // find new itemset
        if (found!=std::string::npos){
            items.clear();
            string name = ligne;
            name.erase(0,itemset.length()+1);
            cluster= name;
            std::istringstream buf(cluster);
            currentitems.clear();
            for(std::string token; getline(buf, token, ' '); ){
                currentitems.push_back(atoi(token.c_str()));
            }
              for (int i=0; i < (int)GoodItemsets.size();i++){
                bool result = std::equal( GoodItemsets[i].begin(), GoodItemsets[i].end(), currentitems.begin() );

                if (result){
                    nbitemsets++;
                    // save in the output Maxitemset file
                    fichier << "newitemset" << endl;
                    fichier <<"-------------------"<< endl;
                    fichier << "itemset: ";
                    for (int i =0; i < (int)currentitems.size(); i++) fichier << currentitems[i] << " ";
                    fichier << endl;
                    std::string Buffer;
                    while(getline(filetoTake, ligne))
                    {// we copy the data from original file
                        terms=ligne.substr(0, 3);
                        found = terms.find(newfCS);
                        if (found!=std::string::npos) fCS++;
                        terms=ligne.substr(0,2);
                        found = terms.find(newCS);
                        if (found!=std::string::npos) CS++;
                        found = terms.find(newCV);
                        if (found!=std::string::npos) CV++;
                        found = terms.find(newGP);
                        if (found!=std::string::npos) GP++;
                        found = ligne.find(newitemset);
                        if (found!=std::string::npos){
                            fichier << "end newitemset" << endl;
                            // find the end of itemset
                            break;
                        }
                        else fichier << ligne << endl;;

                    }
                }
            }

        }
    }

    cout << endl << endl;
    cout << "\t\tThe file generated is: " << nameoutputfile  << endl << endl;


    return 0;
}
