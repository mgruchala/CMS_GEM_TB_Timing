//
//  TRunParameters.cpp
//  
//
//  Created by Brian L Dorney on 18/03/15.
//
//

#include "TRunParameters.h"

//Constructor
TRunParameters::TRunParameters(){
    /*run.iRun = -1;           //Run Number
    
    run.iBeam_Type = 0;     //Initialized to 0; 11->Electron, 13->Muon, 211->Pion
    
    run.iTrig_Mode = -1;     //0->Async, 1->Sync
    
    run.fTrig_Delay = -1;
    
    //Detector Info
    run.iDet_Pos = -1;       //Position of Detector of Interest in Listing
    run.iDet_Eta = -1;       //iEta value of readout sector
    run.iDet_Phi = -1;       //iPhi value of readout sector
    
    run.fDet_Imon = -1;    //Current through the Divider
    run.fDet_VDrift = -1;  //Potential on the Drift
    
    run.fDet_GasFrac_Ar = -1;  //Argon Fraction in Gas
    run.fDet_GasFrac_CO2 = -1; //CO2 "                 "
    run.fDet_GasFrac_CF4 = -1; //CF4 "                 "
    
    run.strDet_Name = ""; //Detector Name
    
    //VFAT Info
    run.iTURBO_ID = -1;      //TURBO ID, 0->Master, 1->Slave 1, 2->Slave 2, etc...
    run.iVFAT_Pos = -1;      //VFAT Position on TURBO
    
    run.strVFAT_ID = ""; //VFAT ID, hex code
    
    run.iVFAT_IPreAmpIn = -1;
    run.iVFAT_IPreAmpFeed = -1;
    run.iVFAT_IPreAmpOut = -1;
    
    run.iVFAT_IShaper = -1;
    run.iVFAT_IShaperFeed = -1;
    
    run.iVFAT_IComp = -1;        //Current to Comparator
    
    run.iVFAT_MSPL = -1;         //Monostable Pulse Length
    
    run.iVFAT_Latency = -1;      //VFAT Latency
    
    run.fVFAT_Thresh = -1;     //VFAT Threshold in fC
    
    //TDC Info
    run.iTDC_CH_Number = -1;
    
    run.fTDC_Histo_Mean = -1;
    run.fTDC_Histo_RMS = -1;
    run.fTDC_Histo_nPks = -1;
    
    run.fTDC_Histo_PkInt_1stMax = -1;
    run.fTDC_Histo_PkInt_2ndMax = -1;
    run.fTDC_Histo_PkInt_3rdMax = -1;
    
    run.fTDC_Histo_PkPos_1stMax = -1;
    run.fTDC_Histo_PkPos_2ndMax = -1;
    run.fTDC_Histo_PkPos_3rdMax = -1;
    
    run.fTDC_Fit_Amp = -1;
    run.fTDC_Fit_Mean = -1;
    run.fTDC_Fit_Sigma = -1;
    run.fTDC_Fit_Chi2 = -1;
    run.fTDC_Fit_NDF = -1;
    
    
    run.fTDC_Fit_Convo_Amp = -1;
    run.fTDC_Fit_Convo_Mean = -1;
    run.fTDC_Fit_Convo_Sigma = -1;
    run.fTDC_Fit_Convo_Chi2 = -1;
    run.fTDC_Fit_Convo_NDF = -1;
    
    run.fTDC_NumDeconvo_TimeResp = -1;
    
    run.func_Gaus = nullptr;
    run.func_Convo = nullptr;
    
    run.hTDC_Histo = nullptr;*/
} //End Constructor

