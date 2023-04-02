// Name - A.S.Poornash
// Roll no. - 2101CS01
// Declaration of Authorship
// This cpp file, emu.cpp, is part of the assignment of CS210 at the 
// department of Computer Science and Engg, IIT Patna . 
#include <bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <cstdio> 
#include <queue>
#include <fstream>
#include <sstream>
#include <bitset>
#include <regex>
#define intt int32_t

using namespace std;
vector<string> pre_instr_dict = {"data", "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", 
"shl", "shr","adj", "a2sp","sp2a", "call","return", "brz", "brlz", "br", "HALT", "SET" };
vector<string> pre_label_dict;
map<int,int> pre_label_dict_map;
map<int,int> labels_used;
vector<string> read_instr;
vector<pair<int, int>> error_encountered;
vector <int> lines_for_set;
vector<string> for_listing_file;
vector<string> for_error_file;
vector<string> for_object_file;
int db = 0;
int halt_check = 0;
vector<int> seg_fault;
map<int, string> error_map = {
{0, "bogus instruction"},
{1, "invalid label format"},
{2, "duplicate label"},
{3, "unexpected extra operands/characters in the end of line"},
{4, "missing operand"},
{5, "unidentified label"},
{6, "not a number"},
};

vector<string> listing;
long long npn1, mpn1;

vector<int>no_operand_ind = {7,8,9,10,12,13,15,19};
vector<int>value_ind = {0,1,2,11,20};
vector<int>offset_ind = {3,4,5,6,14,16,17,18};

int find_int_in_vector(vector<int> a, int x){
    int ans = -1;
    for(int i = 0 ; i < a.size() ; i++){
        if(a[i] == x){
            ans = i;
        }
    }
    return ans;
}

int find_string_in_vector(vector<string> a, string x){
    int ans = -1;
    for(int i = 0 ; i < a.size() ; i++){
        if(a[i] == x){
            ans = i;
        }
    }
    return ans;
}
int is_alpha(char s){
    int y = s - 'A';
    int z = s - 'a';
    if((y>=0 && y<=25)|| (z>=0 && z<=25)){
        return 1;
    }
    return 0;
}

int is_alphanum(char s){
    int x = s - '0';
    int y = s - 'A';
    int z = s - 'a';
    if((x>=0 && x<=9)||(y>=0 && y<=25)|| (z>=0 && z<=25)||(s == '_')){
        return 1;
    }
    return 0;
}

vector<string> splitchar(string s, char b){
    stringstream test(s);
    string segment;
    vector<string> seglist;
    while(std::getline(test, segment, b)){
        seglist.push_back(segment);
    }
    return seglist;
}
int memory_store(int n){
    if(n==0){
        return 1;
    }
    if(n == 1){
        return 1;
    }
    return n*memory_store(n-1);
}
void hex_calc(){
    npn1 = 40960 ;mpn1 = 38901;
    long long x ,t = 0;
	vector<long long> v;
	v.push_back(1);
	for (int i = 0; i < mpn1; i++) {
		x = i+1;
		v.push_back(x);
	}
	for (auto it = v.begin(); it != v.end(); it++) {
		long long value = *it;
		auto itn = it;
		itn++;
		if (itn != v.end()) {
			long long nvalue = *itn;
			if (nvalue >= value) {
				t += (nvalue - value);
			}
			else {
				t += npn1 + nvalue - value ;
			}
		}
	}
}

intt SP=1<<23 ;
vector<intt> memory(16777216);
intt PC=0,A=0,B=0;

map<intt,string> mneumonics = {{0,"ldc"},{1,"adc"},{2,"ldl"},{3,"stl"},{4,"ldnl"},{5,"stnl"},{6,"add"},{7,"sub"},{8,"shl"},{9,"shr"},{10,"adj"},{11,"a2sp"},{12,"sp2a"},{13,"call"},{14,"return"},{15,"brz"},{16,"brlz"},{17,"br"},{18,"HALT"}}; 

intt size_fit(string str){
    bitset<32> inttermediate(str);
    intt res = (intt)inttermediate.to_ulong();
    return res;
}

string padding_zeroes(string res){
    while(res.size()<8){
        res = '0' + res;
    }
    return res;
}

void ldc(intt value){
	B = A;
	A = value;
}

void adc(intt value){
	A = A + value;
}

