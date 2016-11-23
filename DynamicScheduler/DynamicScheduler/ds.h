#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <vector>
#include <math.h>

#include "structs.h"

using namespace std;

class dynamic_scheduler
{
private:
	vector<IQ> iq;		// Issue Queue
	vector<ROB> rob;	// ReOrder Buffer
	vector<int> r;		// # of Architectual Registers - ARF
	vector<RMT> rmt;	// # of Architectual Registers

	int h, t;	// Head and tail pointers for ROB

	std::ifstream _infile;
	std::string _line;

	vector<std::string> _splt_str;
	std::string _pc;

	vector<bndl> fe;
	vector<bndl> de;
	vector<bndl> rn;
	vector<bndl> rr;
	vector<bndl> di;
	vector<bndl> is;
	vector<bndl> ex;
	vector<bndl> wb;
	vector<bndl> rt;

	int _pl;	// # instructions in the pipeline

	bool isRobFull();
	bool isBndlEmpty(vector<bndl> b);
	bool isBndlFull(vector<bndl> b);

public:
	long rob_size, iq_size, width, cache_size, p;
	std::string tracefile;

	long _cycle;

	dynamic_scheduler(long r, long i, long w, long c, long P, std::string tf);

	void retire();
	void writeback();
	void execute();
	void issue();
	void dispatch();
	void regRead();
	void rename();
	void decode();
	void fetch();

	bool advance_cycle();

	void setRobSize(long rob);
	void setIQSize(long iq);
	void setWidth(long w);
	void setCacheSize(long c);
	void setP(long P);
	void setTF(std::string tf);

	long getRobSize(void);
	long getIQSize(void);
	long getWidth(void);
	long getCacheSize(void);
	long getP(void);
	std::string getTF(void);
};

