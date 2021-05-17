#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include<fstream>
using namespace std;
/*Epsilon 表示空串符号"ε"*/
ofstream Fout("Analysis.txt", ios::out);
/*实现split函数，读入的LR1文法时需要*/
vector<string> split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;
    while(i != s.size()){
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0){
        flag = 1;
        for(string_size x = 0; x < seperator.size(); ++x)
        if(s[i]==seperator[x]){
            ++i;
            flag = 0;
            break;
            }
        }
        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[j] == seperator[x]){
                    flag = 1;
                    break;
                }
                if(flag == 0) 
                    ++j;
        }
        if(i != j){
        result.push_back(s.substr(i, j-i));
        i = j;
        }
    }
    return result;
}

/* 产生式结构，左部符号和右部符号串 */
struct Production {
    string left;
    vector<string> rights;
    bool operator==(Production& rhs) const {
        if (left != rhs.left || rhs.rights.size() != rights.size())
            return false;
        for (int i = 0; i < rights.size(); i++) {
            if (rights[i] != rhs.rights[i])
                return false;
        }
        return true;
    }
};

/* LR1项目 */
struct Item {
    Production p;
    int location;
    string next;//向前看符号 
};

/* LR1项目集 */
struct Items {
    vector<Item> items;
};

/* LR1项目集规范族 */
struct ClusterOfItems {
    /* 有效项目集的集合 */
    vector<Items> items;
    /* 保存DFA的图，first为转移到的状态序号，second是经什么转移 */
    vector< pair<int, string> > g[1000];
}COI;

/* 文法结构体 */
struct Grammar {
    int num;  // 产生式数量
    vector<string> T;   // 终结符
    vector<string> N;   // 非终结符
    vector<Production> prods;  //产生式
}grammar;

/* FIRST集 */
map<string, set<string> > first;

/* DFA队列， 用于存储待转移的有效项目集 */
queue< pair<Items, int> > Q;

/* action表和goto表 */
pair<int, int> action[1000][1000]; // first表示分析动作,1->S 2->R 3->ACC second表示S转移状态或者R产生式序号
int gotoTable[1000][1000];

/* 待分析串 */
vector<string> str;
/* 分析栈,包括状态栈和符号栈*/
stack< pair<int, string> > ST; // first是当前状态，second 是符号

