# MyJackCompiler
このプログラムは『コンピュータシステムの理論と実装』の10章と11章を実装したもの。Jackファイルをvmファイルにコンパイルする。

JackAnalyzer.sh
コマンドラインにおいて、ディレクトリをコンパイルするためのプログラム。（引数としてディレクトリをひとつだけ指定する）intermediaFileを生成する。

JackAnalyzer.cpp
コンパイラのメインプログラム

JackTokennizer.cpp
JackTokennizerを作成するためのプログラム。

CompilarionEngine.cpp
JackTokennizerを元に構文解析を行う。

SymbolTable.cpp
シンボルテーブルの作成、管理

VMWriter.cpp
vmファイルへの書き込みを行う。
