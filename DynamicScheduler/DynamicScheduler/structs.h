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
	ROB() : val(0), dst(-1), rdy(false), exc(false), mis(false), pc(""), i(-1) {}

	int val;
	int dst;
	bool rdy;
	bool exc;
	bool mis;
	std::string pc;

	long i;
};

extern struct RMT
{
	RMT() : val(false), tag(-1) {}

	bool val;
	int tag;
};

extern struct IQ
{
	IQ() : age(-1), op(-1), val(false), dst_tag(-1), s1_rdy(false), s1_val(-1), s2_rdy(false), s2_val(-1), i(-1), log(false) {}

	long age;
	int op;
	bool val;
	int dst_tag;
	bool s1_rdy;
	int s1_val;
	bool s2_rdy;
	int s2_val;

	long i;
	bool log;
};

extern struct instr
{
	instr() : pc(""), op(-1), dst(-1), src1(-1), s1rob(false), s1rdy(false), src2(-1), s2rob(false), s2rdy(false),
		full(false), i(-1) {}

	std::string pc;
	int op;
	int dst;
	int src1;
	bool s1rob;
	bool s1rdy;
	int src2;
	bool s2rob;
	bool s2rdy;
	bool full;
	long i;
};

extern struct EL
{
	EL() : val(false), dst(-1), src1(-1), src2(-1), timer(0), i(-1) {}

	bool val;
	int dst;
	int src1;
	int src2;
	int timer;

	long i;
};

// Just a basic struct for formatting the output
extern struct OUT
{
	OUT() : ind(0), fu(-1), src1(-1), src2(-1), fe_beg(-1), fe_dur(-1), de_beg(-1), de_dur(-1), rn_beg(-1),
		rn_dur(-1), rr_beg(-1), rr_dur(-1), di_beg(-1), di_dur(-1), is_beg(-1), is_dur(-1), ex_beg(-1),
		ex_dur(-1), wb_beg(-1), wb_dur(-1), rt_beg(-1), rt_dur(-1), de_ack(false), rn_ack(false) {}

	long ind;
	int fu;
	int src1;
	int src2;
	int dst;

	// Beginings and durations for each pipeline stage
	long fe_beg;
	int fe_dur;
	long de_beg;
	int de_dur;
	long rn_beg;
	int rn_dur;
	long rr_beg;
	int rr_dur;
	long di_beg;
	int di_dur;
	long is_beg;
	int is_dur;
	long ex_beg;
	int ex_dur;
	long wb_beg;
	int wb_dur;
	long rt_beg;
	int rt_dur;

	bool de_ack;
	bool rn_ack;
};
