#include "ds.h"

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