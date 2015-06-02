#g++ -c `root-config --cflags --libs` -std=c++0x timeResponseCalibration.cpp
#g++ -o timeResponseCalibration timeResponseCalibration.o `root-config --cflags --libs`
#./timeResponseCalibration

#root -l TimeRespCal_RMS.root

g++ -c `root-config --cflags --libs` -std=c++0x timeResponseCalibrationSync.cpp
g++ -o timeResponseCalibrationSync timeResponseCalibrationSync.o `root-config --cflags --libs`
./timeResponseCalibrationSync

#root -l TimeRespCal_RMS.root
