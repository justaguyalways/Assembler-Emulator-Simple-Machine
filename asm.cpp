// Name - A.S.Poornash
// Roll no. - 2101CS01
// Declaration of Authorship
// This cpp file, asm.cpp, is part of the assignment of CS210 at the 
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
using namespace std;

// globa vars
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
vector<int>no_operand_ind = {7,8,9,10,12,13,15,19};
vector<int>value_ind = {0,1,2,11,20};
vector<int>offset_ind = {3,4,5,6,14,16,17,18};

//functions
string sttoval(string s){
    int flagq = 1,flagp=0,flagr=0;    
    for(int i=0;i<s.size()-1;i++){
        if(!isdigit(s[i+1])){
            flagq = 0;
            break;
        }
    }
    if(s[0] == '+'){
        flagr = 1;
    }
    if(s[0]=='-'){
        flagp =1;
    }
    if(isdigit(s[0])){
        flagp =1;
    }
    if(flagq  && flagr){
        if(s.size()-1){
            flagp = 1;
            s = s.substr(1,s.size()-1);
        }
    }
    if(flagq && flagp){
        stringstream m9, m10;
        int t1 = 6;
        int v;
        m9 << s;
        m9 >>  v;
        m10 << hex << v;
        string ans = m10.str();
        t1 -= ans.size();
        if((t1+1)<1){
            ans = ans.substr(ans.size()-6,6);
            for(int i=1;i<=ans.size();i++){
                if(islower(ans[i-1])){
                    ans[i-1] = ans[i-1]-32; 
                }
            }
            return ans;
        }
        else{
            for(int i=1;i<=ans.size();i++){
                if(islower(ans[i-1])){
                    ans[i-1] = ans[i-1]-32;
                }
            }
            string add = "";
            for(int i=0;i<t1;i++){
                add = add + "0";          
            }
            ans = add + ans;
            return ans;
        }
    }
    else if(s[0]=='0' && s[1]=='x'){
        if(s.size()<9){
            int m11 = s.size()-2;
            string ans1 = s.substr(2,m11);
            for(int i=1;i<=ans1.size();i++){
                if(islower(ans1[i-1])){
                    ans1[i-1] = ans1[i-1]-32;
                }
                if(ans1[i-1]>64 && ans1[i-1]<91){
                    return "@@@";
                }
                if(!(isdigit(ans1[i-1]))){
                    return "@@@";
                }
            }
            while(ans1.size()<6){
                ans1 = "0" + ans1;
            }
            return ans1;
        }
        else{
            return "@@@";
        }
    }
    else if(s[0]=='0' && s[1]=='o'){
        if(s.size()<13){
            int m11 = s.size()-2;
            s = s.substr(2,m11);
            for(int i=0;i<s.size();i++){
                if(!(isdigit(s[i])&& s[i]!='9' && s[i]!='8')){
                    return "@@@";
                }
            }
            int i=stoi(s,0,8);
            stringstream ss;
            ss<<hex<<i;
            string ans3 = ss.str();
            while(ans3.size()<6){
                ans3 = "0" + ans3;
            }
            return ans3;
        }
        else{
            return "@@@";
        }
    } 
    return "@@@";           
}
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

// split a string by char
vector<string> splitchar(string s, char b){
    stringstream test(s);
    string segment;
    vector<string> seglist;
    while(std::getline(test, segment, b)){
        seglist.push_back(segment);
    }
    return seglist;
}

