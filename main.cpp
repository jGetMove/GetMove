#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <utility>
#include <stdlib.h>

using namespace std;


std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}

int main()
{

    ifstream myfile;
    string filename = "files/12ContexteSeq.txt";
    myfile.open(filename.c_str(), ios::in);

    fstream timeindex;
    filename = "files/timeindex.dat";
    timeindex.open(filename.c_str(),ios::out);

    fstream data;
    filename = "files/data.dat";
    data.open(filename.c_str(),ios::out);


    if(myfile.is_open())
    {
        cout<<"fichier ouvert"<<endl;

        string line;
        std::vector<std::string> _line;

        vector<vector<int> > temp;



        while (getline (myfile, line))
        {
            _line = explode(line,' ');

             for(int i=0 ; i < _line.size() ; i++){
                cout << _line[i] <<" ";
            }
            cout<<endl;

            if(_line.size() == 3){

                timeindex << _line[0] << " " << _line[1] << endl;

                int ID = atoi( _line[1].c_str());
                int C = atoi( _line[2].c_str());


                cout << "------------------"<<endl;
                cout << "ID : "<< ID <<endl;
                cout << "C : "<< C <<endl;
                cout << "temp : " <<endl;
                for(int i = 0 ; i < temp.size(); i++){

//                    cout << i <<" [ " ;
//                    for(int j = 0 ; j < temp[i].size(); j++){
//                        cout << j << " : "<< temp[i][j] << " ";
//
//                    }
//                    cout<<" ] "<<endl;
                }
                cout << "------------------"<<endl;

                vector<int> v;
                while(temp.size() < ID+1){
                    temp.push_back(v);
                }

//                if(temp.size() < ID){
//
//                    vector<int> t ;
//                    t.push_back(C);
//                    temp.push_back(t);
//                }
//                else{
                temp[ID].push_back(C);
//                }

            }







        }

         for(int i = 0 ; i < temp.size(); i++){
//                data << i << " : ";
                for(int j = 0 ; j < temp[i].size(); j++){
                    data << temp[i][j] << " ";
                }
                data << endl;
            }



    }
    else{
        cout<<"fichier introuvable"<<endl;
    }


    return 0;
}
