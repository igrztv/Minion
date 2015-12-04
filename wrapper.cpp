#include "wrapper.h"

wrapper::wrapper(): sendBufSize(0), sendBuf(NULL), realSize(0){}

void wrapper::add(string name, string value){
    name_set[name] = value;
}

wrapper::~wrapper(){
    delete[] sendBuf;
}

void wrapper::pack(string name, uint8_t* data, int dataSize){ 
    //формирование открывающего тега и длины данных
    string start = name_set[name];
    stringstream ss;
    ss << dataSize;
    string length = " " + ss.str();
    start.insert(start.length() - 1, length);
    int startLen = start.size();
    if (realSize < dataSize + startLen){
        delete[] sendBuf;
        sendBuf = new uint8_t[dataSize + startLen + 10];
        if (sendBuf != 0){
            realSize = dataSize + startLen + 10;
            cout << "realloc to realSize: " << realSize << endl;
        }else{
			cout << "realloc failed!\n" << endl;
            realSize = 0;
            sendBufSize = 0;
            return;
        }
    }
    sendBufSize = dataSize + startLen;
    memcpy(sendBuf, start.c_str(), startLen);
    memcpy(sendBuf + startLen, data, dataSize);
}

void wrapper::pack(string name){

    return ;
}

int wrapper::DataCont::find(string search){
    int searchSize = search.size();
    for(int i = 0; i < size - searchSize + 1; i++){
        bool find = true;
        char one = ((char*)current)[i];
        char tow = search[0];
        if(((char*)current)[i] == search[0]){
            for (int j = 0; j < search.size(); j++){
                if ( search[j] != current[i + j]){
                    find = false;
                    break;
                }
            }
            if( find ){
                return i;
            }
        }
    }
    return string::npos;
}

bool wrapper::find(int& posStart, int& posEnd, DataCont& buf, int& data){
    posStart = buf.find("<");
    if(posStart == string::npos){
        return false;
    }
    posEnd = buf.find(">");
    if(posEnd == string::npos){
        return false;
    }
    if (posStart != 0){
        buf.erase(posStart);
        posStart = 0;
    }
    data += posEnd + 1;
    return true;
}

bool wrapper::DataCont::erase(int shift){
    if (shift < size ){
        current = current + shift;
        size -= shift;
        return true;
    }
    return false;
}

wrapper::DataCont::DataCont(){
    current = NULL;
    size = 0;
    realSize = 0;
}

wrapper::DataCont::~DataCont(){}

void wrapper::DataCont::take(uint8_t* data, int comSize){
    size = comSize;
    realSize = comSize;
    current = data;
}

bool wrapper::DataCont::empty(){
    return !size ? true : false;
}

string wrapper::DataCont::getConstStr(int start, int end){
    if (end < start){
        return NULL;
    }
    string buf = "";
    for (int i = start; i < end - start; i++){
        buf += (char)(((char*)current)[i]);
    }
    return buf;
}

void wrapper::unpack(uint8_t* pack, int& size){
    command.clear();
    dataCont.take((uint8_t*)pack, size);
    int pos = 0;
    int positionStart = 0;
    int positionEnd = 0;
    while(!dataCont.empty()){
        if (!wrapper::find(positionStart, positionEnd, dataCont, pos)){
            return;
        }
        string comm(dataCont.getConstStr(0, positionEnd + 1));
        int posLength = 0;
        int length = 0;
        //если есть пробел означающий наличие данных
        if ((posLength = comm.find(" ")) != string::npos){
            //получаем длину данных
            length = atoi(string(comm.begin() + posLength + 1, comm.end() - 1).c_str());
            //проверка, что все данные поместились в пакет
            if (length + comm.size() > size){
                return;
            }
            //убираем пробел и числа
            comm.erase(comm.begin() + posLength, comm.end() - 1);
        }
        //смещаем указатель на позицию окончания данных
        if(!dataCont.erase(positionEnd + length))
			return;

        pair<string, pair< int, uint8_t* > > value;
        value.first = "";
        value.second.first = 0;
        value.second.second = NULL;
        //выбираем соответствующую команду и записываем ее в массив команд
        for (map<string, string >::iterator i = name_set.begin();i != name_set.end(); i++){
            //если комманда совпала
            if (i->second == comm){
                value.first = i->first;
                value.second.first = length;
                value.second.second = pack + pos;
            }
        }
        if(value.first != ""){
            command.push_back(value);
            cout << command.back().first << " size: " << command.back().second.first << endl;
        }

//        cout << comm << " " << index << endl;
//        index++;
//        for (map<string, vector<string> >::iterator i = name_set.begin();i != name_set.end(); i++){
//            pair<string, pair< int, uint8_t* > > value;
//            value.second.first = 0;
//            value.second.second = NULL;
//            if (i->second[0] == comm){
//                value.first = comm;
//                value.second.first = 0;
//                value.second.second = pack + pos;
//                dataCont.erase(dataCont.begin(), dataCont.begin() + positionEnd + 1);
//                if (i->second.size() == 2){
//                    int bufSize = pos;
//                    if(!wrapper::find(positionStart,positionEnd, buf,pos)){
//                        return;
//                    }
//                    comm = string(buf.begin(), buf.begin() + positionEnd + 1);
//                    if(comm == i->second[1]){
//                        value.second.first = pos - positionEnd - bufSize;
//                        buf.erase(buf.begin(), buf.begin() + positionEnd + 1);
//                    }
//                    else{
//                        value.first = "";
//                    }
//                }
//                if(value.first != ""){
//                    command.push_back(value);
//                }
//            }
//            else{
//                string newStr = string(dataCont.begin(), dataCont.begin() + positionEnd);
//                cout << dataCont.size << endl;
//                cout << "мимо " << string(dataCont.begin(), dataCont.begin() + positionEnd) << endl;
//                buf.erase(buf.begin(), buf.begin() + positionEnd);
//            }
//        }

    }
    return;
}

