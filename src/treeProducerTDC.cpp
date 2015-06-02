//
//  treeProducerTDC.cc
//  
//
//  Created by Brian L Dorney on 15/01/15.
//
//

#include "treeProducerTDC.h"

//ClassImp(treeProducerTDC);

//Constructor
treeProducerTDC::treeProducerTDC(){
    verbose_IO          = false;
    verbose_LUT         = false;
    verbose_PFN         = false;
    verbose_PkCalc      = false;
    verbose_PrintRuns   = false;
    //verbose_MappedData  = false;
    
    iRebinFactor    = 0;
    
    fitOption       = "";
    
    secName_AUTO    = "[AUTO]";
    secName_DET     = "[DET]";
    secName_END     = "[END]";
    secName_MAN     = "[MAN]";
    
    //Set Default Numeric Deconvolution Model
    func_NumericDeconvoModel = new TF1("func_NumericDeconvoModel","sqrt( ( ( x-[0] )/ [1] )^2 - [2] )",0.,50.);
    func_NumericDeconvoModel->SetParameter(0,-2.32171083637621560e-02);
    func_NumericDeconvoModel->SetParameter(1,1.00091696932777618e+00);
    func_NumericDeconvoModel->SetParameter(2,5.23366302191208774e+01);
} //End treeProducerTDC constructor

//Sets the numeric deconvolution model
void treeProducerTDC::setNumericDeconvoModel(string inputFormula, vector<float> inputParameters){
    func_NumericDeconvoModel = new TF1(func_NumericDeconvoModel->GetName(), inputFormula.c_str(),0.,50.);
    
    for (int i=0; i < inputParameters.size(); ++i) {
        func_NumericDeconvoModel->SetParameter(i, inputParameters[i]);
    }
    
    return;
} //End treeProducerTDC::setNumericDeconvoModel()

//Set a Parameter to Be Ignored when calling getParsedFileName()
void treeProducerTDC::setIgnoredParameter(string inputParameter){
    std::transform(inputParameter.begin(),inputParameter.end(),inputParameter.begin(), toupper);
    
    vecIgnoredParam.push_back(inputParameter);
    
    return;
} //End treeProducerTDC::setIgnoredParameter()

//Write the tree
void treeProducerTDC::writeTree(string inputTreeName, string outputDataFile){
    //Variable Declaration
    Run run;
    
    string line;
    
    //Create Output ROOT File
    //Make TFile first, prevents "memory-resident Tree" problems??
    TFile *outputTFile = new TFile(outputDataFile.c_str(),"RECREATE",(inputTreeName + "Tree File").c_str() );
    
    //Create the TTree
    TTree *outputTDCTree = new TTree(inputTreeName.c_str(), "Tree of TDC Data");
    
    //Make a Dummy Histogram?
    TH1F *dummyHisto = new TH1F("dummyHisto","",10,0,10);
    run.hTDC_Histo = dummyHisto;
    
    //Make a Dummy TF1?
    TF1 *dummyFunc = new TF1("dummyFunc","[0]*x",0,100);
    run.func_Gaus = dummyFunc;
    run.func_Convo = dummyFunc;
    
    //Make Branches
    //outputTDCTree->Branch("branchname",&ref,"name/Type ");
    //Run Info
    outputTDCTree->Branch("iRun",&run.iRun,"iRun/I");
    outputTDCTree->Branch("iBeam",&run.iBeam_Type,"iBeam/I");
    outputTDCTree->Branch("iTrig_Mode",&run.iTrig_Mode,"iTrig_Mode/I");
    outputTDCTree->Branch("fTrig_Delay",&run.fTrig_Delay,"fTrig_Delay/F");
    
    //Det Info
    outputTDCTree->Branch("iDet_Eta",&run.iDet_Eta,"iDet_Eta/I");
    outputTDCTree->Branch("iDet_Phi",&run.iDet_Phi,"iDet_Phi/I");
    
    outputTDCTree->Branch("fDet_Imon",&run.fDet_Imon,"fDet_Imon/F");
    outputTDCTree->Branch("fDet_VDrift",&run.fDet_VDrift,"fDet_VDrift/F");
    
    outputTDCTree->Branch("fDet_Gain",&run.fDet_Gain,"fDet_Gain/F");
    outputTDCTree->Branch("fDet_Gain_Err",&run.fDet_Gain_Err,"fDet_Gain_Err/F");
    
    outputTDCTree->Branch("fDet_GasFrac_Ar",&run.fDet_GasFrac_Ar,"fDet_GasFrac_Ar/F");
    outputTDCTree->Branch("fDet_GasFrac_CO2",&run.fDet_GasFrac_CO2,"fDet_GasFrac_CO2/F");
    outputTDCTree->Branch("fDet_GasFrac_CF4",&run.fDet_GasFrac_CF4,"fDet_GasFrac_CF4/F");
    
    //VFAT Info
    outputTDCTree->Branch("iTURBO_ID",&run.iTURBO_ID,"iTURBO_ID/I");
    outputTDCTree->Branch("iVFAT_Pos",&run.iVFAT_Pos,"iVFAT_Pos/I");
    
    outputTDCTree->Branch("strVFAT_ID",&run.strVFAT_ID,"strVFAT_Pos/C");
    
    outputTDCTree->Branch("iVFAT_IPreAmpIn",&run.iVFAT_IPreAmpIn,"iVFAT_IPreAmpIn/I");
    outputTDCTree->Branch("iVFAT_IPreAmpFeed",&run.iVFAT_IPreAmpFeed,"iVFAT_IPreAmpFeed/I");
    outputTDCTree->Branch("iVFAT_IPreAmpOut",&run.iVFAT_IPreAmpOut,"iVFAT_IPreAmpOut/I");
    
    outputTDCTree->Branch("iVFAT_IShaper",&run.iVFAT_IShaper,"iVFAT_IShaper/I");
    outputTDCTree->Branch("iVFAT_IShaperFeed",&run.iVFAT_IShaperFeed,"iVFAT_IShaperFeed/I");
    
    outputTDCTree->Branch("iVFAT_IComp",&run.iVFAT_IComp,"iVFAT_IComp/I");
    
    outputTDCTree->Branch("iVFAT_MSPL",&run.iVFAT_MSPL,"iVFAT_MSPL/I");
    
    outputTDCTree->Branch("iVFAT_Latency",&run.iVFAT_Latency,"iVFAT_Latency/I");
    
    outputTDCTree->Branch("fVFAT_Thresh",&run.fVFAT_Thresh,"fVFAT_Thresh/F");
    
    //TDC Info
    outputTDCTree->Branch("iTDC_CH_Number",&run.iTDC_CH_Number,"iTDC_CH_Number/I");
    
    outputTDCTree->Branch("fTDC_Histo_Mean",&run.fTDC_Histo_Mean,"fTDC_Histo_Mean/F");
    outputTDCTree->Branch("fTDC_Histo_RMS",&run.fTDC_Histo_RMS,"fTDC_Histo_RMS/F");
    outputTDCTree->Branch("fTDC_Histo_nPks",&run.fTDC_Histo_nPks,"fTDC_Histo_nPks/F");
    
    outputTDCTree->Branch("fTDC_Histo_PkInt_1stMax", &run.fTDC_Histo_PkInt_1stMax,"fTDC_Histo_PkInt_1stMax/F");
    outputTDCTree->Branch("fTDC_Histo_PkInt_2ndMax", &run.fTDC_Histo_PkInt_2ndMax,"fTDC_Histo_PkInt_2ndMax/F");
    outputTDCTree->Branch("fTDC_Histo_PkInt_3rdMax", &run.fTDC_Histo_PkInt_3rdMax,"fTDC_Histo_PkInt_3rdMax/F");
    
    outputTDCTree->Branch("fTDC_Histo_PkPos_1stMax", &run.fTDC_Histo_PkPos_1stMax,"fTDC_Histo_PkPos_1stMax/F");
    outputTDCTree->Branch("fTDC_Histo_PkPos_2ndMax", &run.fTDC_Histo_PkPos_2ndMax,"fTDC_Histo_PkPos_2ndMax/F");
    outputTDCTree->Branch("fTDC_Histo_PkPos_3rdMax", &run.fTDC_Histo_PkPos_3rdMax,"fTDC_Histo_PkPos_3rdMax/F");
    
    outputTDCTree->Branch("fTDC_Fit_Amp",&run.fTDC_Fit_Amp,"fTDC_Fit_Amp/F");
    outputTDCTree->Branch("fTDC_Fit_Amp_Err",&run.fTDC_Fit_Amp_Err,"fTDC_Fit_Amp_Err/F");
    outputTDCTree->Branch("fTDC_Fit_Mean",&run.fTDC_Fit_Mean,"fTDC_Fit_Mean/F");
    outputTDCTree->Branch("fTDC_Fit_Mean_Err",&run.fTDC_Fit_Mean_Err,"fTDC_Fit_Mean_Err/F");
    outputTDCTree->Branch("fTDC_Fit_Sigma",&run.fTDC_Fit_Sigma,"fTDC_Fit_Sigma/F");
    outputTDCTree->Branch("fTDC_Fit_Sigma_Err",&run.fTDC_Fit_Sigma_Err,"fTDC_Fit_Sigma_Err/F");
    outputTDCTree->Branch("fTDC_Fit_Chi2",&run.fTDC_Fit_Chi2,"fTDC_Fit_Chi2/F");
    outputTDCTree->Branch("fTDC_Fit_NDF",&run.fTDC_Fit_NDF,"fTDC_Fit_NDF/F");
    
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Amp", &run.fTDC_Fit_Convo_Amp, "fTDC_Fit_ContConvo_Amp/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Amp_Err", &run.fTDC_Fit_Convo_Amp_Err, "fTDC_Fit_ContConvo_Amp_Err/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Mean", &run.fTDC_Fit_Convo_Mean, "fTDC_Fit_ContConvo_Mean/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Mean_Err", &run.fTDC_Fit_Convo_Mean_Err, "fTDC_Fit_ContConvo_Mean_Err/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Sigma", &run.fTDC_Fit_Convo_Sigma, "fTDC_Fit_ContConvo_Sigma/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Sigma_Err", &run.fTDC_Fit_Convo_Sigma_Err, "fTDC_Fit_ContConvo_Sigma_Err/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_Chi2", &run.fTDC_Fit_Convo_Chi2, "fTDC_Fit_ContConvo_Chi2/F");
    outputTDCTree->Branch("fTDC_Fit_ContConvo_NDF", &run.fTDC_Fit_Convo_NDF, "fTDC_Fit_ContConvo_NDF/F");
    
    outputTDCTree->Branch("fTDC_NumDeconvo_TimeResp", &run.fTDC_NumDeconvo_TimeResp, "fTDC_NumDeconvo_TimeResp/F");
    
    outputTDCTree->Branch("hTDC_Histo","TH1F", &run.hTDC_Histo,32000,0);
    
    outputTDCTree->Branch("func_TDC_Gaus","TF1", &run.func_Gaus,32000,0 );
    outputTDCTree->Branch("func_TDC_Convo","TF1", &run.func_Convo,32000,0 );
    
    //Open the Data File
    //------------------------------------------------------
    if (verbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeProducerTDC::writeTree(): trying to open and read: " << fileName_Data << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream dataInput( fileName_Data.c_str() );
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!dataInput.is_open() && verbose_IO) {
        perror( ("treeProducerTDC::writeTree(): error while opening file: " + fileName_Data).c_str() );
        printStreamStatus(dataInput);
    }
    
    //Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getline(dataInput, line) ) {
        //Set File Names for later operations, use a string stream
        std::stringstream strStream;
        
        strStream<<line;
        strStream>>fileName_ROOT>>fileName_LUT;
        
        //Does the user want to comment out this run?
        if (fileName_ROOT.compare(0,1,"#") == 0) continue;
        
        //Output action to user, but only if line is not commented out!!!
        //cout<<"treeProducerTDC::writeTree() - Retreiving Line!\n";
        cout<<"treeProducerTDC::writeTree() - Retreiving Line: " << fileName_ROOT << endl;
        
        //Create an instance of the Run Parameters and then get specific run info
        //Param runParameters;
        TRunParameters runLog;
        //run = runLog.getRun();
        
        //setRun(fileName_ROOT, fileName_LUT, runParameters, runLog);
        setRun(fileName_ROOT, fileName_LUT, runLog);
        
        //Run run = runLog.getRun();
        run = runLog.getRun();
        
        outputTDCTree->Fill();
    } //End Loop Over Input File
    
    //Check to see if we had problems while reading the file
    if (dataInput.bad() && verbose_IO) {
        perror( ("treeProducerTDC::writeTree(): error while reading file: " + fileName_Data).c_str() );
        printStreamStatus(dataInput);
    }
    
    //Write the Tree
    outputTFile->cd();
    //outputTDCTree->Print();
    outputTDCTree->Write();
    
    //Print to the User all the information stored in outputTDCTree
    if (verbose_PrintRuns) {
        printStoredData(outputTDCTree);
    }
    
    //Close the File
    //------------------------------------------------------
    dataInput.close();
    
    //Close the Created ROOT File
    //------------------------------------------------------
    outputTFile->Close();
    
    return;
} //End treeProducerTDC::writeTree()