//Determing the gain of the detector; assumes the following parameters ahve already been set:
//  fDet_Gain_Const;     //Gain Curve - Amplitude
//  fDet_Gain_Const_Err; //Gain Curve - Amplitude, Error
//  fDet_Gain_Slope;     //Gain Curve - Exponential Slope
//  fDet_Gain_Slope_Err; //Gain Curve - Exponential Slope, Error
//The input fDepVar is the physical quantity which parameterizes the gain (e.g. VDrift, Imon, etc...)
void TRunParameters::calcGain(float fDepVar){
    //run.fDet_Gain       = run.fDet_Gain_Const * exp( run.fDet_Gain_Slope * fDepVar );
    //run.fDet_Gain_Err   = exp(run.fDet_Gain_Slope * fDepVar) * sqrt( pow(run.fDet_Gain_Const_Err, 2) + pow(run.fDet_Gain_Slope_Err * run.fDet_Gain_Const * fDepVar, 2) );
    
    run.fDet_Gain       = exp(run.fDet_Gain_Const + run.fDet_Gain_Slope * fDepVar );
    run.fDet_Gain_Err   = sqrt( exp(2. * run.fDet_Gain_Const + run.fDet_Gain_Slope * fDepVar) * ( pow(run.fDet_Gain_Const_Err, 2) + pow(fDepVar * run.fDet_Gain_Slope_Err, 2) ) );
    
    return;
} //End Case: TRunParameters::calcGain()

//As Above, but sets the input gain variables.
void TRunParameters::calcGain(float fDepVar, float &fGain, float &fGain_Err){
    calcGain(fDepVar);
    
    fGain       = getDetGain();
    fGain_Err   = getDetGainErr();
    
    return;
} //End Case: TRunParameters::calcGain()

//Set Run; use 1st character of input to convert to ASCII Key Code then assign proper PDG ID
//NOTE: This method fails if Proton and Pion are ever used in the same run
void TRunParameters::setBeam(std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    char firstChar;
    
    if (input.length() > 0) {
        firstChar = input[0];
    }
    else{
        run.iBeam_Type = 0;
        
        std::cout<<"TRunParameters::setBeam() - Expected input string::length() > 1\n";
        std::cout<<"TRunParameters::setBeam() - Given: '" << input << "' with length " << input.length() << std::endl;
        std::cout<<"TRunParameters::setBeam() - Please Cross-Check Input" << std::endl;
    }
    
    //Maybe instead of switch statement we should adopt what is used in setTrigMode()???
    
    switch ( (int) firstChar ) {
        case 69:    //Electron, |pdg id| = 11
            run.iBeam_Type = 11;
            break;
        case 77:    //Muon, |pdg id| = 13
            run.iBeam_Type = 13;
            break;
        case 80:    //Pion, |pdg id| = 211 (assuming charged pion)
            run.iBeam_Type = 211;
            break;
        default:    //Unrecognized
            run.iBeam_Type = 0;
            std::cout<<"TRunParameters::setBeam() - Unrecognized Input!!!\n";
            std::cout<<"TRunParameters::setBeam() - Was given " << input << std::endl;
            std::cout<<"TRunParameters::setBeam() - Expected 1st Letter to be from set {'E','P','M'}\n";
            break;
    } //End Switch
    
    return;
} //End TRunParameters::setBeam()

//Set Trig Mode; use 1st character of input to convert to ASCII Key Code then assign 1 or 0 based on input
void TRunParameters::setTrigMode(std::string input){
    //char firstChar = (input.substr(0,1)).c_str();
    
    if ( (input.substr(0,1)).compare("A") == 0 ) {
        run.iTrig_Mode = 0;
    }
    else if ( (input.substr(0,1)).compare("S") == 0 ) {
        run.iTrig_Mode = 1;
    }
    else {
        run.iTrig_Mode = -1;
        std::cout<<"TRunParameters::setTrigMode() - Unrecognized Input!!!\n";
        std::cout<<"TRunParameters::setTrigMode() - Was given " << input << std::endl;
        std::cout<<"TRunParameters::setTrigMode() - Expected 1st Letter to be 'A' or 'S'\n";
    }
    
    return;
} //End TRunParameters::setTrigMode()