void ldl(intt offset){
	B = A;
	A = memory[offset + SP];
}

void stl (intt offset){
	memory[offset + SP] = A;
	A = B;
} 

void ldnl(intt offset){
	A = memory[offset+A];
}

void stnl(intt offset){
	memory[offset + A] = B;
}

void add(){
	A = A + B;
}

void sub(){
	intt temp = B - A;
	A = temp;
}

void shl() {
	A = B << A;
}

void shr() {
	A = B >> A;
}

void adj(intt value) {
	SP = SP + value;
}

void a2sp() {
	intt temp1 = A;
	A = B;
	SP = temp1;
}

void sp2a() {
	intt temp2 = A;
	A = SP;
	B = temp2;
}

void call(intt offset) {
	B = A;
	A = PC;
	PC += offset;
}

void returnfunc() {
	PC = A;
	A = B;
}

void brz(intt offset) {
	if (A != 0) {
		return;
	}
	PC = PC + offset;
}

void brlz(intt offset) {
	if (A >= 0) {
		return;
	}
	PC = PC + offset;
}

void br(intt offset) {
	PC = PC + offset;
}

void HALT() {
	return;
}

void ISA(){
    cout<<"Opcode mnemonic operand" << endl;
    cout<<"      data      value"<< endl;
    cout<<"0      ldc      value"<< endl;
    cout<<"1      adc      value"<< endl;
    cout<<"2      ldl      offset"<< endl;
    cout<<"4      ldnl     offset"<< endl;
    cout<<"5      stnl     offset"<< endl;
    cout<<"6      add      "<< endl;
    cout<<"7      sub      "<< endl;
    cout<<"8      shl      "<< endl;
    cout<<"9      shr      "<< endl;
    cout<<"10     adj      value"<< endl;
    cout<<"11     a2sp     "<< endl;
    cout<<"12     sp2a     "<< endl;
    cout<<"13     call     offset"<< endl;
    cout<<"14     return   "<< endl;
    cout<<"15     brz      offset"<< endl;
    cout<<"16     brlz     offset"<< endl;
    cout<<"17     br       offset"<< endl;
    cout<<"18     HALT     "<< endl;
    cout<<"       SET      value"<< endl;

}

intt bin_to_dec(string s){
    
    string temp = s;
    char sign = temp[0];
    while(temp.size()<32){
        temp = sign + temp;
    }
    intt res = size_fit(temp);
    return res;
}

string int_to_hex(intt i){
    stringstream ss;
    ss<<hex<<i;
    string temp = ss.str();
    temp = padding_zeroes(temp);
    return temp;
}

map<string,char> hex_from_bin_map = {{"0000",'0'},{"0001",'1'},{"0010",'2'},{"0011",'3'},{"0100",'4'},{"0101",'5'},{"0110",'6'},{"0111",'7'},{"1000",'8'},{"1001",'9'},{"1010",'A'},{"1011",'B'},{"1100",'C'},{"1101",'D'},{"1110",'E'},{"1111",'F'}};
string hex_from_bin(string s){
    intt gap = 4 , m= 4;
    string res= "";
    for(intt i=1;i<=s.size();i+=gap){
        string temp = s.substr(i-1,gap);
        res =  res + hex_from_bin_map[temp];
    }
    return res;
}


intt bin_to_decimal(string str){
    intt num = 0;
	for (intt i = 1; i <= 32; i++) {
		num = num * 2;
		if (str[i-1] != '0') num++;
	}
	return num;
}

intt instr_execute(intt s){
    PC++;
    intt value = s >> 8;
    if(!(s>=0)){
        value = value|(0xff000000);
    }
    intt opcode = s & ((1<<8)- 1);
    intt offset = value;
    if(opcode == 0){
        ldc(value);
        return 0;
    }
    if(opcode == 1){
        adc(value);
        return 0;
    }
    if(opcode == 2){
       ldl(offset);
        return 0;
    }
    if(opcode == 3){
        stl(offset);
        return 0;
    }
    if(opcode ==4){  
        ldnl(offset);
        return 0;
    }
    if(opcode == 5){ 
        stnl(offset);
        return 0;
    }
    if(opcode == 6){  
        add();
        return 0;
    }
    if(opcode == 7){ 
        sub();
        return 0;
    }
    if(opcode == 8){      
        shl();
        return 0;
    }
    if(opcode == 9){ 
        shr();
        return 0;
    }
    if(opcode==10){        
        adj(value);
        return 0;
    }
    if(opcode == 11){  
        a2sp();
        return 0;
    }
    if(opcode == 12){
        sp2a();
        return 0;
    }
    if(opcode == 13){
        call(offset);
        return 0;
    }
    if(opcode == 14){  
       returnfunc();
        return 0;
    }
    if(opcode == 15){
        brz(offset);
        return 0;
    }
    if(opcode == 16){
        brlz(offset);
        return 0;
    }
    if(opcode == 17){
        br(offset);
        return 0;
    }
    if(opcode == 18){
        HALT();
        return 1;
    }
    cout << endl;
    cout << opcode << endl;
    return 2;
}


