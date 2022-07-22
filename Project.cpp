#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;


struct Pos{
    int x = 0;
    int y = 0;
};

struct Word{
    Word(string word){
        this->word = word;
    }
    string word;
    vector<Pos> pos;
    int Dir; // NF = 0, LR = 1, TD = 2.
};

struct Table{

    Table(string path) {
        ifstream inp(path);
        inp >> W >> H;
        table = new string*[H];
        for(int i = 0; i < H; i++){
            table[i] = new string[W];
            for(int j = 0; j < W; j++){
                inp >> table[i][j];
            }
        }
        string word;
        while((inp >> word) && (word != "#")){
            wordList.push_back(Word(word));
        }

        inp.close();
    }

private:
    int wordFound = 0;
    int W;
    int H;
    string** table;
    vector<Word> wordList;
};


int main(){

}