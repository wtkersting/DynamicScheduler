#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <bitset>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iterator>

std::string hexToBin(std::string addr)
{
	std::string binAddr = "";
	for (int i = 0; i < addr.length(); ++i)
	{
		switch (addr [i])
		{
			case '0': binAddr.append ("0000"); break;
			case '1': binAddr.append ("0001"); break;
			case '2': binAddr.append ("0010"); break;
			case '3': binAddr.append ("0011"); break;
			case '4': binAddr.append ("0100"); break;
			case '5': binAddr.append ("0101"); break;
			case '6': binAddr.append ("0110"); break;
			case '7': binAddr.append ("0111"); break;
			case '8': binAddr.append ("1000"); break;
			case '9': binAddr.append ("1001"); break;
			case 'a': binAddr.append ("1010"); break;
			case 'b': binAddr.append ("1011"); break;
			case 'c': binAddr.append ("1100"); break;
			case 'd': binAddr.append ("1101"); break;
			case 'e': binAddr.append ("1110"); break;
			case 'f': binAddr.append ("1111"); break;
		}
	}

	if (binAddr.length() < 32)
		binAddr.insert(0, 32 - binAddr.length(), '0');
	else if (binAddr.length() < 64 && binAddr.length() > 32)
		binAddr.insert(0, 64 - binAddr.length(), '0');

	//cout << binAddr << endl;

	return binAddr;
}

int STOI(const char *s)
{
    int i = 0;
	bool neg = false;
	
	if (*s == '-')
	{
		neg = true;
		s++;
	}

    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }

	if (neg)
		return i*-1;
	else
		return i;
}

std::string BTOH(std::string bin)
{
	while (bin[0] == '0')
		bin = bin.substr(1);

	while (bin.length() % 4)
		bin = '0' + bin;

	std::string rest("0x"),tmp = "0000";
	int len = bin.length()/4;
	for (int i=0; i<bin.length(); i+=4)
	{
		tmp = bin.substr(i,4);

		if (!tmp.compare("0000"))
		{
			rest = rest + "0";
		}
		else if (!tmp.compare("0001"))
		{
			rest = rest + "1";
		}
		else if (!tmp.compare("0010"))
		{
			rest = rest + "2";
		}
		else if (!tmp.compare("0011"))
		{
			rest = rest + "3";
		}
		else if (!tmp.compare("0100"))
		{
			rest = rest + "4";
		}
		else if (!tmp.compare("0101"))
		{
			rest = rest + "5";
		}
		else if (!tmp.compare("0110"))
		{
			rest = rest + "6";
		}
		else if (!tmp.compare("0111"))
		{
			rest = rest + "7";
		}
		else if (!tmp.compare("1000"))
		{
			rest = rest + "8";
		}
		else if (!tmp.compare("1001"))
		{
			rest = rest + "9";
		}
		else if (!tmp.compare("1010"))
		{
			rest = rest + "a";
		}
		else if (!tmp.compare("1011"))
		{
			rest = rest + "b";
		}
		else if (!tmp.compare("1100"))
		{
			rest = rest + "c";
		}
		else if (!tmp.compare("1101"))
		{
			rest = rest + "d";
		}
		else if (!tmp.compare("1110"))
		{
			rest = rest + "e";
		}
		else if (!tmp.compare("1111"))
		{
			rest = rest + "f";
		}
		else
		{
			continue;
		}
	}
	return rest;
}

long BTOI(std::string str) 
{
	long integer = 0;
	long size = str.length() - 1;
	long count = 0;

	for (int i = 0; i < str.length(); i++)
	{
		if ( str.at(i) == '1' ) 
			integer = integer + pow(2, size - count);
	
		count++; 
	}

	return integer;
}

// Takes 2 binary strings of the same length and returns a binary string of same length that is the two strings xor'd
std::string x_or(std::string s1, std::string s2)
{
	std::string s3;

	for (int i = 0; i < s1.length(); i++)
	{
		if (s1.at(i) == s2.at(i))
			s3 += '0';
		else
			s3 += '1';
	}

	return s3;
}

double hundredths(double a)
{
	return floor(100* a + .5) / 100;
}

std::vector<std::string> split(std::string str)
{
	istringstream iss(str);
	vector<string> tokens;
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(tokens));
    return tokens;
}

// Takes a vector of bundles, returns index of empty bundle.
// If no empty index, returns -1
int emptyIndx(std::vector<bndl> b)
{
	for (int i = 0; i < b.size(); i++)
		if (!b[i].full)
			return i;

	return -1;
}