/* 判断ch是否是终结符 */
int isInT(string ch)
{
    for (int i = 0; i < grammar.T.size(); i++) {
        if (grammar.T[i] == ch) {
            return i + 1;//这里直接返回该终结符在T数组中的位置，方便后面action二维表使用
        }
    }
    return 0;
}
/* 判断ch是否是非终结符 */
int isInN(string ch)
{
    for (int i = 0; i < grammar.N.size(); i++) {
        if (grammar.N[i] == ch) {
            return i + 1;//这里直接返回该非终结符在N数组中的位置，方便后面goto二维表使用
        }
    }
    return 0;
}
/* 求FIRST集 */
void getFirstSet()
{
    /* 终结符的FIRST集是其本身 */
    for (int i = 0; i < grammar.T.size(); i++) {
        string X = grammar.T[i];
        set<string> tmp;
        tmp.insert(X);
        first[X] = tmp;
    }
    /* 当非终结符的FIRST集发生变化时循环 */
    bool change = true;
    while (change) {
        change = false;
        /* 枚举每个产生式 */
        for (int i = 0; i < grammar.prods.size(); i++) {
            Production &P = grammar.prods[i];
            string X = P.left;
            set<string> &FX = first[X];
            /* 如果右部第一个符号是空或者是终结符，则加入到左部的FIRST集中 */
            if (isInT(P.rights[0]) || P.rights[0] == "Epsilon") {
                /* 查找是否FIRST集是否已经存在该符号 */
                auto it = FX.find(P.rights[0]);
                /* 不存在 */
                if (it == FX.end()) {
                    change = true; // 标注FIRST集发生变化，循环继续
                    FX.insert(P.rights[0]);
                }
            } else {
                /* 当前符号是非终结符，且当前符号可以推出空，则还需判断下一个符号 */
                bool next = true;
                /* 待判断符号的下标 */
                int idx = 0;
                while (next && idx < P.rights.size()) {
                    next = false;
                    string Y = P.rights[idx];
                    set<string> &FY = first[Y];
                    for (auto it = FY.begin(); it != FY.end(); it++) {
                        /* 把当前符号的FIRST集中非空元素加入到左部符号的FIRST集中 */
                        if (*it != "Epsilon") {
                            auto itt = FX.find(*it);
                            if (itt == FX.end()) {
                                change = true;
                                FX.insert(*it);
                            }
                        }
                    }
                    /* 当前符号的FIRST集中有空, 标记next为真，idx下标+1 */
                    auto it = FY.find("Epsilon");
                    if (it != FY.end()) {
                        next = true;
                        idx = idx + 1;
                    }
                }
                
            }
        }
    }
    
    printf("FIRST:\n");
    for (int i = 0; i < grammar.N.size(); i++) {
        string X = grammar.N[i];
        cout<<X<<":";
        for (auto it = first[X].begin(); it != first[X].end(); it++) {
            cout<<*it<<" ";
        }
        printf("\n");
    }
}
/* 查找beta_a串的FIRST集， 保存到FS集合中 */
void FIRST(vector<string> &beta_a, set<string> &FS)
{  
    /* 当前符号是非终结符，且当前符号可以推出空，则还需判断下一个符号 */
    bool next = true;
    int idx = 0;
    while (idx < beta_a.size() && next) {
        next = false;
        /* 当前符号是终结符或空，加入到FIRST集中 */
        if (isInT(beta_a[idx]) || beta_a[idx] == "Epsilon") {
            /* 判断是否已经在FIRST集中 */
            auto itt = FS.find(beta_a[idx]);
            if (itt == FS.end()) {
                FS.insert(beta_a[idx]);
            }
        } else {
            string B = beta_a[idx];
            set<string> &FB = first[B];
            for (auto it = first[B].begin(); it != first[B].end(); it++) {
                /* 当前符号FIRST集包含空，标记next为真，并跳过当前循环 */
                if (*it == "Epsilon") {
                    next = true;
                    continue;
                }
                /* 把非空元素加入到FIRST集中 */
                auto itt = FS.find(*it);
                if (itt == FS.end()) {
                    FS.insert(*it);
                }
            }
        }
        idx = idx + 1;
    }
    /* 如果到达产生式右部末尾next还为真，说明beta_a可以推空，将空加入到FIRST集中 */
    if (next) {
        FS.insert("Epsilon");
    }
}

/* 判断是LR1项目t否在有效项目集I中 */
bool isInItems(Items &I, Item &t)
{
    for (auto it = I.items.begin(); it != I.items.end(); it++) {
        Item &item = *it;
        if (item.p == t.p && item.location == t.location && item.next == t.next)
            return true;
    }
    return false;
}

/* 打印某个项目集 */
void printItems(Items &I)
{
    for (auto it = I.items.begin(); it != I.items.end(); it++) {
        Item &L = *it;
        cout<<L.p.left<<"->";
        for (int i = 0; i < L.p.rights.size(); i++) {
            if (L.location == i)
                printf(".");
            cout<<L.p.rights[i];
        }
        if (L.location == L.p.rights.size())
            printf(".");
        cout<<','<<L.next<<"  ";
    }
    printf("\n");
}