void treeProducerTDC::printStoredData(TTree *inputTree){
    //Variable Declaration
    Run run;
    
    //Get Branches
    //Run Info
    inputTree->SetBranchAddress("iRun",&run.iRun);
    inputTree->SetBranchAddress("iBeam",&run.iBeam_Type);
    inputTree->SetBranchAddress("iTrig_Mode",&run.iTrig_Mode);
    inputTree->SetBranchAddress("fTrig_Delay",&run.fTrig_Delay);
    
    //Det Info
    inputTree->SetBranchAddress("iDet_Eta",&run.iDet_Eta);
    inputTree->SetBranchAddress("iDet_Phi",&run.iDet_Phi);
    
    inputTree->SetBranchAddress("fDet_Imon",&run.fDet_Imon);
    inputTree->SetBranchAddress("fDet_VDrift",&run.fDet_VDrift);
    
    inputTree->SetBranchAddress("fDet_Gain",&run.fDet_Gain);
    inputTree->SetBranchAddress("fDet_Gain_Err",&run.fDet_Gain_Err);
    
    inputTree->SetBranchAddress("fDet_GasFrac_Ar",&run.fDet_GasFrac_Ar);
    inputTree->SetBranchAddress("fDet_GasFrac_CO2",&run.fDet_GasFrac_CO2);
    inputTree->SetBranchAddress("fDet_GasFrac_CF4",&run.fDet_GasFrac_CF4);
    
    //VFAT Info
    inputTree->SetBranchAddress("iTURBO_ID",&run.iTURBO_ID);
    inputTree->SetBranchAddress("iVFAT_Pos",&run.iVFAT_Pos);
    
    //inputTree->SetBranchAddress("strVFAT_ID",&run.strVFAT_ID);
    
    inputTree->SetBranchAddress("iVFAT_IPreAmpIn",&run.iVFAT_IPreAmpIn);
    inputTree->SetBranchAddress("iVFAT_IPreAmpFeed",&run.iVFAT_IPreAmpFeed);
    inputTree->SetBranchAddress("iVFAT_IPreAmpOut",&run.iVFAT_IPreAmpOut);
    
    inputTree->SetBranchAddress("iVFAT_IShaper",&run.iVFAT_IShaper);
    inputTree->SetBranchAddress("iVFAT_IShaperFeed",&run.iVFAT_IShaperFeed);
    
    inputTree->SetBranchAddress("iVFAT_IComp",&run.iVFAT_IComp);
    
    inputTree->SetBranchAddress("iVFAT_MSPL",&run.iVFAT_MSPL);
    
    inputTree->SetBranchAddress("iVFAT_Latency",&run.iVFAT_Latency);
    
    inputTree->SetBranchAddress("fVFAT_Thresh",&run.fVFAT_Thresh);
    
    //TDC Info
    inputTree->SetBranchAddress("iTDC_CH_Number",&run.iTDC_CH_Number);
    
    inputTree->SetBranchAddress("fTDC_Histo_Mean",&run.fTDC_Histo_Mean);
    inputTree->SetBranchAddress("fTDC_Histo_RMS",&run.fTDC_Histo_RMS);
    inputTree->SetBranchAddress("fTDC_Histo_nPks",&run.fTDC_Histo_nPks);
    
    inputTree->SetBranchAddress("fTDC_Histo_PkInt_1stMax", &run.fTDC_Histo_PkInt_1stMax);
    inputTree->SetBranchAddress("fTDC_Histo_PkInt_2ndMax", &run.fTDC_Histo_PkInt_2ndMax);
    inputTree->SetBranchAddress("fTDC_Histo_PkInt_3rdMax", &run.fTDC_Histo_PkInt_3rdMax);
    
    inputTree->SetBranchAddress("fTDC_Histo_PkPos_1stMax", &run.fTDC_Histo_PkPos_1stMax);
    inputTree->SetBranchAddress("fTDC_Histo_PkPos_2ndMax", &run.fTDC_Histo_PkPos_2ndMax);
    inputTree->SetBranchAddress("fTDC_Histo_PkPos_3rdMax", &run.fTDC_Histo_PkPos_3rdMax);
    
    inputTree->SetBranchAddress("fTDC_Fit_Amp",&run.fTDC_Fit_Amp);
    inputTree->SetBranchAddress("fTDC_Fit_Mean",&run.fTDC_Fit_Mean);
    inputTree->SetBranchAddress("fTDC_Fit_Sigma",&run.fTDC_Fit_Sigma);
    inputTree->SetBranchAddress("fTDC_Fit_Chi2",&run.fTDC_Fit_Chi2);
    inputTree->SetBranchAddress("fTDC_Fit_NDF",&run.fTDC_Fit_NDF);
    
    inputTree->SetBranchAddress("fTDC_Fit_ContConvo_Amp", &run.fTDC_Fit_Convo_Amp);
    inputTree->SetBranchAddress("fTDC_Fit_ContConvo_Mean", &run.fTDC_Fit_Convo_Mean);
    inputTree->SetBranchAddress("fTDC_Fit_ContConvo_Sigma", &run.fTDC_Fit_Convo_Sigma);
    inputTree->SetBranchAddress("fTDC_Fit_ContConvo_Chi2", &run.fTDC_Fit_Convo_Chi2);
    inputTree->SetBranchAddress("fTDC_Fit_ContConvo_NDF", &run.fTDC_Fit_Convo_NDF);
    
    inputTree->SetBranchAddress("fTDC_NumDeconvo_TimeResp",&run.fTDC_NumDeconvo_TimeResp);
    
    cout<<"Run Beam Mode DLY | iEta iPhi Imon VDrift Gain f_Ar f_CO f_CF4 | IPreAmpI IPreAmpF IPreAmpO IShaper IShaperFeed IComp MSPL Lat Thresh | Sigma_Histo Sigma_Gaus Sigma_Cont Sigma_Num | Pk1_Pos Pk1_Int Pk2_Pos Pk2_Int Pk3_Pos Pk3_Int\n";
    
    for (int i=0; i < inputTree->GetEntries(); ++i) { //Loop Over Tree Entries
        inputTree->GetEntry(i);
        
        printf("%i %i %i %4.2f | ",run.iRun, run.iBeam_Type, run.iTrig_Mode, run.fTrig_Delay);
        printf(" %i %i %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f | ",run.iDet_Eta,run.iDet_Phi,run.fDet_Imon,run.fDet_VDrift,run.fDet_Gain,run.fDet_GasFrac_Ar,run.fDet_GasFrac_CO2,run.fDet_GasFrac_CF4);
        printf("%i %i %i %i %i %i %i %i %4.2f | ",run.iVFAT_IPreAmpIn,run.iVFAT_IPreAmpFeed,run.iVFAT_IPreAmpOut,run.iVFAT_IShaper,run.iVFAT_IShaperFeed,run.iVFAT_IComp,run.iVFAT_MSPL,run.iVFAT_Latency,run.fVFAT_Thresh);
        printf(" %4.2f %4.2f %4.2f %4.2f | ",run.fTDC_Histo_RMS,run.fTDC_Fit_Sigma,run.fTDC_Fit_Convo_Sigma,run.fTDC_NumDeconvo_TimeResp);
        printf(" %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n", run.fTDC_Histo_PkPos_1stMax, run.fTDC_Histo_PkInt_1stMax, run.fTDC_Histo_PkPos_2ndMax, run.fTDC_Histo_PkInt_2ndMax, run.fTDC_Histo_PkPos_3rdMax, run.fTDC_Histo_PkInt_3rdMax);
    } //End Loop Over Tree Entries
    
    return;
} //End treeProducerTDC::printStoredData()

