#include "ds.h"
#include "utils.h"

dynamic_scheduler::dynamic_scheduler(long r, long i, long w, long c, long P, std::string tf)
{
	rob_size = r;
	iq_size = i;
	width = w;
	cache_size = c;
	p = P;
	tracefile = tf;

	h = 3;
	t = 3;

	NO_ROOM = false;

	i_count = 0;

	_infile.open(tf);

	this->iq.resize(iq_size);
	this->rob.resize(rob_size);
	this->rmt.resize(67);	// 67 = # of registers

	this->fe.resize(width);
	this->de.resize(width);
	this->rn.resize(width);
	this->rr.resize(width);
	this->di.resize(width);

	this->el.resize(width*5);	// up to width * 5 executions at any given time
	this->wb.resize(width*5);	// Enough to accept any number of finished executions

	// Initialize # of instructions in pipeline to 0 and cycle to 0
	_pl = 0;
	cycle = 0;
	prnt = 0;
}

// OOO
void dynamic_scheduler::retire()
{
	if (rob[h].rdy)
	{
		for (int i = 0; i < width; i++)
		{
			// Remove from RMT if there was a destination
			if (rob[h].dst != -1 && h == rmt[rob[h].dst].tag)
			{
				rmt[rob[h].dst].val = false;
			}

			// Remove from RoB
			rob[h].rdy = false;
			rob[h].dst = 0;
			rob[h].pc = "";

			out[rob[h].i].rt_dur = cycle - out[rob[h].i].rt_beg + 1;	// Add one

			_pl--;

			if (h == rob_size - 1)
				h = 0;
			else
				h++;

			NO_ROOM = false;
		}
	}
}

// OOO
void dynamic_scheduler::writeback()
{
	// If the wb bundle is not empty
	if (!isBndlEmpty(wb))
	{	
		for (int i = 0; i < wb.size(); i++)
		{
			if (wb[i].full)
			{
				if (wb[i].dst != -1)
				{
					rob[wb[i].dst].rdy = true;
				}

				out[wb[i].i].ex_dur = cycle - out[wb[i].i].ex_beg;
				out[wb[i].i].wb_beg = cycle;
				out[wb[i].i].wb_dur = 1;	// WB duration is ALWAYS 1
				out[wb[i].i].rt_beg = cycle + 1; //RT always starts the cycle after WB

				wb[i].full = false;
				wb[i].i = -1;
			}
		}
	}
}

// OOO
void dynamic_scheduler::execute()
{
	for (int i = 0; i < el.size(); i++)
	{
		if (el[i].val)
		{
			if (out[el[i].i].ex_beg == -1)
			{
				out[el[i].i].is_dur = cycle - out[el[i].i].is_beg;
				out[el[i].i].ex_beg = cycle;
			}

			if (--el[i].timer <= 0)
			{
				// Remove from execution list
				el[i].val = false;

				// Add to wb
				wb[i].full = true;
				wb[i].dst = el[i].dst;
				wb[i].src1 = el[i].src1;
				wb[i].src2 = el[i].src2;

				wb[i].i = el[i].i;

				// Wakeup dependent instructions if there is a dst
				if (el[i].dst != -1)
					exWakeup(el[i].dst);
			}
		}
	}
}

// OOO
void dynamic_scheduler::issue()
{
	if (isIq())
	{
		for (int i = 0; i < iq_size; i++)
		{
			if (iq[i].val && !iq[i].log)
			{
				out[iq[i].i].di_dur = cycle - out[iq[i].i].di_beg;
				out[iq[i].i].is_beg = cycle;

				iq[i].log = true;
				break;
			}
		}
		
		for (int i = 0; i < width; i++)
		{
			int index, oldest;
			
			if (isIqRdy())
			{
				index = getRdyIq();
				oldest = iq[getRdyIq()].age; //iq[0].age;

				// Find the oldest instruction from the IQ (that is ready
				for (int j = 0; j < iq_size; j++)
				{
					if (iq[j].age >= 0 && iq[j].age < oldest && iq[j].val && iq[j].s1_rdy && iq[j].s2_rdy)
					{
						oldest = iq[j].age;
						index = j;
					}
				}

				for (int j = 0; j < 5*width; j++)
				{
					if (!el[j].val)
					{
						// Add instruction to execution list
						el[j].val = true;
						el[j].src1 = iq[index].s1_val;
						el[j].src2 = iq[index].s2_val;
						el[j].dst = iq[index].dst_tag;
						el[j].timer = getLatency(iq[index].op);

						// Remove instruction from IQ
						iq[index].s1_rdy = false;
						iq[index].s2_rdy = false;
						iq[index].val = false;
						iq[index].log = false;

						// Keep track of the instruction
						el[j].i = iq[index].i;

						//out[iq[index].i].di_dur = cycle - out[iq[index].i].di_beg;
						//out[iq[index].i].is_beg = cycle;

						break;	// break after filling out one entry in the execution list
					}
				}
			}	
		}
	}
}

