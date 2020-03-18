#include<iostream>
#include<fstream>
#include<sstream>

class JackTokenizer{
  std::ifstream *ifs;
  std::string str,st;
  //その時点で扱う文字列（一行）
  //その時点で扱う文字列（トークン）
  int sacheck=0;

public:
  JackTokenizer(std::ifstream &in){
    ifs=&in;
  }

  bool hasMoreTokens(){
    advance();
    if(ifs->fail()){
      return false;
    }
    return true;
  }

  void advance(){
    if(ifs->fail()){
      return;
    }
    std::string s;
    getline(*ifs,s);
    str=s;
    int n=str.size();
    int scheck=0;
//std::cout<<"str1:"<<str<<std::endl;
    for(int i=0;i<n;i++){
      if(str[i]=='/'&&str[i+1]=='/'&&scheck==0&&sacheck==0){
        scheck=1;
      }
      if(str[i]=='/'&&str[i+1]=='*'&&sacheck==0&&scheck==0){
        sacheck=1;
      }
      if(str[i]=='*'&&str[i+1]=='/'){
        str.erase(str.begin()+i);
        str.erase(str.begin()+i);
        i-=2;
        n-=2;
        sacheck=0;
      }
      if(scheck==1||sacheck==1){
        str.erase(str.begin()+i);
        i--;
        n--;
      }
    }
//std::cout<<"str2:"<<str<<std::endl;
    if(shoudSkipp(str)){
      if(ifs->fail()){
        return;
      }
      advance();
      return;
    }
  }

  //無視するべき一行の場合trueを返す
  bool shoudSkipp(std::string s){
    int n=s.size(),count=0;
    if(n==0){
      return true;
    }
    for(int i=0;i<n;i++){
      if(s[i]==' '||s[i]=='\t'||s[i]=='\b'||s[i]=='\n'
      ||s[i]=='\r'||s[i]=='\v'){
        count++;
      }
    }
    if(count==n){
      return true;
    }
    return false;
  }

  bool isSymbol(std::string s){
    if(st.size()==1&&(s=="{"||s=="}"||s=="("||s==")"||s=="["||s=="]"
    ||s=="."||s==","||s==";"||s=="+"||s=="-"||s=="*"||s=="/"||s=="&"
    ||s=="|"||s=="<"||s==">"||s=="="||s=="~")){
      return true;
    }
    return false;
  }

  bool isSymbol(char s){
    if(s=='{'||s=='}'||s=='('||s==')'||s=='['||s==']'||s=='.'||s==','
    ||s==';'||s=='+'||s=='-'||s=='*'||s=='/'||s=='&'||s=='|'||s=='<'
    ||s=='>'||s=='='||s=='~'){
      return true;
    }
    return false;
  }

  //おそらくsplitStrが必要

  std::vector<std::string> splitStr(){
// std::cout<<"line:"<<str<<std::endl;
    std::vector<std::string> vs;
    int n=str.size(),inString=0,reset=1,vsIndex=0;
    //inString:””の中にいるか、すなわち文字列中かどうか
    //reset:リセットするか、すなわち新たな文字列を作り始める必要があるか
    //vsIndex:次のインデックスがはいっている。
    for(int i=0;i<n;i++){
//std::cout<<"resetN::"<<reset<<std::endl;
      if(str[i]=='\"'){
//std::cout<<"koko:"<<str[i]<<std::endl;
        if(inString==0){
          inString=1;
        }else if(inString==1){
          inString=0;
        }else{
          std::cout<<"inStringError"<<std::endl;
        }
      }

      if((str[i]==' '||str[i]=='\t'||str[i]=='\b'||str[i]=='\n'
      ||str[i]=='\r'||str[i]=='\v')&&inString==0){
//std::cout<<"blank::"<<str[i]<<std::endl;
        reset=1;
        continue;
      }

      if(isSymbol(str[i])&&inString==0){
        vs.push_back("");
        vs[vsIndex]+=str[i];
        vsIndex++;
        reset=1;
        continue;
      }

      if(reset==1){
        vs.push_back("");
        vs[vsIndex]+=str[i];
        vsIndex++;
        reset=0;
        continue;
      }
      if(reset==0){
        vs[vsIndex-1]+=str[i];
        continue;
      }
    }
    return vs;
  }

  void setSt(std::string s){
    st=s;
  }

  std::string tokenType(){
    //stだとする
    if(st=="class"||st=="method"||st=="function"||st=="constructor"
    ||st=="int"||st=="boolean"||st=="char"||st=="void"||st=="var"
    ||st=="static"||st=="field"||st=="let"||st=="do"||st=="if"
    ||st=="else"||st=="while"||st=="return"||st=="true"||st=="false"
    ||st=="null"||st=="this"){
      return "keyword";
    }else if(isSymbol(st)){
      return "symbol";
    }else if(st[0]=='\"'){
      return "stringConstant";
    }else if(st[0]>='0'&&st[0]<='9'){
      return "integerConstant";
    }else{
      return "identifier";
    }
  }

  std::string keyword(){
    return st;
  }

  std::string symbol(){
    if(st==">"){
      return "&gt;";
    }
    if(st=="<"){
      return "&lt;";
    }
    if(st=="&"){
      return "&amp;";
    }
    return st;
  }

  std::string identifier(){
    return st;
  }

  std::string stringVal(){
    int n=st.size();
    st.erase(st.begin()+n-1);
    st.erase(st.begin());
    return st;
  }

  int intVal(){
    std::stringstream ss;
    ss<<st;
    int num;
    ss>>num;
    return num;
  }

};