//Returns the timing histogram:
/*TH1F* treeProducerTDC::getHistogram(string inputFileName, int chanNum){
    //Variable Declaration
    //TH1F *ret_Histo;
    
    //Open the Data File
    //------------------------------------------------------
    if (verbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeProducerTDC::getHistogram(): trying to open and read: " << inputFileName << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    TFile *file_ROOT = new TFile(inputFileName.c_str(), "READ", "", 1);
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if ( !file_ROOT->IsOpen() || file_ROOT->IsZombie() ) { //Case: Problem Opening File
        if (verbose_IO) {
            perror( ("treeProducerTDC::getHistogram(): error while opening file: " + inputFileName).c_str() );
            std::cout << "Input ROOT File Status:\n";
            std::cout << " IsOpen()=" << file_ROOT->IsOpen() << endl;
            std::cout << " IsZombie()=" << file_ROOT->IsZombie() << endl;
        }
        
        //Close the file
        file_ROOT->Close();
        
        //Return a null pointer
        return nullptr;
    } //End Case: Problem Opening File
    else{
        return ( (TH1F*) file_ROOT->Get( ( "TDC_Ch" + getString(chanNum) ).c_str() ) );
    }
} //End treeProducerTDC::getHistogram()*/

//Loads the lookup table for a given run from a structured filename and also sets manually entered parameters for the given run.
//The returned vector<quad> is the lookup table
//The quad is defined as:
//  first -> Key Value; second -> Identifier; third -> DataType; fourth -> mthd idx
vector<quad<std::string, vector<std::string>, treeProducerTDC::dataType_t, int> > treeProducerTDC::getLookUpTable(string inputFileName/*, Param &mappedParam*/, TRunParameters &runLogger ){
    //Variable Declaration
    bool bSec_Auto, bSec_Det, bSec_Man, bExitFlag = false;
    
    int iMthdIdx;
    
    string line;
    string strTreeName, strDataType;
    
    vector<std::string> vec_strLUTIdents;
    vector<quad<std::string,vector<std::string>, dataType_t, int> > ret_vecOfParamQuad;
    
    //Open the Data File
    //------------------------------------------------------
    if (verbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeProducerTDC::getLookUpTable(): trying to open and read: " << inputFileName << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    //ifstream lutFile;
    ifstream lutFile(inputFileName.c_str() );
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!lutFile.is_open() && verbose_IO) {
        perror( ("treeProducerTDC::getLookUpTable(): error while opening file: " + inputFileName).c_str() );
        printStreamStatus(lutFile);
    }
    
    //Initialize boolean variables
    bSec_Auto=bSec_Det=bSec_Man=false;
    
    //Loop Over the Input LUT File
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getline(lutFile, line) ) {
        //Clear vec_strLUTIdents at the start of each new input
        vec_strLUTIdents.clear();
        
        //Skip Commented lines
        if (line.compare(0,1,"#") == 0 ) continue;
        
        //Check What Section Header We are Under
        if (line.compare(secName_DET) == 0 ) { //Case: Detector Entry
            bSec_Det=true; bSec_Auto=bSec_Man=false;
            continue;   //Should do nothing else for this line
        } //End Case: Detector Entry
        else if(line.compare(secName_AUTO) == 0 ) { //Case: Automatic Search
            bSec_Auto=true; bSec_Det=bSec_Man=false;
            continue;   //Should do nothing else for this line
        } //End Case: Automatic Search
        else if(line.compare(secName_MAN) == 0 ) { //Case: Manual Entry
            bSec_Man=true; bSec_Auto=bSec_Det=false;
            continue;   //Should do nothing else for this line
        } //End Case: Manual Entry
        else if(line.compare(secName_END) == 0 ){ //Case: End of File
            break;
        } //End Case: End of File
        
        //Parse the line in the look up table
        //parsedLine = getParsedLUTLine(line);
        setParsedLUTLine(line,vec_strLUTIdents,strTreeName,strDataType,iMthdIdx,bExitFlag);
        
        //Cross check to make sure the line parsed correctly (size should be 4)
        if ( !bExitFlag || vec_strLUTIdents.size() == 0 ){ //Check on parsed LUT line
            cout<<"line:\n";
            cout<<line<<endl;
            cout<<"Did not parse correctly, please investigate, skipping!\n";
            
            continue;
        } //End Check on parsedLine.size()
        
        //Either store the manually entered parameters in the parameter map or prep the look up table for those parameters which will be automatically found
        if (bSec_Det || bSec_Man) { //Case: Manual Entry
            //Here vec_strLUTIdents.size() = 1; as the user can only make one entry per field in DET or MAN sections
            
            //Check Datatype of Input Parameter and Store accordingly
            if (strDataType.compare("BOOL") == 0 ) { //Case: Data Type - bool
                runLogger.setParameter( (vec_strLUTIdents[0].compare("TRUE") == 0 ), iMthdIdx );
            } //End Case: Data Type - bool
            else if (strDataType.compare("FLOAT") == 0 ) { //Case: Data Type - float
                runLogger.setParameter( std::stof(vec_strLUTIdents[0]), iMthdIdx );
            } //End Case: Data Type - float
            else if (strDataType.compare("INT") == 0 ) {
                //mappedParam.mapDataInt[parsedLine[0]] = std::make_pair( std::stoi(parsedLine[2]), std::stoi(parsedLine[3]) );
                runLogger.setParameter( std::stoi(vec_strLUTIdents[0]), iMthdIdx );
            } //End Case: Data Type - int
            else if (strDataType.compare("STRING") == 0 ) { //Case: Data Type - string
                //mappedParam.mapDataString[parsedLine[0]] = std::make_pair( parsedLine[2], std::stoi(parsedLine[3]) );
                runLogger.setParameter( vec_strLUTIdents[0], iMthdIdx );
            } //End Case: Data Type - string
        } //End Case: Manual Entry
        else if ( bSec_Auto ) { //Case: Automatic Search
            //Check Datatype of Input Parameter and Create quad accordingly
            if (strDataType.compare("BOOL") == 0 ) { //Case: Data Type - bool
                ret_vecOfParamQuad.push_back(make_quad(strTreeName, vec_strLUTIdents, typeBool, iMthdIdx ) );
            } //End Case: Data Type - bool
            else if (strDataType.compare("FLOAT") == 0 ) { //Case: Data Type - float
                ret_vecOfParamQuad.push_back(make_quad(strTreeName, vec_strLUTIdents, typeFloat, iMthdIdx ) );
            } //End Case: Data Type - float
            else if (strDataType.compare("INT") == 0 ) {
                ret_vecOfParamQuad.push_back(make_quad(strTreeName, vec_strLUTIdents, typeInt, iMthdIdx ) );
            } //End Case: Data Type - int
            else if (strDataType.compare("STRING") == 0 ) { //Case: Data Type - string
                ret_vecOfParamQuad.push_back(make_quad(strTreeName, vec_strLUTIdents, typeString, iMthdIdx ) );
            } //End Case: Data Type - string
        } //End Case: Automatic Search
    //} while ( !lutFile.eof() );
    } //End Loop Over Input File
    if ( lutFile.bad() && verbose_IO) {
        perror( ("treeProducerTDC::getLookUpTable(): error while reading file: " + inputFileName).c_str() );
        printStreamStatus(lutFile);
    }
    
    //Check to see if minimum parameter input is successful (i.e. warn user if they fucked up)
    //DET_POS Present?
    if (runLogger.getDetPos() < 0 ) {
        cout<<"ERROR!!! A DET_POS VALUE WAS NOT FOUND IN "<< inputFileName <<endl;
        cout<<"PLEASE SPECIFY A DET_POS VALUE NOW OR EXIT (Ctrl+C)"<<endl;
        
        //cin>>mappedParam.mapDataInt[keyVal_DetPos];
        
        int userInput;
        cin>>userInput;
        
        //mappedParam.mapDataInt[keyVal_DetPos] = std::make_pair(userInput,101);
        
        runLogger.setDetPos(userInput);
    }
    
    //TDC_CH_NUMBER Present?
    if (runLogger.getTDCChanNumber() < 0 ) {
        cout<<"ERROR!!! A TDC_CH_NUMBER VALUE WAS NOT FOUND IN " << inputFileName <<endl;
        cout<<"PLEASE SPECIFY A TDC_CH_NUMBER VALUE NOW OR EXIT (Ctrl+C)"<<endl;
        
        //cin>>mappedParam.mapDataInt[keyVal_TDCCh];
        
        int userInput;
        cin>>userInput;
        
        //mappedParam.mapDataInt[keyVal_TDCCh] = std::make_pair(userInput,301);
        
        runLogger.setTDCChanNumber(userInput);
    }
    
    //Close the File
    lutFile.close();
    
    return ret_vecOfParamQuad;
} //End treeProducerTDC::getLookUpTable()