void dynamic_scheduler::dispatch()
{
	// If the dispatch bundle is full and there is enough room in the Issue Queue
	if (!isBndlEmpty(di) && isEnoughIQ())
	{
		int j = 0;

		for (int i = 0; i < width; i++)
		{
			// Find first <width> empty spots in the IQ
			for (j; j < iq_size; j++)
			{
				if (!iq[j].val)
				{
					iq[j].val = true;
					iq[j].op = di[i].op;

					iq[j].dst_tag = di[i].dst;
					iq[j].s1_rdy = di[i].s1rdy;
					iq[j].s1_val = di[i].src1;
					iq[j].s2_rdy = di[i].s2rdy;
					iq[j].s2_val = di[i].src2;

					// Set an age to be used later
					iq[j].age = cycle;

					// Keep track of the instruction
					iq[j].i = di[i].i;
					
					out[di[i].i].rr_dur = cycle - out[di[i].i].rr_beg;
					out[di[i].i].di_beg = cycle; 

					// Empty the di instruction
					di[i].full = false;
					break;
				}
			}
		}
	}
}

void dynamic_scheduler::regRead()
{
	if (isBndlEmpty(di) && !isBndlEmpty(rr))
	{
		for (int i = 0; i < width; i++)
		{
			// If the source is directly from the ARF, the source is ready.
			if (!rr[i].s1rob)
				rr[i].s1rdy = true;
			else if (rob[rr[i].src1].rdy)	// Else, if the ROB is ready, this source is ready too
				rr[i].s1rdy = true;

			if (!rr[i].s2rob)
				rr[i].s2rdy = true;
			else if (rob[rr[i].src2].rdy)
				rr[i].s2rdy = true;
			
			out[rr[i].i].rn_dur = cycle - out[rr[i].i].rn_beg;
			out[rr[i].i].rr_beg = cycle;

			di[i] = rr[i];
			rr[i].full = false;
		}
	}
}

void dynamic_scheduler::rename()
{
	if (isBndlEmpty(rr) && !isBndlEmpty(rn) && isEnoughRob())
	{
		for (int i = 0; i < width; i++)
		{
			// Rename the source and destination registers - be sure to rename sources before destination
			if (rn[i].src1 != -1)	// If there is a source 1
			{
				if (rmt[rn[i].src1].val)	// If the source has been renamed, get the rob name
				{
					rn[i].src1 = rmt[rn[i].src1].tag;
					rn[i].s1rob = true;		// Set this flag so that we know we have to wait until it is ready
					rn[i].s1rdy = rob[rn[i].src1].rdy;
				}
				else {} // the src remains the ARF register						
			}

			if (rn[i].src2 != -1)	// If there is a source 2
			{
				if (rmt[rn[i].src2].val)	// If the source has been renamed, get the rob name
				{
					rn[i].src2 = rmt[rn[i].src2].tag;
					rn[i].s2rob = true;		// Set this flag so that we know we have to wait until it is ready
					rn[i].s2rdy = rob[rn[i].src2].rdy;
				}
				else {} // the src remains the ARF register
			}


			if (rn[i].dst != -1)	// If there is a destination
			{
				rmt[rn[i].dst].val = true;	// v = true
				rmt[rn[i].dst].tag = t;	// tag = the current tail pointer for the ROB
			}

			rob[t].val = 0;			// Clear the value, we don't care about this yet
			rob[t].dst = rn[i].dst;
			rob[t].rdy = 0;
			rob[t].exc = 0;
			rob[t].mis = 0;
			rob[t].pc = rn[i].pc;	

			rob[t].i = rn[i].i;

			//if (rn[i].dst != -1)	// TODO: Check this out
			rn[i].dst = t;		// Dst, if it exists, always gets renamed

			/*if (t != h-1)
			{
				// If we reach the end of the ROB, go back to index 0 --- Circular FIFO
				if (t == rob_size - 1)
					t = 0;
				else
					t++;
			}
			else
			{
				//cout << "Not good" << endl;
				NO_ROOM = true;
			} */

			if (t == h-1 || (t == rob_size - 1 && h == 0))
				NO_ROOM = true;

			// If we reach the end of the ROB, go back to index 0 --- Circular FIFO
			if (t == rob_size - 1)
				t = 0;
			else
				t++;

			out[rn[i].i].de_dur = cycle - out[rn[i].i].de_beg;
			out[rn[i].i].rn_beg = cycle;

			rr[i] = rn[i];
			rn[i].full = false;
		}
	}
}

