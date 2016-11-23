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

	_infile.open(tf);

	this->iq.resize(iq_size);
	this->rob.resize(rob_size);
	this->r.resize(67);		// 67 = # of registers
	this->rmt.resize(67);	// 67 = # of registers

	// Initialize arf 0's
	for (int i = 0; i < this->r.size(); i++)
		this->r[i] = 0;

	this->fe.resize(width);
	this->de.resize(width);
	this->rn.resize(width);
	this->rr.resize(width);
	this->di.resize(width);

	this->el.resize(width*5);	// up to width * 5 executions at any given time

	for (int i = 0; i < width; i++)
	{
		fe[i].full = false;
		de[i].full = false;
		rn[i].full = false;
		rr[i].full = false;
		di[i].full = false;
	}

	for (int i = 0; i < iq_size; i++)
		iq[i].val = false;

	_pl = 0;
	_cycle = 0;
}

// OOO
void dynamic_scheduler::retire()
{

}

// OOO
void dynamic_scheduler::writeback()
{

}

// OOO
void dynamic_scheduler::execute()
{

}

// OOO
void dynamic_scheduler::issue()
{
	int index, oldest;

	for (int i = 0; i < width; i++)
	{
		index = 0;
		oldest = iq[0].age;

		// Find the oldest instruction from the IQ
		for (int j = 1; j < iq_size; j++)
		{
			if (iq[j].age < oldest)
				index = j;
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

				break;	// break after filling out one entry in the execution list
			}
		}
	}	
}

void dynamic_scheduler::dispatch()
{
	// If the dispatch bundle is full and there is enough room in the Issue Queue
	if (isBndlFull(di) && isEnoughIQ())
	{
		int j = 0;

		for (int i = 0; i < width; i++)
		{
			// Find first <width> empty spots in the IQ
			for (j; j < iq_size; j++)
			{
				if (iq[j].val = false)
				{
					iq[j].val = true;
					iq[j].op = di[i].op;

					iq[j].dst_tag = di[i].dst;
					iq[j].s1_rdy = true; // TODO: Make this work properly
					iq[j].s1_val = di[i].src1;
					iq[j].s2_rdy = true; // Same here ^
					iq[j].s2_val = di[i].src2;

					// Set an age to be used later
					iq[j].age = _cycle;
					
					// Empty the di instruction
					di[i].full = false;
				}
			}
		}
	}
}

void dynamic_scheduler::regRead()
{
	if (isBndlEmpty(di) && isBndlFull(rr))
	{
		for (int i = 0; i < width; i++)
		{
			// Read the values from the ARF
			di[i] = rr[i];
			rr[i].full = false;
		}
	}
}

void dynamic_scheduler::rename()
{
	if (isBndlEmpty(rr) && isBndlFull(rn) && isEnoughRob())
	{
		for (int i = 0; i < width; i++)
		{
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

			// Rename the source and destination registers
			if (rn[i].dst != -1)
				rn[i].dst = t;

			if (rn[i].src1 != -1)	// If there is a source 1
			{
				if (rmt[rn[i].src1].val)	// If the source has been renamed, get the rob name
					rn[i].src1 = rmt[rn[i].src1].tag;
				else {} // the src remains the ARF register						
			}

			if (rn[i].src2 != -1)	// If there is a source 2
			{
				if (rmt[rn[i].src2].val)	// If the source has been renamed, get the rob name
					rn[i].src2 = rmt[rn[i].src1].tag;
				else {} // the src remains the ARF register
			}

			// If we reach the end of the ROB, go back to index 0 --- Circular FIFO
			if (t == rob_size - 1)
				t = 0;
			else
				t++;

			rr[i] = rn[i];
			rn[i].full = false;
		}
	}
}

void dynamic_scheduler::decode()
{
	if (isBndlEmpty(rn) && isBndlFull(de))
	{
		for (int i = 0; i < width; i++)
		{
			rn[i] = de[i];
			de[i].full = false;
		}
	}
	else {} // Do nothing
}

void dynamic_scheduler::fetch()
{
	if (!_infile.eof() && !isBndlFull(de))
	{
		// If there are more instructions and de is not empty
		for (int i = 0; i < width; i++)
		{
			getline(_infile, _line);
			_splt_str = split(_line);
			fe[i].pc = hexToBin(_splt_str[0]);
			fe[i].op = STOI(_splt_str[1].c_str());
			fe[i].dst = STOI(_splt_str[2].c_str());
			fe[i].src1 = STOI(_splt_str[3].c_str());
			fe[i].src2 = STOI(_splt_str[4].c_str());
			fe[i].full = true;

			de[i] = fe[i];
			_pl++;	// Instruction is in the pipeline
		}
	}
	else {} // Do nothing
}

bool dynamic_scheduler::advance_cycle()
{
	for (int i = 0; i < fe.size(); i ++)
		cout << _cycle << "\tfu{" << fe[i].op << "}\tsrc{" << fe[i].src1 << "," << fe[i].src2 << "}\tdst{" << fe[i].dst << "}\tFE{" << _cycle << "," << width << "}\t" << endl;
	
	_cycle++;


	if (!_infile.eof() || _pl > 0)
		return true;
	else
		return false;
}

// Determines if there is enough space in the RoB for the Bundle.
bool dynamic_scheduler::isEnoughRob()
{
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
		if (iq[i].val = false)
			if (++j >= width)
				return true;
	}

	return false;
}

bool dynamic_scheduler::exRdy(int op)
{
	for (int i = 0; i < width; i++)
	{

	}
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
