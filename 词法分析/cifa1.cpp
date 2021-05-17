#include<iostream>
#include<vector>
#include<cstring> 
#include<map>
#include<set>
#include<fstream>
#include<iomanip>
#include<algorithm>
#include<string>
#include<math.h>
using namespace std;

int state=0;
char c;
string token, iskey="-1";
int ptr=-1, maxPtr=-1;                     //maxPtr????????????????????¦Ë??
int flag=0;                                 //??????,??????retract???????¦Ë1
vector<pair<string, string> > keyTable;     //??????
map<string, int> wordNum;                   //???????????????
set<int> errorLocation;                     //????????¦Ë??
long int charNum=-1;                        //??????????
long int line=1;                 //????????????????
map<string,int> SymbolTable;                //?????


                                /*????????*/

void extract();                             //??????§Û?????txt???????????›ÔkeyTable
int table_insert();                         //???????????????????????SymbolTable????????¦Ë?????
void error();                               //???????§Õ???
double SToF();                              //???????????????
int SToI();                                 //?????????????
int SToI_oct();                             //?????????????????????
int SToI_hex();                             //???????????????????????
string reserve();                           //????????????
void retract();                             //??????????
bool letter();                              //????????
bool digit();                               //?????????
void cat();                                 //?????C?????token????
void get_nbc();                             //??????C???????
void get_char();                            //???????
template<class T,class U>
void takenote(U a,T b);                     //???????? 

//????????????›Ô?????
ofstream F_Out("result.txt", ios::out);

                                 /*??????*/