/* 求I的闭包 */
void closure(Items &I)
{
    bool change =  true;
    while (change) {
        change = false;
        Items J;
        /* 枚举每个项目 */
        J.items.assign(I.items.begin(), I.items.end());
        for (auto it = J.items.begin(); it != J.items.end(); it++) {
            Item &L = *it;
            if (L.location < L.p.rights.size()) {
                string B = L.p.rights[L.location];
                if (isInN(B)) {
                    /* 先求出B后面的FIRST集 */
                    set<string> FS;
                    vector<string> beta_a;
                    beta_a.assign(L.p.rights.begin() + L.location + 1, L.p.rights.end());
                    beta_a.push_back(L.next);
                    FIRST(beta_a, FS);

                    for (int i = 0; i < grammar.prods.size(); i++) {
                        Production &P = grammar.prods[i];
                        if (P.left == B) {
                            for (auto it = FS.begin(); it != FS.end(); it++) {
                                string b = *it;
                                Item t;
                                t.location = 0;
                                t.next = b;
                                t.p.left = P.left;
                                t.p.rights.assign(P.rights.begin(), P.rights.end());
                                if (!isInItems(I, t)) {
                                    /* 标记改变 */
                                    change = true;
                                    I.items.push_back(t);
                                } 
                            }
                        } 
                    }
                }
            }
        }
    }
}
/* 判断是否在项目集规范族中，若在返回序号 */
int isInClusterOfItems(Items &I)
{
    for (int i = 0; i < COI.items.size(); i++) {
        Items &J = COI.items[i];
        bool flag = true;
        if (J.items.size() != I.items.size()) {
            flag = false;
            continue;
        }
        /* 每个项目都在该项目集中，则认为这个两个项目集相等 */
        for (auto it = I.items.begin(); it != I.items.end(); it++) {
            Item &t = *it;
            if (!isInItems(J, t)) {
                flag = false;
                break;
            }
        }
        if (flag) {
            return i + 1;
        }
    }
    return 0;
}

/* 转移函数，I为当前的项目集，J为转移后的项目集, 经X转移 */
void go(Items &I, string X, Items &J)
{
    for (auto it = I.items.begin(); it != I.items.end(); it++) {
        Item &L = *it;
        if (L.location < L.p.rights.size()) {
            string B = L.p.rights[L.location];
            if (B == X) {
                Item t;
                t.location = L.location + 1;
                t.next = L.next;
                t.p.left = L.p.left;
                t.p.rights.assign(L.p.rights.begin(), L.p.rights.end());
                J.items.push_back(t);
            }
        }
    }
    /* 若J中有项目，则求其闭包 */
    if (J.items.size() > 0) {
        closure(J);
    }
}

