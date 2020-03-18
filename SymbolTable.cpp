#include<iostream>
#include<vector>

struct HashTable{
  std::string name,type,kind;
  int index;
};

class SymbolTable{
  std::vector<HashTable> cht;
  std::vector<std::vector<HashTable>> sht;
  int staC,fieC,shtIndex;
  std::vector<int> argC,varC;

public:
  SymbolTable(){
    //do nothing
  }

  void open(){
    staC=0;
    fieC=0;
    shtIndex=-1;
  }

  void startSubroutine(){
    std::vector<HashTable> ht;
    sht.push_back(ht);
    argC.push_back(0);
    varC.push_back(0);
    shtIndex++;
    argC[shtIndex]=0;
    varC[shtIndex]=0;
  }

  void endSubroutine(){
  if(shtIndex>=0){
    std::cout<<std::endl<<shtIndex<<": "<<"argcount::"<<argC[shtIndex]<<std::endl;
  }
  for(int i=0;i<sht[shtIndex].size();i++){
    std::cout<<"name:"<<sht[shtIndex][i].name<<" type:"<<sht[shtIndex][i].type
    <<" kind:"<<sht[shtIndex][i].kind<<" index:"<<sht[shtIndex][i].index
    <<std::endl;
  }
    sht.pop_back();
    shtIndex--;
  }

  void define(std::string name,std::string type,std::string kind){
    int index=0;
    if(kind=="static"||kind=="field"){
      if(kind=="static"){
        index=staC;
        staC++;
      }else if(kind=="field"){
        index=fieC;
        fieC++;
      }else{
        std::cout<<"There is nothing such a kind in the classScoop"<<std::endl;
      }
      HashTable ht;
      ht.name=name;
      ht.type=type;
      ht.kind=kind;
      ht.index=index;
      cht.push_back(ht);
    }else{
      if(kind=="arg"){
        index=argC[shtIndex];
        argC[shtIndex]++;
      }else if(kind=="var"){
        index=varC[shtIndex];
        varC[shtIndex]++;
      }else{
        std::cout<<"There is nothing such a kind in the subroutineScoop"<<std::endl;
      }
      HashTable ht;
      ht.name=name;
      ht.type=type;
      ht.kind=kind;
      ht.index=index;
      sht[shtIndex].push_back(ht);
    }
  }

  int varCount(std::string kind){
    if(kind=="static"){
      return staC;
    }else if(kind=="field"){
      return fieC;
    }else if(kind=="arg"){
      return argC[shtIndex];
    }else if(kind=="var"){
      return varC[shtIndex];
    }else{
      std::cout<<"varCountError -> return -1"<<std::endl;
      return -1;
    }
  }

  std::string kindOf(std::string name){
    if(shtIndex!=-1){
      int n=sht[shtIndex].size();
      for(int i=0;i<n;i++){
        if(sht[shtIndex][i].name==name){
          return sht[shtIndex][i].kind;
        }
      }
    }
    int n=cht.size();
    for(int i=0;i<n;i++){
      if(cht[i].name==name){
        return cht[i].kind;
      }
    }
    return "none";
  }

  std::string typeOf(std::string name){
    if(shtIndex!=-1){
      int n=sht[shtIndex].size();
      for(int i=0;i<n;i++){
        if(sht[shtIndex][i].name==name){
          return sht[shtIndex][i].type;
        }
      }
    }
    int n=cht.size();
    for(int i=0;i<n;i++){
      if(cht[i].name==name){
        return cht[i].type;
      }
    }
    return "none";
  }

  int indexOf(std::string name){
    if(shtIndex!=-1){
      int n=sht[shtIndex].size();
      for(int i=0;i<n;i++){
        if(sht[shtIndex][i].name==name){
          return sht[shtIndex][i].index;
        }
      }
    }
    int n=cht.size();
    for(int i=0;i<n;i++){
      if(cht[i].name==name){
        return cht[i].index;
      }
    }
    std::cout<<"varIndexOf -> return -1"<<std::endl;
    return -1;
  }

};