//Set boolean type parameters
void TRunParameters::setParameter(bool input, int methodIdx){
    switch (methodIdx) {
            //Run Methods                               //Expected Input Data Type
            
            //Detector Methods
        case 114: setDetGainIndepVarIsCurrent(input); break;   //boolean
            
            //VFAT Methods
            
            //TDC Methods
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << methodIdx << " for STRING data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set int type parameters
void TRunParameters::setParameter(int input, int methodIdx){
    switch (methodIdx) {
            //Run Methods                               //Expected Input Data Type
        case 1: setRunNumber(input); break;         //int
            
            //Detector Methods
        case 101: setDetPos(input); break;          //int
        case 102: setDetEta(input); break;          //int
        case 103: setDetPhi(input); break;          //int
            
            //VFAT Methods
        case 201: setTURBOID(input); break;         //int
        case 202: setVFATPos(input); break;         //int
        case 204: setVFATIPreAmpIn(input); break;   //int
        case 205: setVFATIPreAmpFeed(input); break; //int
        case 206: setVFATIPreAmpOut(input); break;  //int
        case 207: setVFATIShaper(input); break;     //int
        case 208: setVFATIShaperFeed(input); break; //int
        case 209: setVFATIComp(input); break;       //int
        case 210: setVFATMSPL(input); break;        //int
        case 211: setVFATLatency(input); break;     //int
        case 212: setVFATThresh(input); break;      //int
            
            //TDC Methods
        case 301: setTDCChanNumber(input); break;   //int
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << methodIdx << " for INT data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set float type parameters
void TRunParameters::setParameter(float input, int methodIdx){
    switch (methodIdx) {
        //Run Methods                               //Expected Input Data Type
        case 4: setTrigDelay(input); break;         //float
            
        //Detector Methods
        case 104: setDetCurrent(input); break;      //float
        case 105: setDetDriftV(input); break;       //float
        case 107: setDetGasFracAr(input); break;  //float
        case 108: setDetGasFracCO2(input); break; //float
        case 109: setDetGasFracCF4(input);          break; //float
        case 110: setDetGainConst(input); break;              //float
        case 111: setDetGainConstErr(input); break;           //float
        case 112: setDetGainSlope(input); break;              //float
        case 113: setDetGainSlopeErr(input); break;           //float
            
        //VFAT Methods
            
        //TDC Methods
        case 303: setTDCHistoMean(input); break;           //float
        case 304: setTDCHistoRMS(input); break;            //float
        case 305: setTDCHistoPks(input); break;            //float
        case 307: setTDCFitAmp(input); break;              //float
        case 308: setTDCFitAmpErr(input); break;           //float
        case 309: setTDCFitMean(input); break;             //float
        case 310: setTDCFitMeanErr(input); break;          //float
        case 311: setTDCFitSigma(input); break;            //float
        case 312: setTDCFitSigmaErr(input); break;         //float
        case 313: setTDCFitChi2(input); break;             //float
        case 314: setTDCFitNDF(input); break;              //float
        case 316: setTDCFitConvoAmp(input); break;         //float
        case 317: setTDCFitConvoAmpErr(input); break;      //float
        case 318: setTDCFitConvoMean(input); break;        //float
        case 319: setTDCFitConvoMeanErr(input); break;     //float
        case 320: setTDCFitConvoSigma(input); break;       //float
        case 321: setTDCFitConvoSigmaErr(input); break;    //float
        case 322: setTDCFitConvoChi2(input); break;        //float
        case 323: setTDCFitConvoNDF(input); break;         //float
        case 324: setTDCNumDeconvoTimeResp(input); break;  //float
        case 325: setTDCHistoPksInt_1stMax(input); break;  //float
        case 326: setTDCHistoPksInt_2ndMax(input); break;  //float
        case 327: setTDCHistoPksInt_3rdMax(input); break;  //float
        case 328: setTDCHistoPksPos_1stMax(input); break;  //float
        case 329: setTDCHistoPksPos_2ndMax(input); break;  //float
        case 330: setTDCHistoPksPos_3rdMax(input); break;  //float
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << methodIdx << " for FLOAT data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()

//Set string type parameters
void TRunParameters::setParameter(std::string input, int methodIdx){
    switch (methodIdx) {
            //Run Methods                               //Expected Input Data Type
        case 2: setBeam(input); break;              //string
        case 3: setTrigMode(input); break;          //string
            
            //Detector Methods
        case 106: setDetName(input); break;         //string
            
            //VFAT Methods
        case 203: setVFATID(input); break;          //string
            
            //TDC Methods
            
            //methodIdx Not Recognized
        default:
            std::cout<<"=================================================\n";
            std::cout<<"TRunParameters::setParameter() - methodIdx = " << methodIdx << " for STRING data type NOT recognized\n";
            std::cout<<"TRunParameters::setParameter() - Performing No Action\n";
            std::cout<<"TRunParameters::setParameter() - Please Check Input Again\n";
            std::cout<<"=================================================\n";
            break;
    } //End switch on methodIdx
    
    return;
} //End TRunParameters::setParameter()