void dynamic_scheduler::decode()
{
	if (isBndlEmpty(rn) && !isBndlEmpty(de))
	{
		for (int i = 0; i < width; i++)
		{
			out[de[i].i].fe_dur = cycle - out[de[i].i].fe_beg;
			out[de[i].i].de_beg = cycle;

			rn[i] = de[i];
			de[i].full = false;
		}
	}
	else {} // Do nothing
}

void dynamic_scheduler::fetch()
{
	if (!_infile.eof() && isBndlEmpty(de))
	{
		// If there are more instructions and de is not empty
		for (int i = 0; i < width; i++)
		{
			getline(_infile, _line);
			if (_line.length() > 1)
			{
				_splt_str = split(_line);
				fe[i].pc = hexToBin(_splt_str[0]);
				fe[i].op = STOI(_splt_str[1].c_str());
				fe[i].dst = STOI(_splt_str[2].c_str());
				fe[i].src1 = STOI(_splt_str[3].c_str());
				fe[i].src2 = STOI(_splt_str[4].c_str());
				fe[i].full = true;
				fe[i].i = i_count;

				OUT n;
				n.ind = i_count++;
				n.fe_beg = cycle;
				n.src1 = fe[i].src1;
				n.src2 = fe[i].src2;
				n.dst = fe[i].dst;
				n.fu = fe[i].op;

				out.push_back(n);

				de[i] = fe[i];
				_pl++;	// Instruction is in the pipeline
			}
		}
	}
	else {} // Do nothing
}

bool dynamic_scheduler::advance_cycle()
{
	//for (int i = 0; i < fe.size(); i ++)
	//	cout << cycle << "\tfu{" << fe[i].op << "}\tsrc{" << fe[i].src1 << "," << fe[i].src2 << "}\tdst{" << fe[i].dst << "}\tFE{" << cycle << "," << width << "}\t" << endl;
	
	int i = prnt;
	if (out[i].fe_dur != -1 && out[i].de_dur != -1 && out[i].rn_dur != -1 && out[i].rr_dur != -1 && out[i].di_dur != -1 && out[i].is_dur != -1 && out[i].ex_dur != -1 && out[i].wb_dur != -1 && out[i].rt_dur != -1)
	{
		cout << i << " fu{" << out[i].fu << "} src{" << out[i].src1 << "," << out[i].src2 << "} dst{" << out[i].dst 
			<< "} FE{" << out[i].fe_beg << "," << out[i].fe_dur 
			<< "} DE{" << out[i].de_beg << "," << out[i].de_dur
			<< "} RN{" << out[i].rn_beg << "," << out[i].rn_dur
			<< "} RR{" << out[i].rr_beg << "," << out[i].rr_dur
			<< "} DI{" << out[i].di_beg << "," << out[i].di_dur
			<< "} IS{" << out[i].is_beg << "," << out[i].is_dur
			<< "} EX{" << out[i].ex_beg << "," << out[i].ex_dur
			<< "} WB{" << out[i].wb_beg << "," << out[i].wb_dur
			<< "} RT{" << out[i].rt_beg << "," << out[i].rt_dur << "}" << endl;

		prnt++;
	}

	cycle++;

	if (cycle == 22)
	{
		int k = 1 + 2;
	}

	if (cycle == 1000)
		int k = 1 + 2;

	if (!_infile.eof() || _pl > 0)
		return true;
	else
	{
		for (int i = 0; i < out.size(); i++)
		{
			printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d}, RT{%d,%d}\n",
				i, out[i].fu, out[i].src1, out[i].src2, out[i].dst,
				out[i].fe_beg, out[i].fe_dur,
				out[i].de_beg, out[i].de_dur,
				out[i].rn_beg, out[i].rn_dur,
				out[i].rr_beg, out[i].rr_dur,
				out[i].di_beg, out[i].di_dur,
				out[i].is_beg, out[i].is_dur,
				out[i].ex_beg, out[i].ex_dur,
				out[i].wb_beg, out[i].wb_dur,
				out[i].rt_beg, out[i].rt_dur);
		}

		double o_size = out.size();
		double cycles = cycle;
		double IPC = o_size / cycles;

		printf("# === Simulator Command =========\n");
		printf("# ./sim_ds %d %d %d %d %d %s\n", rob_size, iq_size, width, cache_size, p, tracefile.c_str());//, iq_size, width, cache_size, p, tracefile);
		printf("# === Processor Configuration ===\n");
		printf("# ROB_SIZE 	= %d\n", rob_size);
		printf("# IQ_SIZE  	= %d\n", iq_size);
		printf("# WIDTH    	= %d\n", width);
		printf("# CACHE_SIZE 	= %d\n", cache_size);
		printf("# PREFETCHING	= %d\n", p);
		printf("# === Simulation Results ========\n");
		printf("# Dynamic Instruction Count      = %d\n", out.size());
		printf("# Cycles                         = %d\n", cycle);
		printf("# Instructions Per Cycle (IPC)   = ");
		cout << hundredths(IPC);

		return false;
	}
}

