#include "ds.h"
#include "utils.h"

void dynamic_scheduler::retire()
{

}

void dynamic_scheduler::writeback()
{

}

void dynamic_scheduler::execute()
{

}

void dynamic_scheduler::issue()
{

}

void dynamic_scheduler::regWrite()
{

}

void dynamic_scheduler::rename()
{

}

void dynamic_scheduler::decode()
{

}

void dynamic_scheduler::fetch()
{
	getline(this->infile, this->line);
}

bool dynamic_scheduler::advance_cycle()
{
	cout << pc << "\tfu{" << op << "}\tsrc{" << src1 << "," << src2 << "}\tdst{" << dst << "}\tFE{" << cycle << "," << width << "}\t" << endl;
	cycle++;

	if (getline(this->infile, this->line))
		return true;
	else
		return false;
}

void dynamic_scheduler::initialize()
{
	this->splt_str = split(line);
	this->pc = hexToBin(splt_str[0]);
	this->op = STOI(splt_str[1].c_str());
	this->dst = STOI(splt_str[2].c_str());
	this->src1 = STOI(splt_str[3].c_str());
	this->src2 = STOI(splt_str[4].c_str());
}

void dynamic_scheduler::setRobSize(long rob)
{
	rob_size = rob;
}
void dynamic_scheduler::setIQSize(long iq)
{
	iq_size = iq;
}
void dynamic_scheduler::setWidth(long w)
{
	width = w;
}
void dynamic_scheduler::setCacheSize(long c)
{
	cache_size = c;
}
void dynamic_scheduler::setP(long P)
{
	p = P;
}
void dynamic_scheduler::setTF(std::string tf)
{
	tracefile = tf;
}
long dynamic_scheduler::getRobSize()
{
	return rob_size;
}
long dynamic_scheduler::getIQSize()
{
	return iq_size;
}
long dynamic_scheduler::getWidth()
{
	return width;
}
long dynamic_scheduler::getCacheSize()
{
	return cache_size;
}
long dynamic_scheduler::getP()
{
	return p;
}
std::string dynamic_scheduler::getTF()
{
	return tracefile;
}