/* 构建DFA和项目集规范族 */
void DFA()
{
    /* 构建初始项目集 */
    Item t;
    t.location = 0;
    t.next = "$";
    t.p.left = grammar.prods[0].left;
    t.p.rights.assign(grammar.prods[0].rights.begin(), grammar.prods[0].rights.end());
    Items I;
    I.items.push_back(t);
    closure(I);
    /* 加入初始有效项目集 */
    COI.items.push_back(I);
    /* 把新加入的有效项目集加入待扩展队列中 */
    Q.push(pair<Items, int>(I, 0));
    
    while (!Q.empty()) {
        Items &S = Q.front().first;
        int sidx = Q.front().second;
        /* 遍历每个终结符 */
        for (int i = 0; i  < grammar.T.size(); i++) {
            Items D;
            go(S, grammar.T[i], D);
            int idx;
            /* 若不为空 */
            if (D.items.size() > 0) {
                /* 查找是否已经在有效项目集族里 */
                idx = isInClusterOfItems(D); 
                if (idx > 0) {
                    idx = idx - 1;
                } else {
                    idx = COI.items.size();
                    COI.items.push_back(D);
                    /* 把新加入的有效项目集加入待扩展队列中 */
                    Q.push(pair<Items, int>(D, idx));
                }
                /* 从原状态到转移状态加一条边，边上的值为转移符号 */
                COI.g[sidx].push_back(pair<int, string>(idx,grammar.T[i]));
            }
        }
        /* 遍历每个非终结符 */
        for (int i = 0; i  < grammar.N.size(); i++) {
            Items D;
            go(S, grammar.N[i], D);
            int idx;
            if (D.items.size() > 0) {
                /* 查找是否已经在有效项目集族里 */
                idx = isInClusterOfItems(D); 
                if (idx != 0) {
                    idx = idx - 1;
                } else {
                    idx = COI.items.size();
                    COI.items.push_back(D);
                    /* 把新加入的有效项目集加入待扩展队列中 */
                    Q.push(pair<Items, int>(D, idx));
                }
                /* 从原状态到转移状态加一条边，边上的值为转移符号 */
                COI.g[sidx].push_back(pair<int,string>(idx,grammar.N[i]));
            }
        }
    /* 当前状态扩展完毕，移除队列*/
    Q.pop();
    }
    
    printf("COI size: %d\n", COI.items.size());
    for (int i = 0; i < COI.items.size(); i++) {
        printf("Items %d:\n", i);
        printItems(COI.items[i]);
        for (int j = 0; j < COI.g[i].size(); j++) {
            pair<int,string> p= COI.g[i][j];
            cout<<"to "<<p.first<<" using "<<p.second<<endl;
        }
    }
}
/* 生成LR1分析表 */
void productLR1AnalysisTabel()
{
    for (int i = 0; i < COI.items.size(); i++) {
        Items &LIt= COI.items[i];
        /* 构建action表 */
        for (auto it = LIt.items.begin(); it != LIt.items.end(); it++) {
            Item &L = *it;
            /* 非规约项目 */
            if (L.location < L.p.rights.size()) {
                string a = L.p.rights[L.location];
                int j = isInT(a);
                /* a是终结符 */
                if (j > 0) {
                    j = j - 1; 
                    /* 找到对应a的出边，得到其转移到的状态 */
                    for (int k = 0; k < COI.g[i].size(); k++) {
                        pair<int,string> p = COI.g[i][k];
                        if (p.second == a) {
                            action[i][j].first = 1; // 1->S
                            action[i][j].second = p.first;  //转移状态
                            break;
                        }
                    }
                }
            } else { // 规约项目
                /* 接受项目 */
                if (L.p.left == grammar.prods[0].left) {
                    if (L.next == "$")
                        action[i][grammar.T.size() - 1].first = 3;
                } else {
                    /* 终结符 */
                    int  j = isInT(L.next) - 1;
                    /* 找到产生式对应的序号 */
                    for (int k = 0; k < grammar.prods.size(); k++) {
                        if (L.p == grammar.prods[k]) {
                            action[i][j].first = 2;
                            action[i][j].second = k;
                            break;
                        }
                    }

                }
            }
        }
        /* 构建goto表 */
        for (int k = 0; k < COI.g[i].size(); k++) {
            pair<int,string> p = COI.g[i][k];
            string A = p.second;
            int j = isInN(A);
            /* 终结符 */
            if (j > 0) {
                j = j - 1;
                gotoTable[i][j] = p.first; //转移状态
            }
        }
    }
    /* 打印LR1分析表 */
    printf("......正在打印分析表......");
    for (int i = 0; i < grammar.T.size() / 2 + 1; i++)
        Fout<<"\t";
    Fout<<"action";

    for (int i = 0; i < grammar.N.size() / 2 + grammar.T.size() / 2 + 1; i++)
        Fout<<"\t";
    Fout<<"goto\n";
    Fout<<"\t";
    for (int i = 0; i  < grammar.T.size(); i++) {
    	Fout<<grammar.T[i]<<"\t";
    }
    Fout<<"|\t";
    for (int i = 1; i  < grammar.N.size(); i++) {
        Fout<<grammar.N[i]<<"\t";
    }
    Fout<<"\n";
    for (int i = 0; i < COI.items.size(); i++) {
        Fout<<i<<"\t";
        for (int j = 0; j < grammar.T.size(); j++) {
            if (action[i][j].first == 1) {
                Fout<<'S'<<action[i][j].second<<"\t";
            } else if (action[i][j].first == 2) {
                Fout<<'R'<<action[i][j].second<<"\t";
            } else if (action[i][j].first == 3) { 
                Fout<<"ACC\t";
            } else {
                Fout<<"\t";
            }
        }
        Fout<<"|\t";
        for (int j = 1; j < grammar.N.size(); j++) {
            if (gotoTable[i][j]) {
                Fout<<gotoTable[i][j]<<"\t";
            } else {
                Fout<<"\t";
            }
        }
        Fout<<"\n";
    }
}


