//
// Created by szymo on 15.04.2023.
//

#include "Tempt.h"


struct Connection {
    int timeToTravel;
};

struct City{
    char* name;
    Connection* connections;
};
