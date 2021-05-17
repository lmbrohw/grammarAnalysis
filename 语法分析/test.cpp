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
/*Epsilon ��ʾ�մ�����"��"*/
ofstream Fout("Analysis.txt", ios::out);
/*ʵ��split�����������LR1�ķ�ʱ��Ҫ*/
vector<string> split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;
    while(i != s.size()){
        //�ҵ��ַ������׸������ڷָ�������ĸ��
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
        //�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
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

/* ����ʽ�ṹ���󲿷��ź��Ҳ����Ŵ� */
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

/* LR1��Ŀ */
struct Item {
    Production p;
    int location;
    string next;//��ǰ������ 
};

/* LR1��Ŀ�� */
struct Items {
    vector<Item> items;
};

/* LR1��Ŀ���淶�� */
struct ClusterOfItems {
    /* ��Ч��Ŀ���ļ��� */
    vector<Items> items;
    /* ����DFA��ͼ��firstΪת�Ƶ���״̬��ţ�second�Ǿ�ʲôת�� */
    vector< pair<int, string> > g[1000];
}COI;

/* �ķ��ṹ�� */
struct Grammar {
    int num;  // ����ʽ����
    vector<string> T;   // �ս��
    vector<string> N;   // ���ս��
    vector<Production> prods;  //����ʽ
}grammar;

/* FIRST�� */
map<string, set<string> > first;

/* DFA���У� ���ڴ洢��ת�Ƶ���Ч��Ŀ�� */
queue< pair<Items, int> > Q;

/* action���goto�� */
pair<int, int> action[1000][1000]; // first��ʾ��������,1->S 2->R 3->ACC second��ʾSת��״̬����R����ʽ���
int gotoTable[1000][1000];

/* �������� */
vector<string> str;
/* ����ջ,����״̬ջ�ͷ���ջ*/
stack< pair<int, string> > ST; // first�ǵ�ǰ״̬��second �Ƿ���

/* �ж�ch�Ƿ����ս�� */
int isInT(string ch)
{
    for (int i = 0; i < grammar.T.size(); i++) {
        if (grammar.T[i] == ch) {
            return i + 1;//����ֱ�ӷ��ظ��ս����T�����е�λ�ã��������action��ά��ʹ��
        }
    }
    return 0;
}
/* �ж�ch�Ƿ��Ƿ��ս�� */
int isInN(string ch)
{
    for (int i = 0; i < grammar.N.size(); i++) {
        if (grammar.N[i] == ch) {
            return i + 1;//����ֱ�ӷ��ظ÷��ս����N�����е�λ�ã��������goto��ά��ʹ��
        }
    }
    return 0;
}
/* ��FIRST�� */
void getFirstSet()
{
    /* �ս����FIRST�����䱾�� */
    for (int i = 0; i < grammar.T.size(); i++) {
        string X = grammar.T[i];
        set<string> tmp;
        tmp.insert(X);
        first[X] = tmp;
    }
    /* �����ս����FIRST�������仯ʱѭ�� */
    bool change = true;
    while (change) {
        change = false;
        /* ö��ÿ������ʽ */
        for (int i = 0; i < grammar.prods.size(); i++) {
            Production &P = grammar.prods[i];
            string X = P.left;
            set<string> &FX = first[X];
            /* ����Ҳ���һ�������ǿջ������ս��������뵽�󲿵�FIRST���� */
            if (isInT(P.rights[0]) || P.rights[0] == "Epsilon") {
                /* �����Ƿ�FIRST���Ƿ��Ѿ����ڸ÷��� */
                auto it = FX.find(P.rights[0]);
                /* ������ */
                if (it == FX.end()) {
                    change = true; // ��עFIRST�������仯��ѭ������
                    FX.insert(P.rights[0]);
                }
            } else {
                /* ��ǰ�����Ƿ��ս�����ҵ�ǰ���ſ����Ƴ��գ������ж���һ������ */
                bool next = true;
                /* ���жϷ��ŵ��±� */
                int idx = 0;
                while (next && idx < P.rights.size()) {
                    next = false;
                    string Y = P.rights[idx];
                    set<string> &FY = first[Y];
                    for (auto it = FY.begin(); it != FY.end(); it++) {
                        /* �ѵ�ǰ���ŵ�FIRST���зǿ�Ԫ�ؼ��뵽�󲿷��ŵ�FIRST���� */
                        if (*it != "Epsilon") {
                            auto itt = FX.find(*it);
                            if (itt == FX.end()) {
                                change = true;
                                FX.insert(*it);
                            }
                        }
                    }
                    /* ��ǰ���ŵ�FIRST�����п�, ���nextΪ�棬idx�±�+1 */
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
/* ����beta_a����FIRST���� ���浽FS������ */
void FIRST(vector<string> &beta_a, set<string> &FS)
{  
    /* ��ǰ�����Ƿ��ս�����ҵ�ǰ���ſ����Ƴ��գ������ж���һ������ */
    bool next = true;
    int idx = 0;
    while (idx < beta_a.size() && next) {
        next = false;
        /* ��ǰ�������ս����գ����뵽FIRST���� */
        if (isInT(beta_a[idx]) || beta_a[idx] == "Epsilon") {
            /* �ж��Ƿ��Ѿ���FIRST���� */
            auto itt = FS.find(beta_a[idx]);
            if (itt == FS.end()) {
                FS.insert(beta_a[idx]);
            }
        } else {
            string B = beta_a[idx];
            set<string> &FB = first[B];
            for (auto it = first[B].begin(); it != first[B].end(); it++) {
                /* ��ǰ����FIRST�������գ����nextΪ�棬��������ǰѭ�� */
                if (*it == "Epsilon") {
                    next = true;
                    continue;
                }
                /* �ѷǿ�Ԫ�ؼ��뵽FIRST���� */
                auto itt = FS.find(*it);
                if (itt == FS.end()) {
                    FS.insert(*it);
                }
            }
        }
        idx = idx + 1;
    }
    /* ����������ʽ�Ҳ�ĩβnext��Ϊ�棬˵��beta_a�����ƿգ����ռ��뵽FIRST���� */
    if (next) {
        FS.insert("Epsilon");
    }
}

/* �ж���LR1��Ŀt������Ч��Ŀ��I�� */
bool isInItems(Items &I, Item &t)
{
    for (auto it = I.items.begin(); it != I.items.end(); it++) {
        Item &item = *it;
        if (item.p == t.p && item.location == t.location && item.next == t.next)
            return true;
    }
    return false;
}

/* ��ӡĳ����Ŀ�� */
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

/* ��I�ıհ� */
void closure(Items &I)
{
    bool change =  true;
    while (change) {
        change = false;
        Items J;
        /* ö��ÿ����Ŀ */
        J.items.assign(I.items.begin(), I.items.end());
        for (auto it = J.items.begin(); it != J.items.end(); it++) {
            Item &L = *it;
            if (L.location < L.p.rights.size()) {
                string B = L.p.rights[L.location];
                if (isInN(B)) {
                    /* �����B�����FIRST�� */
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
                                    /* ��Ǹı� */
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
/* �ж��Ƿ�����Ŀ���淶���У����ڷ������ */
int isInClusterOfItems(Items &I)
{
    for (int i = 0; i < COI.items.size(); i++) {
        Items &J = COI.items[i];
        bool flag = true;
        if (J.items.size() != I.items.size()) {
            flag = false;
            continue;
        }
        /* ÿ����Ŀ���ڸ���Ŀ���У�����Ϊ���������Ŀ����� */
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

/* ת�ƺ�����IΪ��ǰ����Ŀ����JΪת�ƺ����Ŀ��, ��Xת�� */
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
    /* ��J������Ŀ��������հ� */
    if (J.items.size() > 0) {
        closure(J);
    }
}

/* ����DFA����Ŀ���淶�� */
void DFA()
{
    /* ������ʼ��Ŀ�� */
    Item t;
    t.location = 0;
    t.next = "$";
    t.p.left = grammar.prods[0].left;
    t.p.rights.assign(grammar.prods[0].rights.begin(), grammar.prods[0].rights.end());
    Items I;
    I.items.push_back(t);
    closure(I);
    /* �����ʼ��Ч��Ŀ�� */
    COI.items.push_back(I);
    /* ���¼������Ч��Ŀ���������չ������ */
    Q.push(pair<Items, int>(I, 0));
    
    while (!Q.empty()) {
        Items &S = Q.front().first;
        int sidx = Q.front().second;
        /* ����ÿ���ս�� */
        for (int i = 0; i  < grammar.T.size(); i++) {
            Items D;
            go(S, grammar.T[i], D);
            int idx;
            /* ����Ϊ�� */
            if (D.items.size() > 0) {
                /* �����Ƿ��Ѿ�����Ч��Ŀ������ */
                idx = isInClusterOfItems(D); 
                if (idx > 0) {
                    idx = idx - 1;
                } else {
                    idx = COI.items.size();
                    COI.items.push_back(D);
                    /* ���¼������Ч��Ŀ���������չ������ */
                    Q.push(pair<Items, int>(D, idx));
                }
                /* ��ԭ״̬��ת��״̬��һ���ߣ����ϵ�ֵΪת�Ʒ��� */
                COI.g[sidx].push_back(pair<int, string>(idx,grammar.T[i]));
            }
        }
        /* ����ÿ�����ս�� */
        for (int i = 0; i  < grammar.N.size(); i++) {
            Items D;
            go(S, grammar.N[i], D);
            int idx;
            if (D.items.size() > 0) {
                /* �����Ƿ��Ѿ�����Ч��Ŀ������ */
                idx = isInClusterOfItems(D); 
                if (idx != 0) {
                    idx = idx - 1;
                } else {
                    idx = COI.items.size();
                    COI.items.push_back(D);
                    /* ���¼������Ч��Ŀ���������չ������ */
                    Q.push(pair<Items, int>(D, idx));
                }
                /* ��ԭ״̬��ת��״̬��һ���ߣ����ϵ�ֵΪת�Ʒ��� */
                COI.g[sidx].push_back(pair<int,string>(idx,grammar.N[i]));
            }
        }
    /* ��ǰ״̬��չ��ϣ��Ƴ�����*/
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
/* ����LR1������ */
void productLR1AnalysisTabel()
{
    for (int i = 0; i < COI.items.size(); i++) {
        Items &LIt= COI.items[i];
        /* ����action�� */
        for (auto it = LIt.items.begin(); it != LIt.items.end(); it++) {
            Item &L = *it;
            /* �ǹ�Լ��Ŀ */
            if (L.location < L.p.rights.size()) {
                string a = L.p.rights[L.location];
                int j = isInT(a);
                /* a���ս�� */
                if (j > 0) {
                    j = j - 1; 
                    /* �ҵ���Ӧa�ĳ��ߣ��õ���ת�Ƶ���״̬ */
                    for (int k = 0; k < COI.g[i].size(); k++) {
                        pair<int,string> p = COI.g[i][k];
                        if (p.second == a) {
                            action[i][j].first = 1; // 1->S
                            action[i][j].second = p.first;  //ת��״̬
                            break;
                        }
                    }
                }
            } else { // ��Լ��Ŀ
                /* ������Ŀ */
                if (L.p.left == grammar.prods[0].left) {
                    if (L.next == "$")
                        action[i][grammar.T.size() - 1].first = 3;
                } else {
                    /* �ս�� */
                    int  j = isInT(L.next) - 1;
                    /* �ҵ�����ʽ��Ӧ����� */
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
        /* ����goto�� */
        for (int k = 0; k < COI.g[i].size(); k++) {
            pair<int,string> p = COI.g[i][k];
            string A = p.second;
            int j = isInN(A);
            /* �ս�� */
            if (j > 0) {
                j = j - 1;
                gotoTable[i][j] = p.first; //ת��״̬
            }
        }
    }
    /* ��ӡLR1������ */
    printf("......���ڴ�ӡ������......");
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
    
    /* ��FIRST�� */
    getFirstSet();
    /* ����DFA��LR1������ */
    DFA();
    productLR1AnalysisTabel();
    /* ����������� */
    string tempstr = "";
    ifstream F_STR("code.txt",ios::in);
    while(!F_STR.eof()){
        F_STR >> tempstr;
        str.push_back(tempstr);
    }
    str.push_back("$");
    /*��ʼ������ջ*/
    ST.push(pair<int, string>(0, "-"));
}

pair<int, string> node[1000];
/* �������� */
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
        /* �ƽ� */
        if (action[s][j].first == 1) {
            ST.push(pair<int, string>(action[s][j].second, a));
            ip = ip + 1;
            node[hwnode++] = make_pair(nodenum,a);
            nodestack.push_back(nodenum++);
            
        }
        /* ��Լ */
        else if (action[s][j].first == 2) {
            Production &P = grammar.prods[action[s][j].second];
            int newnodeid = nodenum++;
			node[hwnode++] = make_pair(newnodeid+1,P.left);
            /* �������������ʽ */
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
        /* ���� */
        else if (action[s][j].first == 3) {
            father[nodestack.back()] = 0;
            printf("ACC\n");
            cout<<"��ӡ�﷨������Ҫ������:"; 
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
        /* action����û�ж��� */
        else {
            /* ��action���ʱ״̬��Ӧ�Ŀմ�Epsilon���ƽ������������ƽ�����ջ�У��ټ������� */
            if(action[s][isInT("Epsilon") - 1].first == 1){
                ST.push(pair<int, string>(action[s][isInT("Epsilon") - 1].second, "Epsilon"));
                cout<<"-------------------�ƽ��մ�----------------"<<"\n";
                //ip = ip + 1;
            }
            else{
                cout<<"error��"<<"����λ����"<<str[ip-1]<<"��"<<a<<"֮��"<<endl;
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