void initGrammar()
{
	ifstream F_grammar("grammar.txt");
    vector<string> vs;
    string s;
    while(getline(F_grammar,s)){
        vs = split(s,"-> ");
        Production tmp;
        tmp.left = vs[0];
        for(int i=1;i<vs.size();++i){
            tmp.rights.push_back(vs[i]);
        }
        grammar.prods.push_back(tmp);
    }

    string non;
    ifstream F_N("N.txt",ios::in);
    while(!F_N.eof()){
        F_N >> non;
        grammar.N.push_back(non);
    }
    string ter;
    ifstream F_T("T.txt",ios::in);
    while(!F_T.eof()){
        F_T >> ter;
        grammar.T.push_back(ter);
    }
    grammar.T.push_back("$");
    
    /* 求FIRST集 */
    getFirstSet();
    /* 构建DFA和LR1分析表 */
    DFA();
    productLR1AnalysisTabel();
    /* 读入待分析串 */
    string tempstr = "";
    ifstream F_STR("code.txt",ios::in);
    while(!F_STR.eof()){
        F_STR >> tempstr;
        str.push_back(tempstr);
    }
    str.push_back("$");
    /*初始化分析栈*/
    ST.push(pair<int, string>(0, "-"));
}

pair<int, string> node[1000];
/* 分析程序 */
void process()
{

    int father[1000];
    	for(int i=0;i<1000;++i){
        father[i]=-1;
	}
	vector<int>nodestack;
	int nodenum=0;
    int ip = 0;
    int hwnode=0;
    printf("\nThe ans:\n");
    do {
        int s = ST.top().first;
        string a = str[ip];
        int j = isInT(a) - 1;
        /* 移进 */
        if (action[s][j].first == 1) {
            ST.push(pair<int, string>(action[s][j].second, a));
            ip = ip + 1;
            node[hwnode++] = make_pair(nodenum,a);
            nodestack.push_back(nodenum++);
            
        }
        /* 规约 */
        else if (action[s][j].first == 2) {
            Production &P = grammar.prods[action[s][j].second];
            int newnodeid = nodenum++;
			node[hwnode++] = make_pair(newnodeid+1,P.left);
            /* 弹出并输出产生式 */
            cout<<P.left<<"->";
            for (int i = 0; i < P.rights.size(); i++) {

                if(!nodestack.empty()){
                	father[nodestack.back()] = newnodeid+1;
                	nodestack.pop_back();
				}
                ST.pop();
                cout<<P.rights[i]<<" ";
            }
            nodestack.push_back(newnodeid);
            printf("\n");
            s = ST.top().first;
            string A = P.left;
            j = isInN(A) - 1;
            ST.push(pair<int, string>(gotoTable[s][j], A));
        }
        /* 接受 */
        else if (action[s][j].first == 3) {
            father[nodestack.back()] = 0;
            printf("ACC\n");
            cout<<"打印语法树所需要的数组:"; 
            for(int i=0;;++i){
            	cout<<father[i]<<" ";
            	if(father[i]==0) break;
			}
			cout<<endl;
//			for(int i=0;i<20;i++){
//				cout<<node[i].first<<" "<<node[i].second<<endl;
//			}
            return;
        }
        /* action表中没有动作 */
        else {
            /* 若action表此时状态对应的空串Epsilon是移进动作，则将其移进分析栈中，再继续分析 */
            if(action[s][isInT("Epsilon") - 1].first == 1){
                ST.push(pair<int, string>(action[s][isInT("Epsilon") - 1].second, "Epsilon"));
                cout<<"-------------------移进空串----------------"<<"\n";
                //ip = ip + 1;
            }
            else{
                cout<<"error："<<"出错位置在"<<str[ip-1]<<"与"<<a<<"之间"<<endl;
                return;
            }
        }
    } while(1);

}


int main()
{
    initGrammar();
    process();
    return 0;
}