string rename_file_req(string s){
    intt ck = 0;
    intt x = s.find('.');
    if(x == string::npos){
        ck = 1;
    }
    if(!ck){
        return s.substr(0,x);
    }else{
        return s;
    }
}

void memory_dump_write(string filename,intt num){
    fstream memDump;
    memDump.open(rename_file_req(filename)+"_memorydumpfile.txt",ios::out);
    for(intt ct=0;ct<num;ct++){
        memDump<<int_to_hex(ct) << "     " <<int_to_hex(memory[ct])<<"\n";
    }
    memDump.close();
}
void store_PC_incr(string mcode){
    memory[PC] = bin_to_dec(mcode);
    PC++;
}
intt StoreinttoMemory(ifstream &code){
    PC = 0;
    string mcode;
    while(getline(code,mcode)){
        store_PC_incr(mcode);
    }
    return PC;
}

void memory_display(intt num){
    memory_store(11);
    for(intt ct=0;ct<num;ct++){
        cout<<int_to_hex(ct)<<"     "<<int_to_hex(memory[ct])<<"\n";
    }
}



intt run_instructions(intt trace,intt num){
    ofstream out;
    out.open("trace.txt");
    PC = 0;
    intt ct = 0;
    while(PC < num + 1){
        intt temp = instr_execute(memory[PC]);
        intt Op = memory[PC-1]&0xff;
        ct++;
        memory_store(11);
        if(trace){
            cout<<"PC="<<int_to_hex(PC)<<"\t"<<"SP="<<int_to_hex(SP)<<"\t"<<"A="<<int_to_hex(A)<<"\t"<<"B="<<int_to_hex(B)<<"\t"<<mneumonics[Op]<<"\t"<<int_to_hex(memory[PC])<<"\n";
            out<<"PC="<<int_to_hex(PC)<<"\t"<<"SP="<<int_to_hex(SP)<<"\t"<<"A="<<int_to_hex(A)<<"\t"<<"B="<<int_to_hex(B)<<"\t"<<mneumonics[Op]<<"\t"<<int_to_hex(memory[PC])<<"\n";
        }
        if(temp==1){          
            break;
        }
        else if(temp==2){       
            cout << int_to_hex(PC-1) <<" undefined opcode error" << endl;
            break;
        }
    }
    cout << ct << " lines executed " << endl;
    return 0;
}


void options_available(){
    cout <<"choose" << endl;
    cout <<"1 for" << "prior instruction" << endl;
    cout <<"2 for" << "trace" << endl;
    cout <<"3 for" << "isa" << endl;
    cout <<"4 for" << "after instruction execution" << endl;
}


int main(){
    cout<<"give the machine code file name as input to " << endl;
    string filename;
    cin>>filename;
    ifstream machine_file(filename);
    
    while(true)
    {
        options_available();
        intt flag;
        cin >> flag;
        intt num = StoreinttoMemory(machine_file);       
        if(!(flag==1 || flag==2||flag==3||flag==4)){
            cout<<"invalid option as it is not from 1 to 4" << endl;
            continue;
        }
        if(flag==1){
            cout<<"memory prior execution of code"<< endl;
            memory_display(num);
        }
        if(flag==2){
            cout<<"trace is given as"<<endl;
        }
        run_instructions(flag==2,num); 
        if(flag==3){
            ISA();  
        }
        if(flag==4){
            cout<< "memory after performing the execution" << endl;
            memory_display(num);
            memory_dump_write(filename,num);
        }
        break;
    }
}