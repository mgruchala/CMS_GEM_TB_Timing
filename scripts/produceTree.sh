#rm H2_Data.root

g++ -gdwarf-3 -c `root-config --cflags --glibs --libs` -std=c++11 produceTDCTTree.cpp TRunParameters.cpp
g++ -gdwarf-3 -o produceTDCTTree produceTDCTTree.o TRunParameters.o `root-config --cflags --glibs --libs` -L $ROOTSYS/lib/ -lSpectrum
./produceTDCTTree