int main(){

	F_Out << "[Line " << line << "]:" << endl;
    extract();
    //?????????????????
	if (NULL == freopen("testWithoutError.txt", "r", stdin)) {
		cout << "test.txt?????" << endl;
		cout << "FAILED!" << endl;
		return 0;
	}
	
    do {
            switch (state) {
            case 0:{
            	token="";
                if(flag==0) get_char();
                get_nbc();
                switch (c) {
                    //??????????
                    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':
                    case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':
                    case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case '_':
                    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':
                    case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':
                    case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':state = 1; break;
                    //???¨®?????
                    case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state = 2; break;
                    //???????????
                    case '<':state = 3; break;
                    case '>':state = 4; break;
                    case '=':state = 16; break;                                               //???????§Ø?????????????????
                    case '+':state = 0; ++wordNum["+"]; takenote ("+","-"); flag=0; break;    //??
                    case '-':state = 0; ++wordNum["-"]; takenote ("-","-"); flag=0; break;    //??
                    case '*':state = 0; ++wordNum["*"]; takenote ("*","-"); flag=0; break;    //??
                    case '(':state = 0; ++wordNum["("]; takenote ("(","-"); flag=0; break;    //??????
					case ')':state = 0; ++wordNum[")"]; takenote (")","-"); flag=0; break;    //?????? 
					case '{':state = 0; ++wordNum["{"]; takenote ("{","-"); flag=0; break;    //??????
					case '}':state = 0; ++wordNum["}"]; takenote ("}","-"); flag=0; break;    //?????? 
					case '[':state = 0; ++wordNum["["]; takenote ("[","-"); flag=0; break;    //??????
					case ']':state = 0; ++wordNum["]"]; takenote ("]","-"); flag=0; break;    //?????? 
                    case '&':state = 5; break;                                                 //?????
                    case '|':state = 6; break;                                                 //?????
                    case '!':state = 0; flag=0; ++wordNum["!"]; takenote ("logic-op","not"); break;    //?????
					case ';':state = 0; flag=0; ++wordNum[";"];break;
					case ',':state = 0; flag=0; ++wordNum[","];break;
					case '.':state = 0; flag=0; ++wordNum["."];break;
                    case '0':state = 7; break;                                                  //????????????????????
                    case '/':state = 9; break;                                                  //?????
                    case '\n':state=17;break;
                    default:state = 18; break;     
                };
                break;
            }
            case 1:{                    //???????
                cat();
                get_char();
                iskey=reserve();
                if(letter() || digit() || c=='_') state=1;
                else if(c=='(' && iskey=="-1"){
                	state=19;
				}
//				else if(c=='['){
//					state=20;					
//				}
                else{
                    retract();
                    state=0;
                    iskey=reserve();
                    if(iskey!="-1") { ++wordNum[token]; takenote (iskey,"-"); }
                    else{
                        int identry=table_insert();
                        ++wordNum[token];
                        takenote ("id",identry);
                        
                    }
                }
                break;
            }
            case 2:{             //??????
                cat();
                get_char();
                switch(c){
                    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state=2;break;
                    case '.': state=11;break;
                    case 'E': state=13;break;
                    default:retract(); state=0; ++wordNum["num-dec"]; takenote ("num-dec",SToI());  break;
                };
                break;
            }
            case 11:{              //§³??????
                cat();
                get_char();
                if(digit()) state=12;
                else{
                    error();
                    state=0;
                }
                break;
            }
            case 12:{              //§³??????
                cat();
                get_char();
                switch(c){
                    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state=12;break;
                    case 'E': state=13;break;
                    default: retract(); state=0;  ++wordNum["num-dec"];  takenote ("num-dec",SToF()); break;
                };
                break;
            }
            case 13:{              //E??
                cat();
                get_char();
                switch(c){
                    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state=15;break;
                    case '+':case '-': state=14;break;
                    default: retract(); error(); state=0; break;
                };
                break;
            }
            case 14:{              //???????????
                cat();
                get_char();
                if(digit()) state=15;
                else{
                    retract();
                    error();
                    state=0;
                }
                break;
            }
            case 15:{              //??????
                cat();
                get_char();
                if(digit()) state=15;
                else{
                    retract();
                    state=0;
                    ++wordNum["num-dec"]; 
                    takenote ("num-dec",SToF());
                }
                break;
            }
            case 3:{              //§³??
                cat();
                get_char();
                switch(c){
                    case '=': cat(); state=0; flag=0; ++wordNum[token]; takenote ("relop","LE"); break;
                    case '>': cat(); state=0; flag=0; ++wordNum[token]; takenote ("relop","NE"); break;
                    default:  retract(); state=0; ++wordNum[token]; takenote ("relop","LT"); break; 
                };
                break;
            }
            case 4:{              //????
                cat();
                get_char();
                switch(c){
                    case '=': cat(); state=0; flag=0; ++wordNum[token]; takenote ("relop","GE"); break;
                    default:  retract(); state=0; ++wordNum[token]; takenote ("relop","GT"); break; 
                };
                break;
            }
            case 16:{              //???
                cat();
                get_char();
                if(c=='='){
                    cat();
                    state=0;
                    flag=0;
                    ++wordNum[token];
                    takenote ("relop","EQ");
                }
                else{
                    retract();
                    state=0;
                    ++wordNum[token]; 
                    takenote ("assign-op","=");
                }
                break;
            }
            case 5:{              //¦Ë??????
                cat();
                get_char();
                if(c=='&'){
                    cat();
                    state=0;
                    flag=0;
                    ++wordNum[token]; 
                    takenote ("logic-op","and");
                }
                else{
                    retract();
                    state=0;
                    ++wordNum[token];
                    takenote("bit-op","and");
                }
                break;
            }
            case 6:{              //¦Ë?????
                cat();
                get_char();
                if(c=='|'){
                    cat();
                    state=0;
                    flag=0;
                    ++wordNum[token]; 
                    takenote ("logic-op","or");
                }
                else{
                    retract();
                    ++wordNum[token];
                    takenote("bit-op","or");
                    state=0;
                }
                break;
            }
            case 7:{              //???????????????0
                cat();
                get_char();
                switch(c){
                    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':state=7; break;
                    case 'x': state=8;break;
                    default:  retract(); state=0; ++wordNum["num-oct"];  takenote ("num-oct",SToI_oct());break;
                };
                break;
            }
            case 8:{              //?????????0x
                cat();
                get_char();
                switch(c){
                    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
                    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
                    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':state=8;break;
                    default:retract(); state=0; ++wordNum["num-hex"]; takenote ("num-hex",SToI_hex()); break;
                };
                break;
            }
            case 9:{              //??
                cat();
                get_char();
                if(c=='*') state=10;
                else{
                    retract();
                    state=0;
                    ++wordNum[token];
                    takenote ("/","-");
                }
                break;
            }
            case 10:{              //???
                get_char();
                while(c!='*') get_char();
                get_char();
                if(c=='/') {
                    state=0;
                    flag=0;
                    ++wordNum["??????"];
                    takenote("???","-");
                }
                else {
                	error();
                	flag=0;
                	state=0;
				}
                break;
            } 
            case 17:{              //????
                ++wordNum["enter"];
                ++line;
                F_Out << "[Line " << line << "]:" << endl;
                state = 0;
            	flag=0;
                break;
            }
            case 19:{              //????
            	get_char();
            	if(letter() || digit()){
            		state=19;
				}
				else if(c==')'){
					state=0;
					flag=0;
					++wordNum[token];
					int identry=table_insert();
                    takenote ("function-id",identry);
					//takenote("function",token);
				}
				else{
					retract();
					error();
					state=0;
				}
				break;
			}
//			case 20:{              //????
//				get_char();
//				if(digit()) state=20;
//				else if(c==']'){
//					state=0;
//					flag=0;
//					++wordNum["array"];
//					takenote("array",token);
//				}
//				else{
//					retract();
//					error();
//					state=0;
//				}
//				break;
//			}
            case 18:{              //????????
                error();
                state=0;
                flag=0; 
                break;
            }
        }
    }while(c!=EOF);

                                            /*?????????*/

    F_Out << endl << "-------??????????????????????????????-------" << endl;
	F_Out << "?????????" << line << endl << endl;
	F_Out << "?????????" << charNum << endl << endl;
	F_Out << "???????(?????????????)??" << endl;
	for (auto it : wordNum) {
		F_Out << "( ?? "<< it.first <<" ?? , " << it.second <<" ) "<< endl;
	}
	F_Out << endl;
	if (errorLocation.empty())
		F_Out << "??????§Ó???????????" << endl;
	else {
		F_Out << "??????§Õ?????????????????§Ü??????" << endl << "Line: ";
		for (auto it : errorLocation)
			F_Out <<endl<<"  " << it;
	}
	cout<<"successful";
	return 0;
}