//Given a Histogram and a peak position (in X-axis units); determine the peak integral
float treeProducerTDC::getPeakIntegral(TH1F *hInputHisto, float fInputPeakPos_X){
    //Variable Declaration
    bool bReset=false;          //Tracks Iteration Count
    
    bool bPeakLimit_Lwr = false;//Reached 1st Bin of Histogram?
    bool bPeakLimit_Upr = false;//Reached Last Bin of Histogram?
    
    int iCounter=0;             //Tracks Iteration Count
    
    int iPeakPos_X;             //Starting Bin Position of fInputPeakPos_X
    int iPeakPos_X_LwrBnd = 1;  //Lower Bound of Peak Bins
    int iPeakPos_X_UprBnd = 1;  //Upper Bound of Peak Bins
    
    int iBinNum_X = hInputHisto->GetNbinsX();   //Number of Bins in hInputHisto
    
    float fBinWidth_X = hInputHisto->GetBinLowEdge(iBinNum_X) - hInputHisto->GetBinLowEdge(iBinNum_X-1);
    
    float ret_PeakIntegral;
    
    //Transform fInputPeakPos_X to iPeakPos_X
    iPeakPos_X = std::floor(fInputPeakPos_X) / fBinWidth_X; //This should be an integer;
    
    //Check to make sure fInputPeakPos_X is within Bin Range
    if (hInputHisto->GetBinLowEdge(iPeakPos_X) <= fInputPeakPos_X && fInputPeakPos_X < hInputHisto->GetBinLowEdge(iPeakPos_X+1) ) { //Case: iPeakPos_X is the starting Position
        /* No Change, iPeakPos_X already set */
    } //End Case: iPeakPos_X is the starting Position
    else if(hInputHisto->GetBinLowEdge(iPeakPos_X-1) <= fInputPeakPos_X && fInputPeakPos_X < hInputHisto->GetBinLowEdge(iPeakPos_X) ) { //Case: (iPeakPos_X-1) is the starting Position
        //Reset iPeakPos_X
        --iPeakPos_X;
    } //End Case: iPeakPos_X is the starting Position
    if (hInputHisto->GetBinLowEdge(iPeakPos_X+1) <= fInputPeakPos_X && fInputPeakPos_X < hInputHisto->GetBinLowEdge(iPeakPos_X+2) ) { //Case: iPeakPos_X is the starting Position
        ++iPeakPos_X;
    } //End Case: iPeakPos_X is the starting Position
    else{ //Case: Unrecognized?
        cout<<"treeProducerTDC::getPeakIntegral() - Unrecognized Case!!!\n";
        cout<<"treeProducerTDC::getPeakIntegral() - hInputHisto->GetBinLowEdge(iPeakPos_X-1) = " << hInputHisto->GetBinLowEdge(iPeakPos_X-1) << endl;
        cout<<"treeProducerTDC::getPeakIntegral() - hInputHisto->GetBinLowEdge(iPeakPos_X) = " << hInputHisto->GetBinLowEdge(iPeakPos_X) << endl;
        cout<<"treeProducerTDC::getPeakIntegral() - hInputHisto->GetBinLowEdge(iPeakPos_X+1) = " << hInputHisto->GetBinLowEdge(iPeakPos_X+1) << endl;
        
        cout<<"treeProducerTDC::getPeakIntegral() -  fInputPeakPos_X = " << fInputPeakPos_X << endl;
        
        cout<<"treeProducerTDC::getPeakIntegral() -  fInputPeakPos_X OUT OF RANGE?!\n";
    } //End Case: Unrecognzied?
    
    //Set Starting Peak Info
    ret_PeakIntegral = hInputHisto->GetBinContent(iPeakPos_X);
    
    //Telescope Through Peak
    while ( !( hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) == 0
              && hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd) == 0 )
           ) { //Loop Through Peak
        //NOTE: To Include Correct Upper Bound Go 2 bins more since the lower bound of the 2nd bin is the starting upper bound of the peak!!!
        //NOTE: Hence the +1 below in the "iPeakPos_X + iPeakPos_X_UprBnd + 1" expression
        
        if (bReset && verbose_PkCalc) {
            cout<<"============================================" << endl;
            cout<<"iCounter = " << iCounter << endl;
            
            cout<<"iPeakPos_X - iPeakPos_X_LwrBnd = " << iPeakPos_X - iPeakPos_X_LwrBnd << endl;
            cout<<"iPeakPos_X = " << iPeakPos_X << endl;
            cout<<"iPeakPos_X + iPeakPos_X_UprBnd = " << iPeakPos_X + iPeakPos_X_UprBnd << endl;
            
            cout<<"hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) = " << hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) << endl;
            cout<<"ret_PeakIntegral = " << ret_PeakIntegral << endl;
            cout<<"hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd) = " << hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd) << endl;
            
            (bPeakLimit_Lwr) ? cout<<"bPeakLimit_Lwr = true"<<endl : cout<<"bPeakLimit_Lwr = false"<<endl;
            (bPeakLimit_Upr) ? cout<<"bPeakLimit_Upr = true"<<endl : cout<<"bPeakLimit_Upr = false"<<endl;
            cout<<"============================================" << endl;
            
            int dummy;
            cout<<"Pausing For User Prompt (Enter an Integer or Press Ctrl+C to Exit)"<<endl;
            cin>>dummy;
        }
        
        //Increase Integral, Three Cases
        if (!bPeakLimit_Lwr && !bPeakLimit_Upr) { //Case: Neither Histogram Limit Reached
            //ret_PeakIntegral += hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) + hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd +1);   //INCORRECT!!! GIVES WRONG PEAK INTEGRAL
            ret_PeakIntegral += hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) + hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd);     //Confirmed Working
        } //End Case: Neither Histogram Limit Reached
        else if (bPeakLimit_Lwr && !bPeakLimit_Upr ) { //Case: Lower Limit Reached, Add Only in Upper Direction
            //ret_PeakIntegral += hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd +1);   //INCORRECT!!! GIVES WRONG PEAK INTEGRAL
            ret_PeakIntegral += hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd);     //Confirmed Working
        } //End Case: Lower Limit Reached, Add Only in Upper Direction
        else if(!bPeakLimit_Lwr && bPeakLimit_Upr) { //Case: Upper Limit Reached, Add Only in Lower Direction
            ret_PeakIntegral += hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd);
        } //End Case: Upper Limit Reached, Add Only in Lower Direction
        
        //Widen Lower Acceptance Zone
        if ( (iPeakPos_X - iPeakPos_X_LwrBnd - 1) > 1 ) { //Case: Lower Limit Not Reached
            iPeakPos_X_LwrBnd++; //Due to Sign convention above we use ++ operator here
        } //End Case: Lower Limit Not Reached
        else { //Case: Lower Limit REACHED!!!
            bPeakLimit_Lwr = true;
        } //End Case: Lower Limit REACHED!!!
        
        //Widen Upper Acceptance Zone
        if ( ( iPeakPos_X + iPeakPos_X_UprBnd + 1 ) < hInputHisto->GetNbinsX() ) { //Case: Upper Limit Not Reaced
            iPeakPos_X_UprBnd++;
        } //End Case: Upper Limit Not Reaced
        else { //Case: Upper Limit Reaced!!!
            bPeakLimit_Upr = true;
        } //End Case: Upper Limit Reaced!!!
        
        //Check Rare Tail Exit Condition
        if ( bPeakLimit_Lwr && hInputHisto->GetBinContent(iPeakPos_X + iPeakPos_X_UprBnd) == 0 ) {break;}
        if ( bPeakLimit_Upr && hInputHisto->GetBinContent(iPeakPos_X - iPeakPos_X_LwrBnd) == 0 ) {break;}
        
        iCounter++;
        
        if (iCounter > 10 && !bReset && verbose_PkCalc) {
            cout<<"More than 10 iterations have been processed; resetting to 1st iteration and displaying output step wise"<<endl;
            
            ret_PeakIntegral = hInputHisto->GetBinContent(iPeakPos_X);
            
            iPeakPos_X_LwrBnd = 1;  //Lower Bound of Peak Bins
            iPeakPos_X_UprBnd = 1;  //Upper Bound of Peak Bins
            
            bPeakLimit_Lwr = false;
            bPeakLimit_Upr = false;
            
            bReset = true;
            
            iCounter = 0;
        }
    } //End Loop Through Peak
    
    return ret_PeakIntegral;
} //End treeProducerTDC::getPeakIntegral()

