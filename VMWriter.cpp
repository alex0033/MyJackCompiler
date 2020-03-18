#include<iostream>
#include<fstream>
#include<sstream>

class VMWriter{
  std::ofstream* ofs;

public:
  VMWriter(){
    //do nothing
  }

  void open(std::ofstream& file){
    ofs=&file;
  }

  void writePush(std::string segment,int index){
    *ofs<<"push "<<segment<<" "<<index<<std::endl;
  }

  void writePop(std::string segment,int index){
    *ofs<<"pop "<<segment<<" "<<index<<std::endl;
  }

  void writeArithmetic(std::string command){
    *ofs<<command<<std::endl;
  }

  void writeLabel(std::string label){
    *ofs<<"label "<<label<<std::endl;
  }

  void writeGoto(std::string label){
    *ofs<<"goto "<<label<<std::endl;
  }

  void writeIf(std::string label){
    *ofs<<"if-goto "<<label<<std::endl;
  }

  void writeCall(std::string name,int nArgs){
    *ofs<<"call "<<name<<" "<<nArgs<<std::endl;
  }

  void writeFunction(std::string name,int nLocals){
    *ofs<<"function "<<name<<" "<<nLocals<<std::endl;
  }

  void writeReturn(){
    *ofs<<"return"<<std::endl;
  }

};
