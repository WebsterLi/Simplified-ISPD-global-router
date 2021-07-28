#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <fstream>
#include "routingdb.h"

using namespace std;


class Parser
{
    public:
	Parser() {;}
	~Parser() {;}

	void ReadISPD(ifstream& file);

    private:
};

#endif
