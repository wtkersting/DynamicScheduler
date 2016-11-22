#include "ds.h"
#include "utils.h"
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

using namespace std;

void do_ds(dynamic_scheduler ds);

/**
	ROB_SIZE:	16
	Number of entries in the ReOrder Buffer (ROB).

	IQ_SIZE:	8
	This is the number of entries in the Issue Queue (IQ).

	WIDTH:		1
	This is the superscalar width of all pipeline stages, in terms of the maximum number of instructions
	in each pipeline stage.  The one exception is Writeback: the number of instructions that may 
	complete execution in a given cycle is not limited to WIDTH

	CACHE_SIZE:	0
	This is the instruction cache size in bytes.

	P:			0
	This is the prefetching flag indicating whether the next-line prefetching is enabled.

	tracefile:	val_trace_gcc1
	This is the tracefile used in our program.
*/


//	Arguments: ROB_SIZE IQ_SIZE WIDTH CACHE_SIZE P tracefile
int main(int args, char* argv[])
{
	dynamic_scheduler ds(STOI(argv[1]), STOI(argv[2]), STOI(argv[3]), STOI(argv[4]), STOI(argv[5]), argv[6]);

	do_ds(ds);

	return 1;
}

void do_ds(dynamic_scheduler ds)
{
	std::string tf = ds.getTF();

	cout << ds.getRobSize() << endl;
	cout << ds.getIQSize() << endl;
	cout << ds.getWidth() << endl;
	cout << ds.getCacheSize() << endl;
	cout << ds.getP() << endl;
	cout << ds.getTF() << endl;

	vector<IQ> iq(ds.getIQSize());
	vector<ROB> rob(ds.getRobSize());
	vector<ARF> r(67);		// # of Architectual Registers
	vector<RMT> rmt(67);	// # of Architectual Registers

	std::ifstream infile(tf.c_str());
	std::string line;
	long cycle = 0;
	vector<std::string> splt_str;
	std::string pc;
	int op;
	int dst;
	int src1;
	int src2;

	while (std::getline(infile, line))
	{
		splt_str = split(line);
		pc = hexToBin(splt_str[0]);
		op = STOI(splt_str[1].c_str());
		dst = STOI(splt_str[2].c_str());
		src1 = STOI(splt_str[3].c_str());
		src2 = STOI(splt_str[4].c_str());

		cout << pc << "\tfu{" << op << "}\tsrc{" << src1 << "," <<src2 <<"}\tdst{" << dst << "}\tFE{" << cycle << "," << ds.getWidth() << "}\t" << endl;
		cycle++;
	}
}

int getLatency(int operation)
{
	switch (operation)
	{
	default:
		cout << "Operation " << operation << " not recognized";
		return -1;
		break;
	case 0:
		return 1;	// latency = 1 cycle for op0
		break;
	case 1:
		return 2;	// latency = 2 cycles for op1
		break;
	case 2:
		return 5;	// latency = 5 cycles for op2
		break;
	}
}