//error checking for each instr and also give value fo instr
string check_error(string s, int j, string listwrite, int u){
    // for HALT warning
    auto halt_check_case = s.find("HALT");
    if(halt_check_case != string::npos){
        halt_check = 1;
    }
    vector<string> parts0 = splitchar(s, ' ');
    vector<string>parts;
    for(int i = 0 ;i < parts0.size() ; i++){
        parts0[i] = regex_replace(parts0[i], regex("^ +"), "");
        parts0[i] = regex_replace(parts0[i], regex(" +$"), "");
        if(parts0[i].size()){
            parts.push_back(parts0[i]);
        }
    }
    //finding instr in master list
    auto it = find(pre_instr_dict.begin(), pre_instr_dict.end(), parts[0]);

    //check if instr is in master list
    if(it == pre_instr_dict.end()){
        error_encountered.push_back({u, 0});
    }
    else
    {
        int instr_ind = it - pre_instr_dict.begin();
        // for no operand instr
        if(find_int_in_vector(no_operand_ind, instr_ind) != -1){
            if(parts.size() > 1){
                error_encountered.push_back({u, 3});
            }
            string value4s = to_string(instr_ind-1);
            value4s = sttoval(value4s).substr(4,2);
            listwrite = listwrite + " " + "000000" + value4s;
        }
        // for value operand instr
        if(find_int_in_vector(value_ind, instr_ind)!= -1){
            if(parts.size() < 2){
                error_encountered.push_back({u, 4});
            }else{
                int value2 = find_string_in_vector(pre_label_dict, parts[1]);
                if(value2 != -1){
                    value2 = pre_label_dict_map[value2];
                    labels_used[value2]++;
                    string value2s = to_string(value2);
                    if(find_string_in_vector(pre_label_dict, parts[1]) != -1){
                        listwrite = listwrite + " " + sttoval(value2s);
                    }
                }else{
                    string value3s = sttoval(parts[1]);
                    if(value3s == "@@@"){
                        int f1 = is_alpha(parts[1][0]);
                        for(int i = 1 ; i < parts[1].size() ; i++){
                            f1 = f1 & is_alphanum(parts[1][i]);
                        }
                        if(f1){
                            error_encountered.push_back({u, 5});
                        }else{
                            db = 1;
                            error_encountered.push_back({u, 6});
                        }
                    }else{
                        listwrite = listwrite + " " + value3s;
                    }
                }
                int value1 = instr_ind - 1;
                string value1s = to_string(value1);
                value1s = sttoval(value1s);
                value1s = value1s.substr(4,2);
                if(value1 != -1 && value1 != 19){
                    listwrite = listwrite + value1s;
                }else{
                    //extending the value part to cover the opcode as well
                    listwrite = listwrite.substr(0,9) + "00" + listwrite.substr(9,6);
                }
                if(parts.size() > 2){
                    if(db){
                        error_encountered.pop_back();
                    }
                    error_encountered.push_back({u,3});
                }
            }
            db = 0;   
        }
        //for offset operand instr
        if(find_int_in_vector(offset_ind, instr_ind)!= -1){
            if(parts.size() < 2){
                error_encountered.push_back({u, 4});
            }else{
                int value2 = find_string_in_vector(pre_label_dict, parts[1]);
                if(value2 != -1){
                    value2 = pre_label_dict_map[value2];
                    labels_used[value2]++;
                    value2 = value2 - j + 1;
                    if(value2 == -1){
                        seg_fault.push_back(u);
                    }
                    string value2s = to_string(value2);
                    if(find_string_in_vector(pre_label_dict, parts[1]) != -1){
                        listwrite = listwrite + " " + sttoval(value2s);
                    }
                }else{
                    string value3s = sttoval(parts[1]);
                    if(value3s == "@@@"){
                        int f1 = is_alpha(parts[1][0]);
                        for(int i = 1 ; i < parts[1].size() ; i++){
                            f1 = f1 & is_alphanum(parts[1][i]);
                        }
                        if(f1){
                            error_encountered.push_back({u, 5});
                        }else{
                            db = 1;
                            error_encountered.push_back({u, 6});
                        }
                    }else{
                        listwrite = listwrite + " " + value3s;
                    }
                }
            }
            int value1 = instr_ind - 1;
            string value1s = to_string(value1);
            value1s = sttoval(value1s);
            value1s = value1s.substr(4,2);
            if(value1 != -1 && value1 != 19){
                listwrite = listwrite + value1s;
            }
            if(parts.size() > 2){
                if(db){
                    error_encountered.pop_back();
                }
                error_encountered.push_back({u,3});
            }
            db = 0;
        }
    }
    return listwrite;
}
    



//checking label name format error
int check_label_format(string s){
    int f1 = is_alpha(s[0]);
    for(int i = 1; i < s.size() ; i++){
        f1 = f1 && (is_alphanum(s[i]) || s[i] == '_');
    }
    return f1;
}

