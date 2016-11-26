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
	vector<RMT> rmt;	// # of Architectual Registers
	vector<EL> el;		// execution list

	int h, t;	// Head and tail pointers for ROB
	long prnt;
	bool NO_ROOM;

	std::ifstream _infile;
	std::string _line;

	vector<std::string> _splt_str;
	std::string _pc;

	vector<instr> fe;
	vector<instr> de;
	vector<instr> rn;
	vector<instr> rr;
	vector<instr> di;

	vector<instr> wb;

	int _pl;	// # instructions in the pipeline
	long i_count; // number of instructions

	void exWakeup(int dst);

	bool isEnoughRob();
	bool isEnoughIQ();
	bool isIq();
	bool isIqRdy();
	int getRdyIq();
	bool exRdy(int op);
	int getLatency(int op);
	bool isBndlEmpty(vector<instr> b);
	bool isBndlFull(vector<instr> b);

public:
	long rob_size, iq_size, width, cache_size, p;
	std::string tracefile;

	vector<OUT> out;

	long cycle;

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

