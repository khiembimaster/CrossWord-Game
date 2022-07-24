#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <map>
using namespace std;


struct Pos{
    Pos(int height, int weight, string direction): w{weight},h{height},Dir{direction} {}
    int w = 0;
    int h = 0;
    string Dir = "NOT FOUND"; // NF = 0, LR = 1, TD = 2.
};

struct TrieNode{
    TrieNode* children[26];
    TrieNode* failureLink = nullptr;
    vector<string> outputs;
};

TrieNode* getNode(){
    TrieNode* p = new TrieNode;
    for(int i = 0; i < 26; ++i){
        p->children[i] = nullptr;
    }
    return p;
}

void buildFailure(TrieNode *pRoot){
    pRoot->failureLink = pRoot;
    queue<TrieNode*> que;
    //Build First layer 
    for(int i = 0; i < 26; i++){
        if(!pRoot->children[i])
            continue;
        que.push(pRoot->children[i]);
        pRoot->children[i]->failureLink = pRoot;
    }

    //Build the rest
    while(que.size()){
        TrieNode* cursor = que.front();
        que.pop();
        for(int i = 0; i < 26; i++){
            if(!cursor->children[i])
                continue;
            TrieNode* thisChild = cursor->children[i];
            TrieNode* suffix = cursor->failureLink;
            while(!(suffix->failureLink->children[i]) && suffix != pRoot)
                suffix = suffix->failureLink;
            
            if(suffix->children[i])
                thisChild->failureLink = suffix->children[i];
            else thisChild->failureLink = pRoot;

            que.push(thisChild);
        }

        for(string iter : cursor->failureLink->outputs)
            cursor->outputs.push_back(iter);
    
    }
        
}

struct Table{

    Table(string path) {
        ifstream inp(path);
        inp >> W >> H;
        //create table
        table = new char*[H+1];
        for(int i = 1; i <= H; i++){
            table[i] = new char[W+1];
            for(int j = 1; j <= W; j++){
                inp >> table[i][j];
            }
        }
        string word;
        this->pRoot = getNode();
        //create trie
        while((inp >> word) && (word != "#")){
            TrieNode* cursor = pRoot;
            this->wordsList[word].push_back(Pos(0,0,"NF"));
            for(int i = 0; i<word.size(); ++i){
                int index = word[i] - 'A';
                if(!cursor->children[index])
                    cursor->children[index] = getNode();
                cursor = cursor->children[index];
            }
            cursor->outputs.push_back(word);
        }

        inp.close();

        buildFailure(this->pRoot);
    }

    void search(){
        
        string Direction = "LR";
        for(int h = 1; h <= this->H; ++h){
            TrieNode* parent = this->pRoot;
            for(int w = 1; w <= this->W; ++w){
                if(parent->children[table[h][w]-'A']) {
                    parent = parent->children[table[h][w]-'A'];
                    if(parent->outputs.size()){
                        for(string out : parent->outputs){
                            if(wordsList[out].size() == 1) ++wordFound;
                            wordsList[out].push_back(Pos(h,w-out.size()+1,Direction));
                        }
                    }
                }
                else{
                    while(parent != pRoot && !parent->children[table[h][w]-'A'])
                        parent = parent->failureLink;
                    
                    if(parent->children[table[h][w]-'A'])
                        parent = parent->children[table[h][w]-'A'];
                }
            }
        }


        Direction = "TD";
        for(int w = 1; w <= this->W; ++w){
            TrieNode* parent = this->pRoot;
            for(int h = 1; h <= this->H; ++h){
                if(parent->children[table[h][w]-'A']) {
                    parent = parent->children[table[h][w]-'A'];
                    if(parent->outputs.size()){
                        for(string out : parent->outputs){
                            if(wordsList[out].size() == 1) ++wordFound;
                            wordsList[out].push_back(Pos(h-out.size()+1,w,Direction));
                        }
                    }
                }
                else{
                    while(parent != pRoot && !parent->children[table[h][w]-'A'])
                        parent = parent->failureLink;
                    
                    if(parent->children[table[h][w]-'A'])
                        parent = parent->children[table[h][w]-'A'];
                }
            }
        }
    }

public:
    int wordFound = 0;
    int W;
    int H;
    char** table;
    TrieNode* pRoot;
    map<string, vector<Pos>> wordsList;
};




int main(){
    Table myGame("input.txt");
    myGame.search();
    ofstream out("output.txt");
    out << myGame.wordFound << endl;
    for(auto it : myGame.wordsList){
        out << it.first << " ";
        if(it.second.size() == 1)
            out << "(" << 0 << "," << 0 << ")" << " " << "NF" << "| " << endl;
        else {
            for(Pos pos : it.second){
                if(pos.Dir == "NF") continue;
                out << "(" << pos.h << "," << pos.w << ")" << " " << pos.Dir << "| ";
            }
            out << endl;
        }
    }
    out.close();
}