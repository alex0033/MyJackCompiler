#include<iostream>
#include<vector>
#include<fstream>
#include "JackTokenizer.cpp"
#include "CompilationEngine.cpp"

std::string getHeadName(std::string str){
//std::cout<<"come on"<<std::endl;
  int n=str.size(),check=0;
  for(int i=0;i<n-1;i++){
    if(str[i]=='/'){
//std::cout<<str<<std::endl;
      str=str.substr(i+1);
      n-=i+1;
      i=-1;
    }
    if(str[i]==' '){
      str.erase(str.begin()+n-1);
      n--;
      i--;
    }
  }
  if(str[n-1]=='/'){
    str.erase(str.begin()+n-1);
  }
  return str;
}

int main(){
  std::string directoryPath,str,outPutFileName,vmFile;
  //tokenizerFileName="/home/alex/path/JackCompiler/JackTokenizer";
  std::vector<std::string> rightStr,xmlStr,vmStr,tokenizerStr;
  std::ifstream intermedia("/home/alex/path//JackCompiler/intermediaFile");
  intermedia>>directoryPath;
  str=getHeadName(directoryPath);
  directoryPath+="/";
  outPutFileName=directoryPath;

  while(intermedia>>str){
    int n=str.size(),check=0;
    std::string originalSt;
    for(int i=0;i<n;i++){
      if(str[i]=='.'&&str[i+1]=='j'&&str[i+2]=='a'&&str[i+3]=='c'&&str[i+4]=='k'){
        check=1;
        break;
	    }
      originalSt+=str[i];
    }
    if(check==1){
      std::string st=directoryPath+str,
      forXml=directoryPath+"xml/"+originalSt+".xml",
      forVm=directoryPath+originalSt+".vm",
      forTok=directoryPath+"tokenizer/"+originalSt+".tok";
      rightStr.push_back(st);
      xmlStr.push_back(forXml);
      vmStr.push_back(forVm);
      tokenizerStr.push_back(forTok);
    }
    if(intermedia.fail())break;
  }
  intermedia.close();
  //ここまでが入力ファイル、出力ファイル名の設定

  int n=rightStr.size();
  //ここからファイルごとに処理
  for(int i=0;i<n;i++){
    std::ifstream ifs(rightStr[i]);
    JackTokenizer jt(ifs);
    std::ofstream ot(tokenizerStr[i]);
    //ここから一行ずつの処理（標準入力一個分）
    while(jt.hasMoreTokens()){
      std::vector<std::string> splitStr=jt.splitStr();
      int m=splitStr.size();
      //ここからトークンごとの処理
      for(int j=0;j<m;j++){
        jt.setSt(splitStr[j]);
        std::string tokenType=jt.tokenType();
        if(tokenType=="keyword"){
          ot<<"<keyword> "<<jt.keyword()<<" </keyword>"<<std::endl;
        }else if(tokenType=="symbol"){
          ot<<"<symbol> "<<jt.symbol()<<" </symbol>"<<std::endl;
        }else if(tokenType=="identifier"){
          ot<<"<identifier> "<<jt.identifier()<<" </identifier>"<<std::endl;
        }else if(tokenType=="stringConstant"){
          ot<<"<stringConstant> "<<jt.stringVal()<<" </stringConstant>"<<std::endl;
        }else if(tokenType=="integerConstant"){
          ot<<"<integerConstant> "<<jt.intVal()<<" </integerConstant>"<<std::endl;
        }else{
          std::cout<<"cannot classify:"<<splitStr[j]<<std::endl;
        }
      }
    }
    std::ifstream itokenStream(tokenizerStr[i]);
    std::ofstream otokenStream(xmlStr[i]);
    std::ofstream vmStream(vmStr[i]);
    CompilationEngine comp(itokenStream,otokenStream,vmStream);
  }
  
}