int main(){
    //taking file name
    string code_name;
    cout << "enter asm file name" << endl;
    cin >> code_name;
    fstream new_file;

    //reading from the given file name
    new_file.open(code_name+".asm", ios::in);
    if (new_file.is_open()) { 
        string line;
        while (getline(new_file, line)) { 
            read_instr.push_back(line);
        }
        new_file.close(); 
    }
    int p2 = -1;
   //recording all labels and seeing if there are more than 2 labels in a line
    for (int i = 0 ; i < read_instr.size() ; i++){
         //remove leading spaces in instr
        read_instr[i] = regex_replace(read_instr[i], regex("^ +"), "");

        //removing comments
        vector<string> temp_instr_br = splitchar(read_instr[i],';');

        //label work
        string temp_instr = temp_instr_br[0];
        temp_instr_br = splitchar(temp_instr, ':');

        //seeing if there is a label or not
        int ct = 0;
        for(int k = 0 ; k < temp_instr.size() ; k++){
            if(temp_instr[k] == ':') ct++;
        }

        vector<string> temp_instr_br1;
        for(int j = 0 ; j < temp_instr_br.size() ; j++){
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex("^ +"), "");
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex(" +$"), "");
            if(temp_instr_br[j].size()){
                temp_instr_br1.push_back(temp_instr_br[j]);
            }
        }

        if(temp_instr_br1.size() == 0){
            continue;
        }
        p2++;
        if(ct){
            if(temp_instr_br1.size() > 2){
                error_encountered.push_back({i, 3});
            }else{
                if(check_label_format(temp_instr_br1[0])){
                    //duplicate declaration of labels
                    if(count(pre_label_dict.begin(), pre_label_dict.end(), temp_instr_br1[0]) != 0){
                        error_encountered.push_back({i, 2});
                        continue;
                    }
                    pre_label_dict.push_back(temp_instr_br1[0]);
                    pre_label_dict_map[pre_label_dict.size()-1] = p2;
                }else{
                   error_encountered.push_back({i, 1});
                }
            }
            if(temp_instr_br1.size() == 1){
                p2--;
            }
        }
        
    }
    
    //for SET function
    int p1=0;
    for (int i = 0 ; i < read_instr.size() ; i++){
         //remove leading spaces in instr
        read_instr[i] = regex_replace(read_instr[i], regex("^ +"), "");

        //removing comments
        vector<string> temp_instr_br = splitchar(read_instr[i],';');

        //label work
        string temp_instr = temp_instr_br[0];
        temp_instr_br = splitchar(temp_instr, ':');
        vector<string> temp1;
        for(auto x: temp_instr_br){
            auto y = splitchar(x, ' ');
            for(auto z: y){
                temp1.push_back(z);
            }
        }
        temp_instr_br = temp1;
        //seeing if there is a label or not
        int ct = 0;
        for(int k = 0 ; k < temp_instr.size() ; k++){
            if(temp_instr[k] == ':') ct++;
        }

        vector<string> temp_instr_br1;
        for(int j = 0 ; j < temp_instr_br.size() ; j++){
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex("^ +"), "");
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex(" +$"), "");
            if(temp_instr_br[j].size()){
                temp_instr_br1.push_back(temp_instr_br[j]);
            }
        }

        if(temp_instr_br1.size() == 0){
            continue;
        }
        p1++;
        // for handling SET 
        if(temp_instr_br1.size() == 3){
            auto set_pos = temp_instr_br1[1].find("SET");
            if(set_pos != string::npos){
                if(find_string_in_vector(pre_label_dict, temp_instr_br1[0]) != -1){
                    int value5 = find_string_in_vector(pre_label_dict, temp_instr_br1[0]);
                    pre_label_dict_map[value5] = stoi(temp_instr_br1[2]);
                    lines_for_set.push_back(i);
                }
            }
        }
        
        
    }

    //detecting errors in instr part
    // for PC 
    int p = 0;
    for (int i = 0 ; i < read_instr.size() ; i++){
        string curr_ind = to_string(p);
        if(p < 0){
            curr_ind = to_string(0);
        }
        
        string listwrite = "00" + sttoval(curr_ind);

        if(find_int_in_vector(lines_for_set, i) != -1){
            p++;
            listwrite = listwrite + " " + read_instr[i];
            cout << listwrite << endl;
            for_listing_file.push_back(listwrite);
            continue;
        }
        //remove leading spaces in instr
        read_instr[i] = regex_replace(read_instr[i], regex("^ +"), "");
        
        //removing comments
        vector<string> temp_instr_br = splitchar(read_instr[i],';');

        //label work
        string temp_instr = temp_instr_br[0];
        string instr_for_listwrite = temp_instr;
        temp_instr_br = splitchar(temp_instr, ':');

        //seeing if there is a label or not
        int ct = 0;
        for(int k = 0 ; k < temp_instr.size() ; k++){
            if(temp_instr[k] == ':') ct++;
        }

        vector<string> temp_instr_br1;
        for(int j = 0 ; j < temp_instr_br.size() ; j++){
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex("^ +"), "");
            temp_instr_br[j] = regex_replace(temp_instr_br[j], regex(" +$"), "");
            if(temp_instr_br[j].size()){
                temp_instr_br1.push_back(temp_instr_br[j]);
            }
        }
        
        //empty line or comment only for PC updation
        if(temp_instr_br1.size() == 0){
            for_listing_file.push_back(listwrite);
            cout << listwrite << endl;
            continue;
        }
        p++;

        if(ct){
            if(temp_instr_br1.size() > 2){
            }else{
                if(temp_instr_br1.size() > 1){    
                    listwrite = check_error(temp_instr_br1[1], p + 1  , listwrite, i);    
                }else{
                    p--;
                }
            }
        }else{ 
                listwrite = check_error(temp_instr_br1[0], p + 1 , listwrite, i);
        }
        if(listwrite.size() >= 17){
            for_object_file.push_back(listwrite.substr(9,8));
        }
        
        listwrite = listwrite + " " + instr_for_listwrite;
        for_listing_file.push_back(listwrite);
        cout << listwrite << endl;           
    }
    for(auto x: error_encountered){
        cout << x.first << " " << x.second << endl;
    }
    int ck = 1;
    string logfilename;
    cout << "Enter the name the result files are to be named according to" << endl;
    cin >> logfilename;
    if(!error_encountered.size()){
        string final_listing;
        if(for_listing_file.size() > 0){
            final_listing = for_listing_file[0];
            for(int m = 1 ; m < for_listing_file.size(); m++ ){
                final_listing = final_listing + "\n" + for_listing_file[m];
            }
        }else{
            final_listing = "";
        }
        std::ofstream log_file(
        logfilename + ".lst", std::ios_base::out | std::ios_base::app ); 
        log_file << final_listing << endl;
    }else{
        sort(error_encountered.begin(), error_encountered.end(), [](pair<int,int> a, pair<int,int> b){return b.first > a.first;});
        string error_listing = "line " + to_string(error_encountered[0].first) + " " + error_map[error_encountered[0].second];
        for(int lo = 1 ; lo < error_encountered.size() ; lo++){
            error_listing = error_listing + "\n" + "line " + to_string(error_encountered[lo].first) + " " + error_map[error_encountered[lo].second];
        }
        std::ofstream log_file(
        logfilename  + ".log", std::ios_base::out | std::ios_base::app ); 
        log_file << error_listing << endl;
        ck = 0;
    }
    if(ck){
        //write object file
        string whole_object_file = "";
        int ct1 = 0;
        for(int k1 = 0 ; k1 < for_object_file.size() ; k1++){
            string object_file_line;
            object_file_line = for_object_file[k1];
            object_file_line = regex_replace(object_file_line, regex("^ +"), "");
            object_file_line = regex_replace(object_file_line, regex(" +$"), "");
            // cout << object_file_line << endl;
            if(!object_file_line.size()) continue;
            ct1++;
            stringstream ss;
            ss << hex << object_file_line;
            unsigned n1;
            ss >> n1;
            bitset<32> b(n1);
            object_file_line = b.to_string();
            if(ct1==1){
                whole_object_file = object_file_line;
            }else{
                whole_object_file = whole_object_file + "\n" + object_file_line;
            }
        }
        std::ofstream log_file(
            logfilename + ".o", std::ios_base::out | std::ios_base::app ); 
            log_file << whole_object_file << endl;
        for(auto x:for_object_file){
            cout << x<< endl;
        }
    }
    // for warning file
    string warning_lines = "";
    if(!halt_check){
        warning_lines = "Warning: no HALT instruction found\n";
    }
    for(auto y1 : seg_fault){
        warning_lines = warning_lines + "Warning: line " + to_string(y1) + " caused segmentation fault(can be caused by a infinite loop)\n";
    }
    std::ofstream log_file(
            logfilename +"_warning"+ ".log", std::ios_base::out | std::ios_base::app ); 
            log_file << warning_lines << endl;
    return 0;
}
