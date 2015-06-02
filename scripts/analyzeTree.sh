g++ -c `root-config --cflags --glibs --libs` -std=c++11 analyzeTree.cpp treeAnalyzer.cpp treeAnalyzerTDC.cpp
g++ -o analyzeTree analyzeTree.o treeAnalyzer.o treeAnalyzerTDC.o `root-config --cflags --glibs --libs`
./analyzeTree $1
