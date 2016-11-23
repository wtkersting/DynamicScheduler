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

	for (int i = 0; i < width; i++)
	{
		fe[i].full = false;
		de[i].full = false;
		rn[i].full = false;
		rr[i].full = false;
		di[i].full = false;
	}

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

}

void dynamic_scheduler::dispatch()
{
	for (int i = 0; i < width; i++)
	{
		if (di[i].full)
		{

		}
	}
}

void dynamic_scheduler::regRead()
{
	for (int i = 0; i < width; i++)
	{
		if (rr[i].full)
		{
			// If di is empty
			if (!di[i].full)
			{
				// Read the values from the ARF
				di[i] = rr[i];
				rr[i].full = false;
			}
			else {} // If di is full, do nothing
		}
	}
}

void dynamic_scheduler::rename()
{
	for (int i = 0; i < width; i++)
	{
		if (rn[i].full)
		{
			//TODO: isRobFull implementation
			if (!rr[i].full) // || this->isRobFull())
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
		else {} // Do nothing
	}
}

void dynamic_scheduler::decode()
{
	if (!isBndlEmpty(rn) && isBndlFull(de))
	{
		for (int i = 0; i < width; i++)
		{
			if (de[i].full)
			{
				rn[i] = de[i];
				de[i].full = false;
			}
			else {} // Do nothing
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

//TODO: isRobFull()
bool dynamic_scheduler::isRobFull()
{
	return false;
}

// Returns falls unless every instruction is empty in the bundle.
bool dynamic_scheduler::isBndlEmpty(vector<bndl> b)
{
	for (int i = 0; i < b.size(); i++)
		if (b[i].full)
			return false;

	return true;
}

// Returns false unless every instruction is full in the bundle.
bool dynamic_scheduler::isBndlFull(vector<bndl> b)
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
