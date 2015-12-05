#ifndef WRAPPER_H
#define WRAPPER_H
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>


using namespace std;

class wrapper{
public:
	wrapper();
	~wrapper();
	void pack(string name);
	void pack(string name, uint8_t* data, int dataSize);
	void unpack(uint8_t* data, int& size);
	uint8_t* sendBuf;
	int sendBufSize;
	pair<string ,pair< int, void*> > operator[](int);
	 size_t getSize();
	void add(string, string);
	vector< pair<string ,pair< int, uint8_t*> > > command;
private:
	int realSize;
	
	map<string, string > name_set;
	struct DataCont{
		DataCont();
		~DataCont();
		void take(uint8_t* data, int comSize);
		int find(string search);
		bool empty();
		bool erase(int shift);
		string getConstStr(int start, int end);
		int realSize;
		int size;
		uint8_t* current;
	} dataCont;
	bool find(int&, int&, DataCont&, int&);

	//vector<uint8_t>* sendBuf;
};

#endif // WRAPPER_H
