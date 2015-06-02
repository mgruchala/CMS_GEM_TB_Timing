g++ -gdwarf-3 -c `root-config --cflags --glibs --libs` -std=c++11 printTree.cpp TRunParameters.cpp
g++ -gdwarf-3 -o printTree printTree.o TRunParameters.o `root-config --cflags --glibs --libs` -L $ROOTSYS/lib/ -lSpectrum
./printTree
