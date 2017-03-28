//
//  config.h
//
//
//  Created by Pascal Poncelet on 08/07/2015.
//
//

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

class Config{
    public:
    static bool DEBUG_PERSO;
    static bool DEBUG;
    static bool FULLDEBUG;
    static std::string defaultFile;

    Config();
    ~Config();

};


#endif
