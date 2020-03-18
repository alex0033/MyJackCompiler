#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include "SymbolTable.cpp"
#include "VMWriter.cpp"

class CompilationEngine{
  std::ifstream* ifs;
  std::ofstream* ofs;
  std::string line,className;
  int indent,ifLabelCount,loopLabelCount,fieldCount;
  SymbolTable st;
  VMWriter vmw;
  //indent:インデントの設定、enclosureに入ったら＋１、出たら−１

public:
  CompilationEngine(std::ifstream &ifile,std::ofstream &ofile,std::ofstream &OMedia){
    ifs=&ifile;
    ofs=&ofile;
    indent=0;
    ifLabelCount=0;
    loopLabelCount=0;
    fieldCount=0;
    st.open();
    vmw.open(OMedia);
    getline(*ifs,line);
    compileClass();
  }

  void makeIndent(){
    for(int i=0;i<indent;i++){
      *ofs<<" ";
    }
  }

  void normalOutput(){
    makeIndent();
    *ofs<<line<<std::endl;
  }

  void setLabel(std::string str){
    makeIndent();
    *ofs<<"<"<<str<<">"<<std::endl;
  }

  std::string outputLineElement(){
    std::string str;
    int startPoint=0,endPoint=0;
    for(int i=0;i<line.size();i++){
      if(line[i]=='>'&&endPoint==0){
        startPoint=i+2;
        continue;
      }
      if(line[i]=='<'&&startPoint!=0){
        endPoint=i-2;
        break;
      }
    }
    if(startPoint>endPoint){
      std::cout<<"elementError"<<std::endl;
      //return "elementError";
    }
    for(int i=startPoint;i<=endPoint;i++){
      str+=line[i];
    }
    return str;
  }

  std::string outputLineKind(){
    std::string str;
    int startPoint=0,endPoint=0;
    for(int i=0;i<line.size();i++){
      if(line[i]=='<'){
        startPoint=i+1;
        continue;
      }
      if(line[i]=='>'){
        endPoint=i-1;
        break;
      }
    }
    if(startPoint>endPoint){
      std::cout<<"elementError"<<std::endl;
      //return "elementError";
    }
    for(int i=startPoint;i<=endPoint;i++){
      str+=line[i];
    }
    return str;
  }