//For an underscore delimited string input returns a vector of strings whose elements are the strings in between the underscores
list<string> treeProducerTDC::getParsedFileName(string input){
    //Variable Declaration
    bool exitFlag       = false;
    
    int posLast         = 0;
    int posUnderscore1  = 0;
    
    string param        = "";
    string underscore   = "_";
    
    list<string> ret_strings;
    
    //Go through the input file name and remove any file path (if present)
    while ( input.find("/") != std::string::npos ) { //Loop Over Input String - Remove FilePath
        input = input.substr(input.find("/")+1, input.length() - input.find("/") );
    } //End Loop Over Input String - Remove FilePath
    
    //Go through the input file name
    //Treat the underscores as deliminators; find the relevant parameters of the run
    //Store them in ret_strings, return to user
    do {
        bool bIgnoreParam = false;
        
        //Debugging
        //cout<<"treeProducerTDC::getParsedFileName - No Fault 1_a" << endl;
        //cout<<"============================================" << endl;
        //Find Underscores
        posUnderscore1 = input.find(underscore, 0);
        
        //Set the Loop to Exit if an underscore is not found after position of the last underscore
        //Then set posUnderscore1 = input.length() so it takes the last part of the input string into consideration
        if(posUnderscore1 == std::string::npos){ //Case: Exit Flag = true
            exitFlag=true;
            posUnderscore1 = input.length();
        } //End Case: Exit Flag = true
        
        //Store this parameter
        param = input.substr(posLast,posUnderscore1);
        if(!exitFlag) input = input.substr(posUnderscore1+1,input.length()-posUnderscore1);
        
        //transform to upper case
        std::transform(param.begin(),param.end(),param.begin(), toupper);
        
        //Check to make sure we do not ignore this parameter
        for (auto iter = vecIgnoredParam.begin(); iter != vecIgnoredParam.end(); ++iter) {
            if (param.compare( *iter ) == 0 ){
                bIgnoreParam = true;
                break;
            }
        } //End Loop over vecIgnoreParam
        
        //Store the now all capital case, Unless it is to be ignored
        if (bIgnoreParam) {
            continue;
        }
        else{
            ret_strings.push_back(param);
        }
    } while (!exitFlag);
    
    return ret_strings;
} //End treeProducerTDC::getParsedFileName()

//For a line formatted like "text(text)=text;"
//Index of output vector goes as:
//  0->Tree Name; 1->Data Type; 2->Value/Identifier
void treeProducerTDC::setParsedLUTLine(string &inputLine, vector<string> &vec_strLUTIdents, string &strTreeName, string &strDataType, int &iMthdIdx, bool &bExitFlag){
    //Variable Declaration
    int iPos_Paren1 = inputLine.find("(",0);
    int iPos_Paren2 = inputLine.find(")",0);
    int iPos_Equals = inputLine.find("=",0);
    int iPos_Colon  = inputLine.find(":",0);
    int iPos_End    = inputLine.find(";",0);
    
    int iPos_LastComma = 0;
    
    string strStoredIdentifier;
    //string strTreeName, strDataType, strValueIdent, strMthIdx;
    
    vector<int> vec_iPos_Commas;
    
    //vector<string> ret_strings;
    //vector<string> commaStringContainer; //Temporary Container
    
    //Check to make sure iPos_Paren1 found in input string
    if (iPos_Paren1 == std::string::npos) {
        cout<<"Character '(' Not Found in line:\n";
        cout<<inputLine<<endl;
        cout<<"Exiting treeProducerTDC::getParsedLUTLine(), Cross-Check LUT File:\n";
        cout<<fileName_LUT<<endl;
        
        bExitFlag = false;
        
        //return ret_strings;
        return;
    }
    
    //Check to make sure iPos_Paren2 found in input string
    if(iPos_Paren2 == std::string::npos) {
        cout<<"Character ')' Not Found in line:\n";
        cout<<inputLine<<endl;
        cout<<"Exiting treeProducerTDC::getParsedLUTLine(), Cross-Check LUT File:\n";
        cout<<fileName_LUT<<endl;
        
        bExitFlag = false;
        
        //return ret_strings;
        return;
    }
    
    //Check to make sure iPos_Equals found in input string
    if (iPos_Equals == std::string::npos) {
        cout<<"Character '=' Not Found in line:\n";
        cout<<inputLine<<endl;
        cout<<"Exiting treeProducerTDC::getParsedLUTLine(), Cross-Check LUT File:\n";
        cout<<fileName_LUT<<endl;
        
        bExitFlag = false;
        
        //return ret_strings;
        return;
    }
    
    //Check to make sure iPos_Colon found in input string
    if (iPos_Colon == std::string::npos) {
        cout<<"Character ':' Not Found in line:\n";
        cout<<inputLine<<endl;
        cout<<"Exiting treeProducerTDC::getParsedLUTLine(), Cross-Check LUT File:\n";
        cout<<fileName_LUT<<endl;
        
        bExitFlag = false;
        
        //return ret_strings;
        return;
    }
    
    //Check to make sure iPos_End found in input string
    if (iPos_End == std::string::npos) {
        cout<<"Character ';' Not Found in line:\n";
        cout<<inputLine<<endl;
        cout<<"Exiting treeProducerTDC::getParsedLUTLine(), Cross-Check LUT File:\n";
        cout<<fileName_LUT<<endl;
        
        bExitFlag = false;
        
        //return ret_strings;
        return;
    }
    
    //Positions of Parameters Independent of Commas Found, Create Substrings
    strTreeName     = inputLine.substr(0,iPos_Paren1);
    strDataType     = inputLine.substr(iPos_Paren1+1,iPos_Paren2 - iPos_Paren1 - 1);
    
    iMthdIdx        = std::stoi( inputLine.substr(iPos_Colon+1,iPos_End - iPos_Colon - 1 ) );
    
    //Find Positions of All commas
    while ( inputLine.find(",",iPos_LastComma) != std::string::npos || iPos_LastComma >= iPos_Colon ) { //comma search loop
        //Find the Most Recent Comma;
        vec_iPos_Commas.push_back( inputLine.find(",",iPos_LastComma) );
        
        //Reset the position of the last comma to the next character after the last comma
        iPos_LastComma = inputLine.find(",",iPos_LastComma)+1;
    } //End comma search loop
    
    //Use comma knowledge to find Value Identifier(s)
    if (vec_iPos_Commas.size() > 0) { //Case: Multiple Value Identifiers
        //Loop Through All comma positions to find value identifiers
        //There are N+1 elements for N commas
        for (int i=0; i<= vec_iPos_Commas.size(); ++i) { //Loop Over vec_iPos_Commas
            if ( i == 0 ) { //Case: First Element
                strStoredIdentifier = inputLine.substr(iPos_Equals + 1,vec_iPos_Commas[i] - iPos_Equals - 1 );
            } //End Case: First Element
            else if( i == vec_iPos_Commas.size() ){ //Case: Last Element
                //Here i is out of range of vec_iPos_Commas, hence "i-1" index
                strStoredIdentifier = inputLine.substr(vec_iPos_Commas[i-1] + 1,iPos_Colon - vec_iPos_Commas[i-1] - 1 );
            } //End Case: Last Element
            else{ //Case: All Other Elements
                strStoredIdentifier = inputLine.substr(vec_iPos_Commas[i-1] + 1,vec_iPos_Commas[i] - vec_iPos_Commas[i-1] - 1 );
            } //End Case: All Other Elements
            
            //Transform the LUT Identifier
            std::transform(strStoredIdentifier.begin(),strStoredIdentifier.end(),strStoredIdentifier.begin(),toupper);
            
            //Store the LUT Identifier
            vec_strLUTIdents.push_back( strStoredIdentifier );
        } //End Loop Over vec_iPos_Commas
    } //End Case: Multiple Value Identifiers
    else{ //Case: Only One Value Identifier
        //Get the LUT Identifier
        strStoredIdentifier = inputLine.substr(iPos_Equals+1,iPos_Colon - iPos_Equals - 1 );
        
        //Transform the LUT Identifier
        std::transform(strStoredIdentifier.begin(),strStoredIdentifier.end(),strStoredIdentifier.begin(),toupper);
        
        //Store the LUT Identifier
        vec_strLUTIdents.push_back( strStoredIdentifier );
    } //End Case: Only One Value Identifier
    
    //transform to all capitals (allows case-insensitive comparison
    std::transform( strTreeName.begin(), strTreeName.end(), strTreeName.begin(), toupper);
    std::transform( strDataType.begin(), strDataType.end(), strDataType.begin(), toupper);
    //std::transform( strValueIdent.begin(), strValueIdent.end(), strValueIdent.begin(), toupper);
    
    //No need to transform strMthIdx as it is an integer (toupper does nothing to integers anyway)
    
    //Store Substrings
    //ret_strings.push_back( strTreeName );
    //ret_strings.push_back( strDataType );
    //ret_strings.push_back( strValueIdent );
    //ret_strings.push_back( strMthIdx );
    
    //Debugging
    if (verbose_LUT) { //Case: Verbose LUT Printing
        cout<<"For Tree Name = " << strTreeName << " List of Found Identifiers:\n";
        cout<<"i\tLUT Identifier\n";
        for (int i=0; i<vec_strLUTIdents.size(); ++i) {
            cout<<i<<"\t"<<vec_strLUTIdents[i]<<endl;
        }
    } //End Case: Verbose LUT Printing
    
    bExitFlag = true; //Exits Successfully
    
    return;
    //return ret_strings;
} //End treeProducerTDC::setParsedLUTLine()

