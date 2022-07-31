#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <map>
using namespace std;


struct Pos{
    //fast initializing
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
    //Breadth first order traversal
    queue<TrieNode*> que;
    //Build First layer 
    for(int i = 0; i < 26; i++){
        if(!pRoot->children[i])
            continue;
        que.push(pRoot->children[i]);
        //The first layer TrieNode will only point to root 
        pRoot->children[i]->failureLink = pRoot;
    }

    //Build the rest
    while(que.size()){
        //Get current node
        TrieNode* cursor = que.front();
        que.pop();
        //For each children
        for(int i = 0; i < 26; i++){
            if(!cursor->children[i])
                continue; // skip if this is not a children
            
            TrieNode* thisChild = cursor->children[i]; 

            //find Longest common suffix
            TrieNode* suffix = cursor->failureLink;
            while(!(suffix->failureLink->children[i]) && suffix != pRoot)
                suffix = suffix->failureLink;
            
            //if this suffix has a child with the same key as thisChild, set thisChild's failureLink to that suffix's child
            if(suffix->children[i])
                thisChild->failureLink = suffix->children[i];
            else thisChild->failureLink = pRoot; // there are no common suffix 

            //Push thisChild to que for later layer traversing
            que.push(thisChild);
        }
        // inherit outputs
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
        //create trie
        string word;
        this->pRoot = getNode();
        while((inp >> word) && (word != "#")){
            TrieNode* cursor = pRoot;
            this->wordsList[word].push_back(Pos(0,0,"NF")); // set output default value as Not found - a placeholder
            for(int i = 0; i<word.size(); ++i){
                int index = word[i] - 'A';// get the appropriate index from ASCII code
                if(!cursor->children[index])// empty node so create new one
                    cursor->children[index] = getNode(); 
                cursor = cursor->children[index]; //follow the next level
            }
            cursor->outputs.push_back(word); // mark that the current pattern ends at this node
        }

        inp.close();

        buildFailure(this->pRoot); 
    }

    void search(){
        
        //Search row by row
        string Direction = "LR";
        for(int h = 1; h <= this->H; ++h){
            TrieNode* parent = this->pRoot;
            for(int w = 1; w <= this->W; ++w){
                if(parent->children[table[h][w]-'A']) { //There is a link, Follow it
                    parent = parent->children[table[h][w]-'A'];
                    if(parent->outputs.size()){ // this node of Trie is an end of one or some patterns
                        for(string out : parent->outputs){
                            if(wordsList[out].size() == 1) ++wordFound; //this is the first time we match this pattern, Increase word found.
                            wordsList[out].push_back(Pos(h,w-out.size()+1,Direction));// Store the Position to wordList
                        }
                    }
                }
                else{ //Link end/ fail/ unmatch
                    while(parent != pRoot && !parent->children[table[h][w]-'A']) //find the other path with common suffix
                        parent = parent->failureLink; 
                    
                    if(parent->children[table[h][w]-'A']) //There is a link
                        parent = parent->children[table[h][w]-'A']; // re-direct the parent pointer to where the process can continue.
                }
            }
        }

        //Search column by cloumn (the same)
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

    //Out put
    ofstream out("output.txt");
    out << myGame.wordFound << endl;
    // for each key-value pair int map wordsList
    for(auto it : myGame.wordsList){
        out << it.first << " "; // string pattern 
        if(it.second.size() == 1) // Not found
            out << "(" << 0 << "," << 0 << ")" << " " << "NF" << "| " << endl;
        else { // Founded
            for(Pos pos : it.second){
                if(pos.Dir == "NF") continue; // Since I initialized all the components in wordsList with a placeholders - Pos(0,0,"NF"). Let skip this.  
                out << "(" << pos.h << "," << pos.w << ")" << " " << pos.Dir << "| "; // write down Pos
            }
            out << endl;
        }
    }
    out.close();
}