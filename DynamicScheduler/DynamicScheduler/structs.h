// This file contains all structs used in this project
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <vector>
#include <math.h>

extern struct ROB
{
	int val;
	int dst;
	bool rdy;
	bool exc;
	bool mis;
	std::string pc;
};

extern struct RMT
{
	bool val;
	int tag;
};

extern struct IQ
{
	bool val;
	std::string dst_tag;
	bool s1_rdy;
	std::string s1_val;
	bool s2_rdy;
	std::string s2_val;
};

extern struct bndl
{
	std::string pc;
	int op;
	int dst;
	int src1;
	int src2;
	bool full;
};
