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

class dynamic_scheduler
{
	public:
		long rob_size;
		long iq_size;
		long width;
		long cache_size;
		long p;
		std::string tracefile;

		dynamic_scheduler(long r, long i, long w, long c, long P, std::string tf)
		{
			rob_size = r;
			iq_size = i;
			width = w;
			cache_size = c;
			p = P;
			tracefile = tf;
		}

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

class pipeline
{
	public:
		int *de;
		int *rn;
		int *rr;
		int *di;
		int *iq;
		int *ex_lst;
		int *wb;
		int *rob;
};

struct ROB
{
	int val;
	int dst;
	bool rdy;
	bool exc;
	bool mis;
	std::string pc;
};

struct RMT
{
	int val;
	std::string tag;
};

struct ARF
{
	int val;
};

struct IQ
{
	bool val;
	std::string dst_tag;
	bool s1_rdy;
	std::string s1_val;
	bool s2_rdy;
	std::string s2_val;
};