// Wakes up dependent instructions on given dst in IQ, DI, and RR
void dynamic_scheduler::exWakeup(int dst)
{
	for (int i = 0; i < iq_size; i++)
	{
		if (iq[i].s1_val == dst)
			iq[i].s1_rdy = true;
		if (iq[i].s2_val == dst)
			iq[i].s2_rdy = true;
	}

	for (int i = 0; i < width; i++)
	{
		if (di[i].s1rob && di[i].src1 == dst)
			di[i].s1rdy = true;
		if (di[i].s2rob && di[i].src2 == dst)
			di[i].s2rdy = true;

		if (rr[i].s1rob && rr[i].src1 == dst)
			rr[i].s1rdy = true;
		if (rr[i].s2rob && rr[i].src2 == dst)
			rr[i].s2rdy = true;
	}
}

// Determines if there is enough space in the RoB for the Bundle.
bool dynamic_scheduler::isEnoughRob()
{
	if (NO_ROOM)
		return false;

	if (h == t)
		if (rob_size >= width)
			return true;
		else 
			cout << "Not Good" << endl;

	if (h < t)
	{
		if (rob_size - (t - h) + 1 >= width)
			return true;
		else
			return false;
	}
	else
	{
		if (h - t >= width)
			return true;
		else 
			return false;
	}
}

// Returns true if there is enough room in the IQ for the bundle
bool dynamic_scheduler::isEnoughIQ()
{
	int j = 0;
	for (int i = 0; i < iq_size; i++)
	{
		if (!iq[i].val)
			if (++j >= width)
				return true;
	}

	return false;
}

// Returns true if there is a value in the IQ
bool dynamic_scheduler::isIq()
{
	for (int i = 0; i < iq_size; i++)
		if (iq[i].val)
			return true;

	return false;
}

// Returns true if there is a value in the IQ and its sources are ready
bool dynamic_scheduler::isIqRdy()
{
	for (int i = 0; i < iq_size; i++)
		if (iq[i].val && iq[i].s1_rdy && iq[i].s2_rdy)
			return true;

	return false;
}

// Returns the index of the first ready IQ --- returns -1 if none are ready
int dynamic_scheduler::getRdyIq()
{
	for (int i = 0; i < iq_size; i++)
		if (iq[i].val && iq[i].s1_rdy && iq[i].s2_rdy)
			return i;

	return -1;
}

// Returns the latency for a given operation
int dynamic_scheduler::getLatency(int op)
{
	switch (op)
	{
	case 0:
		return 1;
		break;
	case 1:
		return 2;
		break;
	case 2:
		return 5;
		break;
	}
}

// Returns falls unless every instruction is empty in the bundle.
bool dynamic_scheduler::isBndlEmpty(vector<instr> b)
{
	for (int i = 0; i < b.size(); i++)
		if (b[i].full)
			return false;

	return true;
}

// Returns false unless every instruction is full in the bundle.
bool dynamic_scheduler::isBndlFull(vector<instr> b)
{
	for (int i = 0; i < b.size(); i++)
		if (!b[i].full)
			return false;

	return true;
}

void dynamic_scheduler::setRobSize(long rob)
{
	this->rob_size = rob;
}
void dynamic_scheduler::setIQSize(long iq)
{
	this->iq_size = iq;
}
void dynamic_scheduler::setWidth(long w)
{
	this->width = w;
}
void dynamic_scheduler::setCacheSize(long c)
{
	this->cache_size = c;
}
void dynamic_scheduler::setP(long P)
{
	this->p = P;
}
void dynamic_scheduler::setTF(std::string tf)
{
	this->tracefile = tf;
}
long dynamic_scheduler::getRobSize()
{
	return this->rob_size;
}
long dynamic_scheduler::getIQSize()
{
	return this->iq_size;
}
long dynamic_scheduler::getWidth()
{
	return this->width;
}
long dynamic_scheduler::getCacheSize()
{
	return this->cache_size;
}
long dynamic_scheduler::getP()
{
	return this->p;
}
std::string dynamic_scheduler::getTF()
{
	return this->tracefile;
}