  void compileClass(){
    setLabel("class");  //<class>
    indent++;
    normalOutput();  //<keyword> class </keyword>
    getline(*ifs,line);
    normalOutput();  //<id> className </id>
    className=outputLineElement();
    getline(*ifs,line);
    normalOutput();  //<s> { </s>
    getline(*ifs,line);
    //advance();
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str=="static"||str=="field"){
        compileClassVarDec();
      }else if(str=="constructor"||str=="function"||str=="method"){
        compileSubroutineDec();
      }else if(str=="}"){
        break;
      }else{
        std::cout<<"errorClassBody:"<<line<<std::endl;
      }
    }
    normalOutput();  //<s> } </s>
    indent--;
    setLabel("/class");
  }

  //ここでは値のセット（シンボルテーブルに）のみ
  void compileClassVarDec(){
    setLabel("classVarDec");
    indent++;
    std::string kind=outputLineElement(); //ここでkindの把握
    normalOutput();  //<key> static||field </key>
    getline(*ifs,line);
    std::string type=outputLineElement(); //confirm type here
    normalOutput(); //<key|id> type </>
    while(getline(*ifs,line)){
      std::string str=outputLineElement();
      if(str==";"){
        break;
      }
      if(str!=","){
        st.define(str,type,kind);
        if(kind=="field"){
          fieldCount++;
        }
      }
      normalOutput();
    }
    normalOutput();
    indent--;
    setLabel("/classVarDec");
    getline(*ifs,line);
  }

  void compileSubroutineDec(){
    st.startSubroutine();
    //subroutineTableの初期化
    setLabel("subroutineDec");
    indent++;
    std::string whatSubroutine=outputLineElement();
    if(whatSubroutine=="method"){
      //ここあとで確認
      st.define("thisClassObjectForAgument","className","arg");
    }
    normalOutput();  //<key> method||function||constructor </key>
    getline(*ifs,line);
    std::string returnType=outputLineElement();
    normalOutput(); //<id||key> int||id.. </>
    getline(*ifs,line);
    normalOutput(); //<id> subroutineName </id>
    std::string subroutineName=className+"."+outputLineElement();
    getline(*ifs,line);
    normalOutput(); //<s> ( </s>
    getline(*ifs,line);
    int numberOfParameter=compileParameterList();
    normalOutput(); //<s> ) </s>
    getline(*ifs,line);
    setLabel("subroutineBody");
    indent++;
    normalOutput();  //<s> { </s>
    getline(*ifs,line);
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str=="var"){
        compileVarDec();
      }else{
        break;
      }
    }
    vmw.writeFunction(subroutineName,st.varCount("var")); //after change
    if(whatSubroutine=="constructor"){
      //thisにオブジェク（自分自身）をセット
      vmw.writePush("constant",fieldCount);
      vmw.writeCall("Memory.alloc",1);
      vmw.writePop("pointer",0);
    }else if(whatSubroutine=="method"){
      vmw.writePush("argument",0);
      vmw.writePop("pointer",0);
    }else{
      if(whatSubroutine!="function"){
        std::cout<<"functionError"<<std::endl;
      }
    }
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str=="}"){
        break;
      }
      if(str=="let"||str=="if"||str=="while"||str=="do"||str=="return"){
        compileStatements();
      }else{
        std::cout<<"subroutineStatementError:"<<line<<std::endl;
      }
    }
    normalOutput();  //<s> } </s>
    indent--;
    setLabel("/subroutineBody");
    indent--;
    setLabel("/subroutineDec");
    getline(*ifs,line);
    vmw.writeReturn();
    if(returnType=="void"){
      vmw.writePop("temp",1);
    }
    st.endSubroutine();
  }

  int compileParameterList(){
    int count=0;
    setLabel("parameterList");
    indent++;
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str==")"){
        break;
      }
      if(str!=","){
        std::string type=outputLineElement();
        normalOutput(); //<k|id> type </>
        getline(*ifs,line);
        std::string argName=outputLineElement();
        normalOutput(); //<id> argName </id>
        getline(*ifs,line);
        st.define(argName,type,"arg");
        count++;
        continue;
      }
      normalOutput();
      getline(*ifs,line);
    }
    indent--;
    setLabel("/parameterList");
    return count;
  }

  int compileVarDec(){
    int count=0;
    setLabel("varDec");
    indent++;
    std::string kind=outputLineElement();
    normalOutput(); //<k> var </k>
    getline(*ifs,line);
    std::string type=outputLineElement();
    normalOutput(); //<k>  varType <k>
    while(getline(*ifs,line)){
      std::string str=outputLineElement();
      if(str==";"){
        break;
      }
      normalOutput();
      if(str!=","){
        count++;
        st.define(str,type,kind);
      }
    }
    normalOutput();  //<s> ; </s>
    indent--;
    setLabel("/varDec");
    getline(*ifs,line);
    return count;
  }

  void compileStatements(){
    setLabel("statements");
    indent++;
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str=="let"){
        compileLet();
      }else if(str=="do"){
        compileDo();
      }else if(str=="while"){
        compileWhile();
      }else if(str=="return"){
        compileReturn();
      }else if(str=="if"){
        compileIf();
      }else{
        break;
        std::cout<<"stamentError"<<std::endl;
      }
    }
    indent--;
    setLabel("/statements");
  }

  std::string setPushPopCommand(std::string kind){
    std::string command;
    if(kind=="static"){
      command="static";
    }else if(kind=="field"){
      command="this"; //after change -> OK
    }else if(kind=="var"){
      command="local";
    }else{
      command="argument";
    }
    return command;
  }

  void compileLet(){
    setLabel("letStatement");
    indent++;
    normalOutput(); //<k> let </k>
    getline(*ifs,line);
    normalOutput(); //<id> varName </id>
    std::string str=outputLineElement();
    std::string command=setPushPopCommand(st.kindOf(str));
    int index=st.indexOf(str);
    getline(*ifs,line);
    std::string arrayStr=outputLineElement();
    if(arrayStr=="["){
      normalOutput();  //<s> [ </s>
      getline(*ifs,line);
      vmw.writePush(command,index);
      compileExpression(); //添字の値ゲット
      vmw.writeArithmetic("add");
      normalOutput();  //<s> ] </s>
      getline(*ifs,line);
    }
    normalOutput();  //<s> = </s>
    getline(*ifs,line);
    compileExpression();  //式の値ゲット
    if(arrayStr=="["){
      vmw.writePop("temp",1);
      vmw.writePop("pointer",1); //目的の配列へのアドレス設定
      vmw.writePush("temp",1);
      command="that";
      index=0;
    }
    vmw.writePop(command,index);
    normalOutput(); //<s> ; </s>
    indent--;
    setLabel("/letStatement");
    getline(*ifs,line);
  }

  //(expression)
  void expressionInBrackets(){
    normalOutput();  //<s> ( </s>
    getline(*ifs,line);
    compileExpression();
    normalOutput();  //<s> ) </s>
    getline(*ifs,line);
  }

  //{statements}
  void statementsInBrackets(){
    normalOutput();  //<s> { </s>
    getline(*ifs,line);
    compileStatements();
    normalOutput();  //<s> } </s>
    getline(*ifs,line);
  }

  std::string makeLabelName(std::string labelName,int count){
    std::stringstream ss;
    ss<<className<<"_"<<labelName<<"_"<<count;
    return ss.str();
  }

  void compileIf(){
    setLabel("ifStatement");
    indent++;
    normalOutput();  //<k> if </k>
    getline(*ifs,line);
    std::string ifLabel=makeLabelName("if",ifLabelCount)
    ,elseLabel=makeLabelName("else",ifLabelCount),
    outLabel=makeLabelName("ifOut",ifLabelCount);
    ifLabelCount++;
    expressionInBrackets(); //(expression)
    vmw.writeIf(ifLabel);
    vmw.writeGoto(elseLabel);
    vmw.writeLabel(ifLabel);
    statementsInBrackets(); //{statements}
    vmw.writeGoto(outLabel);
    vmw.writeLabel(elseLabel);
    if(outputLineElement()=="else"){
      normalOutput();  //<k> else </k>
      getline(*ifs,line);
      statementsInBrackets(); //{statements}
    }
    vmw.writeLabel(outLabel);
    indent--;
    setLabel("/ifStatement");
  }

  void compileWhile(){
    setLabel("whileStatement");
    indent++;
    normalOutput();  //<k> while </k>
    getline(*ifs,line);
    std::string loopLabel=makeLabelName("loop",loopLabelCount),
    outLabel=makeLabelName("loopOut",loopLabelCount),
    inLabel=makeLabelName("loopIn",loopLabelCount);
    loopLabelCount++;
    vmw.writeLabel(loopLabel);
    expressionInBrackets(); //(expression)
    vmw.writeIf(inLabel);
    vmw.writeGoto(outLabel);
    vmw.writeLabel(inLabel);
    statementsInBrackets(); //{statements}
    vmw.writeGoto(loopLabel);
    vmw.writeLabel(outLabel);
    indent--;
    setLabel("/whileStatement");
  }

  void compileDo(){
    setLabel("doStatement");
    indent++;
    normalOutput(); //<k> do </k>
    getline(*ifs,line);
    subroutineCall();
    normalOutput(); //<s> ; </s>
    indent--;
    setLabel("/doStatement");
    getline(*ifs,line);
  }

  void compileReturn(){
    setLabel("returnStatement");
    indent++;
    normalOutput();  //<s> return </s>
    getline(*ifs,line);
    if(outputLineElement()==";"){
      vmw.writePush("constant",0); //voidの処理
    }else{
      compileExpression();
    }
    normalOutput(); //<s> ; </s>
    indent--;
    setLabel("/returnStatement");
    getline(*ifs,line);
  }

  bool isOp(){
    std::string str=outputLineElement();
    if(str=="+"||str=="-"||str=="*"||str=="/"||str=="&amp;"||str=="|"
    ||str=="&lt;"||str=="&gt;"||str=="="){
      return true;
    }
    return false;
  }

  std::string setArithmeticCommand(){
    std::string str=outputLineElement();
    if(str=="+"){
      return "add";
    }else if(str=="-"){
      return "sub";
    }else if(str=="*"){
      return "call Math.multiply 2"; //あとで修正
    }else if(str=="/"){
      return "call Math.divide 2"; //あとで修正
    }else if(str=="&amp;"){
      return "and";
    }else if(str=="|"){
      return "or";
    }else if(str=="&lt;"){
      return "lt";
    }else if(str=="&gt;"){
      return "gt";
    }else if(str=="="){
      return "eq";
    }else if(str=="-"){
      std::cout<<"errorOp"<<std::endl;
      return "errorHere";
    }else{
      std::cout<<"ArithmeticCommandError"<<std::endl;
    }
  }

  void compileExpression(){
    setLabel("expression");
    indent++;
    compileTerm();
    while(!(ifs->fail())){
      if(isOp()){
        std::string arithmetic=setArithmeticCommand();
        normalOutput(); //<s> op </s>
        getline(*ifs,line);
        compileTerm();
        vmw.writeArithmetic(arithmetic);
      }else{
        break;
      }
    }
    indent--;
    setLabel("/expression");
  }

  int compileExpressionList(){
    int count=0;
    setLabel("expressionList");
    indent++;
    while(!(ifs->fail())){
      std::string str=outputLineElement();
      if(str==")"){
        break;
      }else if(str==","){
        normalOutput(); //<s> , </s>
        getline(*ifs,line);
      }else{
        count++;
        compileExpression();
      }
    }
    indent--;
    setLabel("/expressionList");
    return count;
  }

  void compileTerm(){
    setLabel("term");
    indent++;
    std::string str1=outputLineElement(),kind=outputLineKind();
    if(str1=="("){
      expressionInBrackets();  //(expression):()は他のカッコ
    }else if(str1=="-"||str1=="~"){
      normalOutput();  //<k> - </k>
      if(str1=="-"){
        vmw.writePush("constant",0);
        getline(*ifs,line);
        compileTerm();
        vmw.writeArithmetic("sub");
      }else{
        getline(*ifs,line);
        compileTerm();
        vmw.writeArithmetic("not");
      }
    }else if(str1=="true"){
      vmw.writePush("constant",1);
      vmw.writeArithmetic("neg");
      getline(*ifs,line);
    }else if(str1=="false"||str1=="nul"){
      vmw.writePush("constant",0);
      getline(*ifs,line);
    }else if(str1=="this"){
      vmw.writePush("pointer",0);
      getline(*ifs,line);
    }else{
      std::string copyLine=line; //文字列の保持
      auto pos=ifs->tellg();   //ポジションの保持
      getline(*ifs,line);
      std::string str2=outputLineElement();
      if(str2=="."||str2=="("){ //(->[
        line=copyLine; //巻き戻し（文字列）
        ifs->seekg(pos); //巻き戻し（ポジション）
        subroutineCall();
      }else{
        if(kind=="integerConstant"&&str2!="["){
          std::stringstream ss;
          ss<<str1;
          int n;
          ss>>n;
          vmw.writePush("constant",n);
        }else if(kind=="identifier"){
          int index=st.indexOf(str1);
          std::string type=st.typeOf(str1);
          std::string realkind=st.kindOf(str1);
          std::string command;
          if(realkind=="static"){
            command="static";
          }else if(realkind=="field"){
            command="this";
          }else if(realkind=="var"){
            command="local";
          }else{
            command="argument";
          }
          vmw.writePush(command,index);
        }else if(kind=="stringConstant"){
          int n=str1.size();
          vmw.writePush("constant",n);
          vmw.writeCall("String.new",1);
          vmw.writePop("pointer",1);
          for(int i=0;i<=n;i++){
            vmw.writePush("pointer",1);
          }
          for(int i=0;i<n;i++){
            int c=str1[i];
            vmw.writePush("constant",c);
            vmw.writeCall("String.appendChar",2);
            vmw.writePop("temp",2); //使わないだろう
          }
        }else{
          std::cout<<"eeeerror"<<std::endl;
        }
        makeIndent();
        *ofs<<copyLine<<std::endl;
      }
      if(str2=="["){
        normalOutput();  //<s> [ </s>
        getline(*ifs,line);
        compileExpression(); //ここを抜けたとき、SPの一番上は添字の値、その１個下は配列のアドレス
        vmw.writeArithmetic("add"); //たどり着きたいアドレスにセット完了
        vmw.writePop("pointer",1); //thatにセット
        vmw.writePush("that",0); //値をプッシュ
        normalOutput();  //<s> ] </s>
        getline(*ifs,line);
      }
    }
    indent--;
    setLabel("/term");
  }

  void subroutineCall(){
    std::string functionName,str1=outputLineElement();
    std::string kind=st.kindOf(str1);
    normalOutput();  //<id> id </id>
    getline(*ifs,line);
    int numberOfParameter=0;
    if(outputLineElement()=="."){
      normalOutput(); //<s> . </s>
      getline(*ifs,line);
      std::string str2=outputLineElement();
      if(kind=="none"){
        //function,constructor
        functionName=str1+"."+str2;
      }else{
        //method
        int index=st.indexOf(str1);
        std::string type=st.typeOf(str1);
        std::string command=setPushPopCommand(kind);
        vmw.writePush(command,index);
        functionName=type+"."+str2;
        numberOfParameter++;
      }
      normalOutput();  //<id> id </id>
      getline(*ifs,line);
    }else{
      //クラス内の関数
      //method(オブジェクトがそのクラス自身)
      vmw.writePush("pointer",0);
      numberOfParameter++;
      functionName=className+"."+str1;
    }
    normalOutput();  //<s> ( </s>
    getline(*ifs,line);
    numberOfParameter+=compileExpressionList();
    vmw.writeCall(functionName,numberOfParameter);
    normalOutput(); //<s> ) </s>
    getline(*ifs,line);
  }

};