void get_char(){
    c=getchar();
    //cout<<c;
    if(c!='\n' && c!='\t' && c!=' '){
    	++charNum;
	}

}

void get_nbc(){
    while(c==' '){
        get_char();
    }
}

void cat(){
    token+=c;
}

bool digit(){
    return (c<='9' && c>='0') ? true : false;
}

bool letter(){
    return isalpha(c);
}

void retract(){
    flag=1;
}

string reserve(){
	for (auto it : keyTable) {
		if (it.first == token)
			return it.second;
	}
	return "-1";
}

int SToI(){
    return stoi(token);
}

int SToI_oct(){
	return stoi(token,nullptr,0);
}

int SToI_hex(){
	return stoi(token,nullptr,16);
}

double SToF(){
	return stof(token);
}

void error()
{
	errorLocation.insert(line);    //??????????????§Ü?
	return;
}

int table_insert(){
	map<string,int>::const_iterator got = SymbolTable.find (token);
    if(got==SymbolTable.end()){
        ++maxPtr;
        SymbolTable[token]=maxPtr;
        return maxPtr;
    }
    else{
        ptr=got->second;
        return ptr;
    }
}

void extract(){
    	//?????›¥?????????????
	ifstream F_keyTable("key_table.txt", ios::in);

	string expression = "", mark = "";
	if (!F_keyTable.is_open())
		cout << "Open file failure" << endl;
	while (!F_keyTable.eof()){            // ??¦Ä??????????????
		F_keyTable >> expression >> mark;
		keyTable.push_back(make_pair(expression, mark));
	}
	F_keyTable.close();
}

template<class T,class U>
void takenote(U a,T b){
    F_Out <<"	< ' "<< a <<" '  ,  "<< b <<" >" << endl;
}

