#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
using namespace std;

class IDecryptor
{
public:
    virtual string decrypt(string const & filename) = 0;
};
string XOR(string text,unsigned char K)
{
    string decryptedText = "";
    for(int i = 0; i < text.size(); i++)
    {
        int x = text[i];
        int k = K;
        int y = x ^ k;
        char a = y;
        decryptedText += a;
    }
    return decryptedText;
}

string CEZAR(string text, int d) //d - distanta
{
    for (int i = 0; i < text.size(); i++)
    {
        char c = text[i];
        if(isalpha(c))
        {
            c = text[i];
            text[i] -= d;
            if((islower(c) && text[i] < 'a') || (isupper(c) && text[i] < 'A'))
            {
                text[i] += 26;
            }
        }
    }
    return text;
}
string ROTATIE(string s, int d)
{
    reverse(s.begin(), s.begin()+d);
    reverse(s.begin() + d, s.end());
    reverse(s.begin(), s.end());
    return s;
}
vector<int> XOR2(vector<int>s, string x)
{
    vector<int> result(s.size(),0);
    for(int i = 0; i < s.size();i++)
    {
        char y = x[i%x.size()] - '0';
        int a = y;
        result[i] = s[i] ^ a ;
    }
    return result;
}
vector<int> ROTATIE2(vector<int> s, int d)
{
    reverse(s.begin(), s.begin()+d);
    reverse(s.begin()+d, s.end());
    reverse(s.begin(), s.end());
    return s;
}
vector<int> INVERSARE(vector<int> v)
{
    vector<int> reversed;
    for(int i = v.size()-1; i >=0; i--)
    {
        reversed.push_back(v[i]);
    }
    return reversed;
}
class Decryptor:public IDecryptor
{
private:
    string descriptionText;
    string descriptionBlock;
    string contorText;
    string contorBlock;
public:
    string decrypt(string const & filename)
    {
        string text;
        ifstream f(filename, ios::binary);
        char c[9];
        vector<vector<int>>control;
        map<int,int> lastIndex;
        int indexBlock = 0;
        int indexControl = 0;
        while (f.read(c,8*sizeof(char)))
        {
            vector<int>blockControl;
            for(int i = 0; i < 8;i++)
            {
                for(int j=0; j < 8; j++)
                {
                    blockControl.push_back(((c[i] >>(7- j)) & 1));
                }
            }
            if(descriptionBlock == "INVERSARE")
            {
                blockControl = INVERSARE(blockControl);
            }
            else if(descriptionBlock == "ROTATIE")
            {
                blockControl = ROTATIE2(blockControl, stoi(contorBlock));
            }
            else if(descriptionBlock == "XOR")
            {
                blockControl = XOR2(blockControl, contorBlock);
            }
            for(int i = 0; i < 8; i++)
            {
                c[i] = 0;
                for(int j = 0; j < 8; j ++)
                {
                    c[i] += blockControl[i*8+j] * pow(2,(7-j));
                }
            }
            control.push_back(blockControl);
            for(int k = 0; k < 6; k ++)
            {
                if(c[k] != '\0')
                {
                    text += c[k];
                }
            }
            if(c[6] >= 0)
            {
                f.seekg((int)c[6]*8);
                lastIndex[indexControl] = indexBlock;
                indexBlock = ((int)c[6]);
                indexControl ++;
            }
            else
            {
                lastIndex[indexControl] = indexBlock;
                break;
            }
         }
        vector<int> result;
        string decryptedBlockText = "";
        for(int i = 0; i < control.size();i++ )
        {
            result = control[i];
            for(int j = 0; j < 8; j++)
            {
                if(result[56+j] !=result[48+j]^result[40+j]^result[32+j]^result[24+j]^result[16+j]^result[8+j]^result[j])
                {
                    string a = "Blocul cu indexul ";
                    string b = " a fost alterat!";
                    string s =  a + to_string(lastIndex[i]) +  b;
                    return s;
                }
            }
        }
        if(descriptionText == "ROTATIE")
            text = ROTATIE(text,stoi(contorText));
        if(descriptionText == "CEZAR")
            text = CEZAR(text,stoi(contorText));
        if(descriptionText == "XOR")
        {
            char x = contorText[0];
            text = XOR(text,x);
        }
        return text;
    }
    Decryptor(string description)
    {
        istringstream iss(description);
        iss >> descriptionText;
        if(descriptionText == "ROTATIE" || descriptionText == "CEZAR" || descriptionText == "XOR")
        {
            iss >> contorText;
        }
        iss >>  descriptionBlock;
        if(descriptionBlock == "ROTATIE" || descriptionBlock == "XOR")
        {
            iss >> contorBlock;
        }
    }
};
class DecryptorFactory:public IDecryptor
{
public:
    static IDecryptor *makeDecryptor(string const & description)
    {
       Decryptor* D = new Decryptor(description);
       return D;
    }
};