//Prints All Bit Flags for an input ifstream
void treeProducerTDC::printStreamStatus(ifstream &inputStream){
    std::cout << "Input File Stream Bit Status:\n";
    std::cout << " good()=" << inputStream.good() << endl;
    std::cout << " eof()=" << inputStream.eof() << endl;
    std::cout << " fail()=" << inputStream.fail() << endl;
    std::cout << " bad()=" << inputStream.bad() << endl;
    
    return;
} //End treeProducerTDC::printStreamStatus()

//Sets the timing histogram:
void treeProducerTDC::setHistogram(string inputFileName, TH1F &inputHisto, int chanNum, bool &bExitFlag){
    //Open the Data File
    //------------------------------------------------------
    if (verbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeProducerTDC::setHistogram(): trying to open and read: " << inputFileName << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    TFile *file_ROOT = new TFile(inputFileName.c_str(), "READ", "", 1);
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if ( !file_ROOT->IsOpen() || file_ROOT->IsZombie() ) { //Case: Problem Opening File
        if (verbose_IO) {
            perror( ("treeProducerTDC::setHistogram(): error while opening file: " + inputFileName).c_str() );
            std::cout << "Input ROOT File Status:\n";
            std::cout << " IsOpen()=" << file_ROOT->IsOpen() << endl;
            std::cout << " IsZombie()=" << file_ROOT->IsZombie() << endl;
        }
        
        bExitFlag = false;
    } //End Case: Problem Opening File
    else{
        inputHisto = ( * ( (TH1F*) file_ROOT->Get( ( "TDC_Ch" + getString(chanNum) ).c_str() ) ) );
        
        bExitFlag = true;
    }
    
    //Close the file
    file_ROOT->Close();
    
    //Exit
    return;
} //End treeProducerTDC::setHistogram()

//As below, but for a single parameter entry.
void treeProducerTDC::setMappedParam(string &parsedInput, quad<string,vector<string>,dataType_t,int> &lutItem, /*Param &mappedParam,*/ TRunParameters &runLogger){
    
    //Debugging
    //cout<<"treeProducerTDC::setMappedParam() - parsedInput = " << parsedInput << endl;
    
    switch ( lutItem.third ) { //switch on dataType
        case typeBool:
            //Debugging
            //cout<<"Identifier(float) = " << lutItem.first << endl;
            //cout<<"parsedInput = " << parsedInput << endl;
            runLogger.setParameter( (parsedInput.compare("TRUE") == 0), lutItem.fourth );
            break;
        case typeFloat:
            //Debugging
            //cout<<"Identifier(float) = " << lutItem.first << endl;
            //cout<<"parsedInput = " << parsedInput << endl;
            runLogger.setParameter( std::stof(parsedInput), lutItem.fourth );
            break;
        case typeInt:
            //Debugging
            //cout<<"Identifier(int) = " << lutItem.first << endl;
            //cout<<"parsedInput = " << parsedInput << endl;
            runLogger.setParameter( std::stoi(parsedInput), lutItem.fourth );
            break;
        case typeString:
            runLogger.setParameter( parsedInput, lutItem.fourth );
            break;
        default:
            break;
    } //End switch on dataType
    
    return;
} //End setMappedParam() - Single

//Maps parameters found in the filename to the expected TTree Structure based on the definitions found in the lookup table
void treeProducerTDC::setMappedParam(list<string> &parsedFileNames, vector<quad<string,vector<string>,dataType_t,int> > &lookUpTable, /*Param &mappedParam,*/ TRunParameters &runLogger){
    //Variable Declaration
    
    //Idea here is to loop over lookUpTable and find all members of parsedFileNames that match
    //In case of multiple matches we rely on user input to select the correct one
    //Single matches are automatically assigned.
    for (auto iterLUT = lookUpTable.begin(); iterLUT != lookUpTable.end(); ++iterLUT) { //Loop Over lookUpTable
        vector<string> tempParam; //Temporary container
        
        //We will remove found matches from parsedFileNames
        //This is for speed and then to tell the user which items we did not match (if requested)

        //( *iterLUT ).first; //This is the Key Value
        //( *iterLUT ).second; //This is a VECTOR of Identifier(s)
        //( *iterLUT ).third; //This is the data type
        
        //Debugging
        //cout<<"\n=========Match List========="<<endl;
        //cout<<"nth_Match\tIdentifier\tPFN\n";
        
        //Debugging
        //cout<<"\n=========Match List========="<<endl;
        //cout<<"LUT Identifier\tPFN\n";
        
        //For Each LUT Element there exists a Vector of Identifiers (i.e. for BEAM element there exists the set {Muon,Muons,Pion,Pions,El,Electron,Electrons, etc...)
        for (auto iterLUTElement = ( (*iterLUT).second).begin(); iterLUTElement != ( (*iterLUT).second).end(); ++iterLUTElement) { //Loop Over Vector of Identifiers for a Single LUT Element ( (*iterLUT).second) )
            bool bMatchFound_LUTField = false;
            
            for (auto iterPFN = parsedFileNames.begin(); iterPFN != parsedFileNames.end(); /* No Increment*/ ) { //Loop Over parsedFileNames
                bool bMatchFound_PFN = false;    //Match Variable
                
                //Debugging
                //cout<<(*iterLUTElement)<<"\t"<<(*iterPFN)<<endl;
                
                //Check to see if the LUT identifier is longer than the PFN member
                //If it is, skip; if not check for match
                if ( (*iterLUTElement).length() > (*iterPFN).length() ) { //Case: LUT Identifier Length > PFN Member Length, SKIP!
                    ++iterPFN;
                    continue;
                } //End Case: LUT Identifier Length > PFN Member Length, SKIP!
                else{ //Case: LUT Identifier Length <= PFN Member Length, Search
                    int pfnLength = (*iterPFN).length();
                    int lutLength = (*iterLUTElement).length();
                    
                    if (pfnLength == lutLength && (*iterPFN).compare( (*iterLUTElement) ) == 0 ) { //Case: same length, attempt direct comparison
                        //Store the string
                        tempParam.push_back( *iterPFN );
                        
                        //Debugging
                        //cout<<tempParam.size()<< "\t" <<(*iterLUTElement)<< "\t" << (*iterPFN) << endl;
                        
                        bMatchFound_PFN = true;
                    } //Case: same length, attempt direct comparison
                    
                    //Search Algorithm: Bubble
                    for (int i=0; i < (pfnLength - lutLength) && !bMatchFound_PFN; ++i) { //Loop over (*iterPFN)
                        if ( (*iterPFN).compare(i,lutLength, (*iterLUTElement) ) == 0 ) { //Case: Match Found!
                            
                            //Store the string
                            //Two cases
                            //  1-> Characters exist after comparison string, take those
                            //  2-> Characters exist before comparison string, take those
                            if (i==0) { //Case 1: Take characters after comparison string
                                tempParam.push_back( (*iterPFN).substr(lutLength, pfnLength - lutLength ) );
                            } //End Case 1: Take characters after comparison string
                            else{ //Case 2: Take characters before comparison string
                                tempParam.push_back( (*iterPFN).substr(0,i) );
                            } //Case 2: Take characters before comparison string
                            
                            //Debugging
                            //cout<<tempParam.size()<< "\t" <<(*iterLUTElement) << "\t" << (*iterPFN) << endl;
                            
                            bMatchFound_PFN = true;
                            break; //terminates the loop over characters in (*iterPFN)
                        } //End Case: Match Found!
                    } //End Loop over (*iterPFN)
                    
                    //Search Algorithm: Modified Bubble (Telescope)
                    //Here we look to see if the (*iterPFN) is a dissected (*iterLUT) with something inbetween the pieces of (*iterLUT)
                    //i.e. (*iterPFN) = DLYZZns is a dissected DLYns identifer with ZZ in the middle
                    for (int i=0; (i < (lutLength - 1) ) && !bMatchFound_PFN; ++i) { //Loop Over get<1>(*iterLUT), i.e. LUT Identifier
                        //Generate Telescoped Sub-Strings
                        string strLUTPart1 = (*iterLUTElement).substr(0,i);
                        string strLUTPart2 = (*iterLUTElement).substr(i,lutLength - i);
                        
                        //Search for LUT pieces in (*iterPFN)
                        if ( ((*iterPFN).substr(0,strLUTPart1.length() ) ).compare(strLUTPart1) == 0 && ((*iterPFN).substr(pfnLength - strLUTPart2.length(), strLUTPart2.length() ) ).compare(strLUTPart2) == 0 ) { //Case: Match Found!
                            tempParam.push_back( (*iterPFN).substr( strLUTPart1.length(), pfnLength - strLUTPart1.length() - strLUTPart2.length() ) );
                            
                            //Debugging
                            //cout<<tempParam.size()<<"\t"<< (*iterLUTElement) <<"\t" << (*iterPFN) << endl;
                            
                            bMatchFound_PFN = true;
                            break; //terminates the loop over characters in get<1>(*iterLUT), i.e. LUT Identifier
                        } //End Case: Match Found!
                    } //End Loop Over get<1>(*iterLUT), i.e. LUT Identifier
                } //Case: LUT Identifier Length <= PFN Member Length, Search
                
                //Check for Match & Iterate
                if (bMatchFound_PFN) { //Case: matchFound
                    //Here iterPFN is sent to the erase command, then iterPFN is incremented to iterPFN+1, then value at iterPFN is erased
                    
                    //Debugging
                    //cout<<"iterPFN Position = " << std::distance(iterPFN,parsedFileNames.begin()) << endl;
                    //cout<<"PFN Size = " << parsedFileNames.size() << endl;
                    //cout<<"iterPFN = " << (*iterPFN) << endl;
                    
                    //Erase the current position and get a pointer to the next position
                    iterPFN = parsedFileNames.erase(iterPFN);
                    
                    //Debugging
                    //cout<<"iterPFN Position = " << std::distance(iterPFN,parsedFileNames.begin()) << endl;
                    //cout<<"PFN Size = " << parsedFileNames.size() << endl;
                    //cout<<"iterPFN = " << (*iterPFN) << endl;
                    
                    //Flag this LUT Field as being matched
                    bMatchFound_LUTField = true;   //Numerous PFN's can be matched to this; but o
                } //End Case: matchFound
                else { //Case: No Match Found; Move to Next Item
                    ++iterPFN;
                } //End Case: No Match Found; Move to Next Item
            } //End Loop Over parsedFileNames
            
            //Move to next LUT Field if a LUT Element from this Field is found to have match(es)
            //Numerous PFN's can be matched to one LUT Field
            //But once a LUT Element Identifier, for a given LUT Field, is matched no further elements should be checked for that Field
            if (bMatchFound_LUTField) break;
        } //End Loop Over Vector of Identifiers for a Single LUT Element (i.e. (*iterLUT).second) )
        
        //Check to see if any parameters are found!
        if (tempParam.size() == 0) { //Case: No Parameters Found
            //Verbose Printing Mode, Inform User No Match Found
            if (verbose_LUT) {
                cout<<"No Parameters found matching LUT Field " << ( *iterLUT ).first << endl;
            }
            
            continue;
        } //End Case: No Parameters Found
        
        //Debugging
        /*cout<<"i\tIdentifier\tFoundParam\n";
        for (auto iterTempParam = tempParam.begin(); iterTempParam != tempParam.end(); ++iterTempParam) {
            cout<<iterTempParam - tempParam.begin() << "\t" << ( *iterLUT ).first << "\t" << (*iterTempParam) << endl;
        }*/
        
        //If we make it here parameters have been found; two cases exist:
        // 1-> Only one parameter, store immediately
        // 2-> More than one parameter exists, rely on user input
        if (tempParam.size() == 1) { //Case: Only 1 Parameter Found
            //setMappedParam(tempParam[0], (*iterLUT), mappedParam, runLogger );
            setMappedParam(tempParam[0], (*iterLUT), runLogger );
        } //End Case: Only 1 Parameter Found
        else if (tempParam.size() > 1){ //Case: Multiple Parameters Found, Rely On User Input
            //Check to Make Sure the Found Number of Parameters is compatible with expected Detector Position
            if ( runLogger.getDetPos() > tempParam.size() ) { //Case: Incompatible
                cout<<"ERROR!!! REQUESTED DETECTOR POSITION " << runLogger.getDetPos();
                cout<<" BUT ONLY FOUND " << tempParam.size() << " PARAMETERS!!!" << endl;
                cout<<"ERROR!!! SKIPPING REQUESTED INPUT: " << ( *iterLUT ).first << endl;
                cout<<"ERROR!!! PLEASE CROSS CHECK INPUT FILES!!!" << endl;
            } //End Case: Incompatible
            else { //Case: Compatible
                //setMappedParam(tempParam[(mappedParam.mapDataInt[keyVal_DetPos]).first], (*iterLUT), mappedParam, runLogger);
                setMappedParam(tempParam[runLogger.getDetPos()], (*iterLUT), runLogger);
            } //Case: Compatible
        } //End Case: Multiple Parameters Found, Rely On User Input
    } //End Loop Over lookUpTable
    
    return;
} //End treeProducerTDC::setMappedParam() - Multi

//Sets all information for the current run
void treeProducerTDC::setRun(string inputROOTFileName, string inputLUTFileName, /*Param &mappedParam,*/ TRunParameters &runLogger){
    //Variable Declaration
    bool bExitFlag = false;
    
    list<string> parsedFileNames  = getParsedFileName(inputROOTFileName);
    vector<quad<string, vector<string>, dataType_t, int> > lookUpTable = getLookUpTable(inputLUTFileName, runLogger);
    
    //TH1F *timingHisto;
    //TH1F timingHisto;
    
    TSpectrum *timingSpec = new TSpectrum();
    
    //Loop Over Stored quad's
    /*cout<<"===========Stored Auto Search Parameters===========\n";
    cout<<"idx\tFirst\t{Second}\tThird\tFourth\n";
    for (auto iterLUT = lookUpTable.begin(); iterLUT != lookUpTable.end(); ++iterLUT) { //Loop Over Stored quad's
        cout<<iterLUT - lookUpTable.begin()<<"\t"<<(*iterLUT).first<<"\t";
     
        for(int i=0; i<((*iterLUT).second).size(); ++i){
            cout<<((*iterLUT).second)[i]<<"\t";
        }
     
        cout<<(*iterLUT).third<<"\t"<<(*iterLUT).fourth<<endl;
    } //End Loop Over Stored quad's
    */
    
    //Loop Over Stored parsed File Name
    if (verbose_PFN) {
        cout<<"===========Stored Parsed File Name===========\n";
        cout<<"idx\tItem\n";
        for (auto iterPFN = parsedFileNames.begin(); iterPFN != parsedFileNames.end(); ++iterPFN) { //Loop Over Stored Parsed File Name
            cout<<std::distance(iterPFN,parsedFileNames.begin()) << "\t" << (*iterPFN) << endl;
        } //End Loop Over Stored Parsed File Name
    } //End Case: User Requested Output of PFN
    
    //Map the Parameters
    setMappedParam(parsedFileNames, lookUpTable, runLogger);
    
    //Set the gain
    runLogger.getDetGainIndepVarIsCurrent() ? runLogger.calcGain( runLogger.getDetCurrent() ) : runLogger.calcGain( runLogger.getDetDriftV() );
    
    //Get the Histogram
    //timingHisto = getHistogram(inputROOTFileName, runLogger.getTDCChanNumber() );
    
    //Set the Histogram
    setHistogram(inputROOTFileName, timingHisto, runLogger.getTDCChanNumber(), bExitFlag );
    
    //Check to make sure we do not have a null pointer
    //if ( timingHisto == nullptr ) { //Case: Histo is Null
    if ( !bExitFlag ) { //Case: Histo is Null
        //cout<<"ERROR!!! REQUESTED TIMING HISTOGRAM: TDC_Ch" << runLogger.getTDCChanNumber() << " RETURNS NULL POINTER" << endl;
        cout<<"ERROR!!! REQUESTED TIMING HISTOGRAM: TDC_Ch" << runLogger.getTDCChanNumber() << " NOT SET CORRECTLY" << endl;
        cout<<"ERROR!!! PLEASE CROSS CHECK INPUT ROOT FILE: " << inputROOTFileName << endl;
        cout<<"ERROR!!! PLEASE CROSS CHECK INPUT LUT FILE: " << inputLUTFileName << endl;
        
        return;
    } //End Case: Histo is Null
    else{ //Case: Histo is Non-Null
        //Rebin timingHisto if requested;
        if (iRebinFactor > 0) timingHisto.Rebin(iRebinFactor);
        
        //Change the name of the histogram to be unique (i.e. tied to run number)
        timingHisto.SetName( ("timingHisto_R" + getString( runLogger.getRunNumber() ) ).c_str() );
        
        timingHisto.GetXaxis()->SetRangeUser( timingHisto.GetMean() - 5. * timingHisto.GetRMS(),  timingHisto.GetMean() + 5. * timingHisto.GetRMS() );
        
        //Store the Histogram in runLogger
        //runLogger.setTDCHisto( timingHisto );
        runLogger.setTDCHisto( &timingHisto );
        
        //Set Numeric Data From Histogram - runLogger
        runLogger.setTDCHistoMean( timingHisto.GetMean() );
        runLogger.setTDCHistoRMS( timingHisto.GetRMS() );
        //runLogger.setTDCHistoPks( timingSpec->Search(timingHisto,3,"goff",0.001) ); //goff -> Do Not Draw the Polymarker
        runLogger.setTDCHistoPks( timingSpec->Search(&timingHisto,3,"goff",0.001) ); //goff -> Do Not Draw the Polymarker
        //runLogger.setTDCHistoPks( timingSpec->Search(timingHisto,3,"",0.005) );   //Let's Use This One
        //runLogger.setTDCHistoPks( timingSpec->Search(timingHisto,3,"",0.01) );
        
        //Check Trigger Mode and Act Accordingly
        if (runLogger.getTrigMode() == 0) { //Case: Async Trigger Mode
            //Clone the Histogram and declare the Fit
            
            //[0] -> Amp
            //[1] -> Mean
            //[2] -> Std. Dev
            TF1 *fit_Gaus   = new TF1( ("fit_Gaus_R" + getString( runLogger.getRunNumber() ) ).c_str(),"[0]*exp(-0.5*((x-[1])/[2])**2)",0,1000);
            
            //[0] -> Amp
            //[1] -> Mean position
            //[2] -> Std. Dev (pre-convolution time response)
            TF1 *fit_Convo  = new TF1( ("fit_Convo_R" + getString( runLogger.getRunNumber() ) ).c_str(),"[0]*[2]*sqrt(TMath::Pi()/2.)*(TMath::Erf( (12.5 + (x-[1])) / ([2] * sqrt(2.) ) ) - TMath::Erf( ((x-[1]) - 12.5) / ([2] * sqrt(2.) ) ) )",0,1000);
            
            //Check the number of peaks found; and act appropriately
            if ( runLogger.getTDCHistoPks() == 0) { //Case: Zero Peaks Found
                fit_Gaus->SetParameter(0, -1.);
                fit_Gaus->SetParameter(1, -1.);
                fit_Gaus->SetParameter(2, -1.);
                
                fit_Convo->SetParameter(0, -1.);
                fit_Convo->SetParameter(1, -1.);
                fit_Convo->SetParameter(2, -1.);
            } //End Case: Zero Peaks Found
            else if ( runLogger.getTDCHistoPks() >= 1) { //Case: One Peak Found
                fit_Gaus->SetParameter(1, timingHisto.GetMean() );
                fit_Gaus->SetParameter(2, timingHisto.GetRMS() );
                timingHisto.Fit(fit_Gaus, fitOption.c_str() );
                
                fit_Convo->SetParameter(0, timingHisto.GetMaximum() );
                fit_Convo->SetParameter(1, timingHisto.GetMean() );
                fit_Convo->SetParameter(2, timingHisto.GetRMS() );
                
                fit_Convo->SetParLimits(2,0.,1e6);
                
                timingHisto.Fit(fit_Convo, fitOption.c_str(), "", timingHisto.GetMean() - 4. * timingHisto.GetRMS(), timingHisto.GetMean() + 4. * timingHisto.GetRMS() );
            } //End Case: One Peak Found
            
            //Store Numeric Data From Fit - runLogger
            runLogger.setTDCFit( fit_Gaus );
            runLogger.setTDCFitAmp( (float) fit_Gaus->GetParameter(0) );
            runLogger.setTDCFitAmpErr( (float) fit_Gaus->GetParError(0) );
            runLogger.setTDCFitMean( (float) fit_Gaus->GetParameter(1) );
            runLogger.setTDCFitMeanErr( (float) fit_Gaus->GetParError(1) );
            runLogger.setTDCFitSigma( (float) fit_Gaus->GetParameter(2) );
            runLogger.setTDCFitSigmaErr( (float) fit_Gaus->GetParError(2) );
            runLogger.setTDCFitChi2( (float) fit_Gaus->GetChisquare() );
            runLogger.setTDCFitNDF( (float) fit_Gaus->GetNDF() );
            
            runLogger.setTDCFitConvo( fit_Convo );
            runLogger.setTDCFitConvoAmp( (float) fit_Convo->GetParameter(0) );
            runLogger.setTDCFitConvoAmpErr( (float) fit_Convo->GetParError(0) );
            runLogger.setTDCFitConvoMean( (float) fit_Convo->GetParameter(1) );
            runLogger.setTDCFitConvoMeanErr( (float) fit_Convo->GetParError(1) );
            runLogger.setTDCFitConvoSigma( (float) fit_Convo->GetParameter(2) );
            runLogger.setTDCFitConvoSigmaErr( (float) fit_Convo->GetParError(2) );
            runLogger.setTDCFitConvoChi2( (float) fit_Convo->GetChisquare() );
            runLogger.setTDCFitConvoNDF( (float) fit_Convo->GetNDF() );
            
            runLogger.setTDCNumDeconvoTimeResp( (float) func_NumericDeconvoModel->Eval( timingHisto.GetRMS() ) );
        } //End Case: Async Trigger Mode
        else if(runLogger.getTrigMode() == 1){ //Case: Sync Trigger Mode
            //Loop Over Found Peaks
            Double_t *dHistoPeakPos_X = timingSpec->GetPositionX();
            
            vector<std::pair<float,float> > vec_PksInfo; //pair.first -> Integral; pair.second -> X-Pos
            
            //NOTE: This could be done with a map<float,float> but I want random access "[]" for easy usage
            
            //Get All the Peaks
            cout<<"============================================" << endl;
            cout<<"treeProducerTDC::setRun() - Searching for Peaks\n";
            for (int i=0; i<timingSpec->GetNPeaks(); ++i) { //Loop Through Spec Peaks
                //vec_PksInfo.push_back(std::make_pair( getPeakIntegral(timingHisto, float (dHistoPeakPos_X[i]) ), float (dHistoPeakPos_X[i]) ) );
                vec_PksInfo.push_back(std::make_pair( getPeakIntegral(&timingHisto, float (dHistoPeakPos_X[i]) ), float (dHistoPeakPos_X[i]) ) );
                
                cout<<"treeProducerTDC::setRun() - Peak Found! Continuing Search!\n";
                
                //Map comparison by default is std::less which acts like "<" operator
                //Upon completion map will be ordered by increasing key value
                //map_Peaks[ float (dHistoPeakPos_X[i]) ] = getPeakIntegral(timingHisto, float (dHistoPeakPos_X[i]) );
            } //End Loop Through Spec Peaks
            cout<<"treeProducerTDC::setRun() - All Peaks Found!\n";
            cout<<"============================================" << endl;
            
            //Sort the peaks in Ascending Order
            std::sort(vec_PksInfo.begin(), vec_PksInfo.end() );
            
            //The Peaks are now sorted Lowest to Highest Integral
            //NOTE: if Peak integrals are same they are then sorted by peak position (default std::pair comparator)
            
            //Store the Peaks
            if ( vec_PksInfo.size() >= 3 ) {
                runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
                runLogger.setTDCHistoPksInt_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).first );
                runLogger.setTDCHistoPksInt_3rdMax( (vec_PksInfo[vec_PksInfo.size()-3]).first );
                
                runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
                runLogger.setTDCHistoPksPos_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).second );
                runLogger.setTDCHistoPksPos_3rdMax( (vec_PksInfo[vec_PksInfo.size()-3]).second );
            }
            else if ( vec_PksInfo.size() >= 2 ) {
                runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
                runLogger.setTDCHistoPksInt_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).first );
                
                runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
                runLogger.setTDCHistoPksPos_2ndMax( (vec_PksInfo[vec_PksInfo.size()-2]).second );
            }
            else if ( vec_PksInfo.size() >= 1 ) {
                runLogger.setTDCHistoPksInt_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).first );
                
                runLogger.setTDCHistoPksPos_1stMax( (vec_PksInfo[vec_PksInfo.size()-1]).second );
            }
        } //End Case: Sync Trigger Mode
        else{ //Case: Unrecognized Trigger Mode
            cout<<"ERROR!!! TRIGGER MODE " << runLogger.getTrigMode() << " NOT RECOGNIZED\n";
            cout<<"ERROR!!! PLEASE CROSS CHECK INPUT ROOT FILE: " << inputROOTFileName << endl;
            cout<<"ERROR!!! PLEASE CROSS CHECK INPUT LUT FILE: " << inputLUTFileName << endl;
            
            return;
        } //End Case: Unrecognized Trigger Mode
    } //End Case: Histo is Non-Null
    
    return;
} //End treeProducerTDC::setRun()
