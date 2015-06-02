//
//  TRunParameters.h
//  
//
//  Created by Brian L Dorney on 18/03/15.
//
//

#ifndef ____TRunParameters__
#define ____TRunParameters__

//C++ Includes
#include <cmath>
#include <iostream>
//#include <math>
#include <stdio.h>
#include <string>

//ROOT Includes
#include "TF1.h"
#include "TH1F.h"
#include "TObject.h"

/*  METHOD INDEX GUIDE
 
 //Run Methods                              //Expected Input Data Type
 case 1: setRun(input); break;              //int
 case 2: setBeam(input); break;             //string
 case 3: setTrigMode(input); break;         //string
 case 4: setTrigDelay(input); break;        //float
 
 //Detector Methods
 case 101: setDetPos(input); break;         //int
 case 102: setDetEta(input); break;         //int
 case 103: setDetPhi(input); break;         //int
 case 104: setDetCurrent(input); break;     //float
 case 105: setDetDriftV(input); break;      //float
 case 106: setDetName(input); break;        //      string
 case 107: setDetGasFracAr(input); break;   //float
 case 108: setDetGasFracCO2(input); break;  //float
 case 109: setDetGasFracCF4(input); break;  //float
 case 110: setDetGainConst(input); break;   //float
 case 111: setDetGainConstErr(input); break;//float
 case 112: setDetGainSlope(input); break;   //float
 case 113: setDetGainSlopeErr(input); break;//float
 case 114: setDetGainIndepVarIsCurrent(input); break;   //boolean
 
 //VFAT Methods
 case 201: setTURBOID(input); break;         //int
 case 202: setVFATPos(input); break;         //int
 case 203: setVFATID(input); break;          //     string
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
 case 301: setTDCChanNumber(input); break;          //int
 case 302: setTDCHisto(input); break;               //      TH1F*
 case 303: setTDCHistoMean(input); break;           //float
 case 304: setTDCHistoRMS(input); break;            //float
 case 305: setTDCHistoPks(input); break;            //float
 case 306: setTDCFit(input); break;                 //      TF1*
 case 307: setTDCFitAmp(input); break;              //float
 case 308: setTDCFitAmpErr(input); break;           //float
 case 309: setTDCFitMean(input); break;             //float
 case 310: setTDCFitMeanErr(input); break;          //float
 case 311: setTDCFitSigma(input); break;            //float
 case 312: setTDCFitSigmaErr(input); break;         //float
 case 313: setTDCFitChi2(input); break;             //float
 case 314: setTDCFitNDF(input); break;              //float
 case 315: setTDCFitConvo(input); break;            //      TF1*
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
 
 */

//Run Info
//Most values initialized to -1 (iBeam_Type to 0)
//Pointers initialized to nullptr
struct Run{
    int iRun = -1;           //Run Number
    
    int iBeam_Type = 0;     //11->Electron, 13->Muon, 211->Pion
    
    int iTrig_Mode = -1;     //0->Async, 1->Sync
    
    float fTrig_Delay = -1;    //Trigger Delay in ns;
    
    //Detector Info
    bool bDet_Gain_IndepVar_Imon = true;
    
    int iDet_Pos = -1;       //Position of Detector of Interest in Listing
    int iDet_Eta = -1;       //iEta value of readout sector
    int iDet_Phi = -1;       //iPhi value of readout sector
    
    float fDet_Imon = -1;    //Current through the Divider
    float fDet_VDrift = -1;  //Potential on the Drift
    
    float fDet_Gain;            //Gain, given by f(x) = exp(Const + Slope * x)
    float fDet_Gain_Err;        //Gain Error
    float fDet_Gain_Const;      //Gain Curve - Amplitude
    float fDet_Gain_Const_Err;  //Gain Curve - Amplitude, Error
    float fDet_Gain_Slope;      //Gain Curve - Exponential Slope
    float fDet_Gain_Slope_Err;  //Gain Curve - Exponential Slope, Error
    
    float fDet_GasFrac_Ar = -1;  //Argon Fraction in Gas
    float fDet_GasFrac_CO2 = -1; //CO2 "                 "
    float fDet_GasFrac_CF4 = -1; //CF4 "                 "
    
    std::string strDet_Name = ""; //Detector Name
    
    //VFAT Info
    int iTURBO_ID = -1;      //TURBO ID, 0->Master, 1->Slave 1, 2->Slave 2, etc...
    int iVFAT_Pos = -1;      //VFAT Position on TURBO
    
    std::string strVFAT_ID = ""; //VFAT ID, hex code
    
    int iVFAT_IPreAmpIn = -1;
    int iVFAT_IPreAmpFeed = -1;
    int iVFAT_IPreAmpOut = -1;
    
    int iVFAT_IShaper = -1;
    int iVFAT_IShaperFeed = -1;
    
    int iVFAT_IComp = -1;        //Current to Comparator
    
    int iVFAT_MSPL = -1;         //Monostable Pulse Length
    
    int iVFAT_Latency = -1;      //VFAT Latency
    
    float fVFAT_Thresh = -1;     //VFAT Threshold in fC
    
    //TDC Info
    int iTDC_CH_Number = -1;
    
    float fTDC_Histo_Mean = -1;
    float fTDC_Histo_RMS = -1;
    float fTDC_Histo_nPks = -1;
    
    float fTDC_Histo_PkInt_1stMax = -1;
    float fTDC_Histo_PkInt_2ndMax = -1;
    float fTDC_Histo_PkInt_3rdMax = -1;
    
    float fTDC_Histo_PkPos_1stMax = -1;
    float fTDC_Histo_PkPos_2ndMax = -1;
    float fTDC_Histo_PkPos_3rdMax = -1;
    
    float fTDC_Fit_Amp = -1;
    float fTDC_Fit_Amp_Err = -1;
    float fTDC_Fit_Mean = -1;
    float fTDC_Fit_Mean_Err = -1;
    float fTDC_Fit_Sigma = -1;
    float fTDC_Fit_Sigma_Err = -1;
    float fTDC_Fit_Chi2 = -1;
    float fTDC_Fit_NDF = -1;
    
    float fTDC_Fit_Convo_Amp = -1;
    float fTDC_Fit_Convo_Amp_Err = -1;
    float fTDC_Fit_Convo_Mean = -1;
    float fTDC_Fit_Convo_Mean_Err = -1;
    float fTDC_Fit_Convo_Sigma = -1;
    float fTDC_Fit_Convo_Sigma_Err = -1;
    float fTDC_Fit_Convo_Chi2 = -1;
    float fTDC_Fit_Convo_NDF = -1;
    
    float fTDC_NumDeconvo_TimeResp = -1;
    
    TF1 *func_Gaus = nullptr;
    TF1 *func_Convo = nullptr;
    
    TH1F *hTDC_Histo = nullptr;
};

//Container Class; All values are initialized to "-1"
class TRunParameters : public TObject {
    
public:
    TRunParameters();
    
    //Getters===============================================================
    virtual Run getRun(){ return run;};
    
    //Run Methods - These are methods 1 to 100
    virtual int getRunNumber(){ return run.iRun;};
    
    virtual int getBeam(){ return run.iBeam_Type;};
    
    virtual int getTrigMode(){ return run.iTrig_Mode;};
    virtual float getTrigDelay(){ return run.fTrig_Delay;};

        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //Detector Methods - These are methods 101 to 200
    virtual int getDetPos(){ return run.iDet_Pos;};
    virtual int getDetEta(){ return run.iDet_Eta;};
    virtual int getDetPhi(){ return run.iDet_Phi;};
    
    virtual float getDetCurrent(){ return run.fDet_Imon;};
    
    virtual float getDetDriftV(){ return run.fDet_VDrift;};
    
    virtual std::string getDetName(){return run.strDet_Name;};
    
    virtual float getDetGasFracAr(){  return run.fDet_GasFrac_Ar;};
    virtual float getDetGasFracCO2(){ return run.fDet_GasFrac_CO2;};
    virtual float getDetGasFracCF4(){ return run.fDet_GasFrac_CF4;};
    
    virtual float getDetGain(){ return run.fDet_Gain;};
    virtual float getDetGainErr(){ return run.fDet_Gain_Err;};
    virtual float getDetGainConst(){ return run.fDet_Gain_Const;};    //Gain assumed to be exp(P0+P1*x)
    virtual float getDetGainSlope(){ return run.fDet_Gain_Slope;};    //Gain assumed to be exp(P0+P1*x)
    virtual bool getDetGainIndepVarIsCurrent(){ return run.bDet_Gain_IndepVar_Imon;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //VFAT Methods - These are methods 201 to 300
    virtual int getTURBOID(){ return run.iTURBO_ID;};
    virtual int getVFATPos(){ return run.iVFAT_Pos;};
    virtual std::string getVFATID(){ return run.strVFAT_ID;};
    
    virtual int getVFATIPreAmpIn(){ return run.iVFAT_IPreAmpIn;};
    virtual int getVFATIPreAmpFeed(){ return run.iVFAT_IPreAmpFeed;};
    virtual int getVFATIPreAmpOut(){ return run.iVFAT_IPreAmpOut;};
    
    virtual int getVFATIShaper(){ return run.iVFAT_IShaper;};
    virtual int getVFATIShaperFeed(){ return run.iVFAT_IShaperFeed;};
    
    virtual int getVFATIComp(){ return run.iVFAT_IComp;};
    
    virtual int getVFATMSPL(){ return run.iVFAT_MSPL;};
    
    virtual int getVFATLatency(){ return run.iVFAT_Latency;};
    
    virtual float getVFATThresh(){ return run.fVFAT_Thresh;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //TDC Methods - These are methods 301 to 400
    virtual int getTDCChanNumber(){ return run.iTDC_CH_Number;};
    
    virtual TH1F* getTDCHisto(){ return run.hTDC_Histo;};
    
    virtual float getTDCHistoMean(){ return run.fTDC_Histo_Mean;};
    virtual float getTDCHistoRMS(){ return run.fTDC_Histo_RMS;};
    virtual int getTDCHistoPks(){ return (int) run.fTDC_Histo_nPks;};
    
    virtual float getTDCFitAmp(){ return run.fTDC_Fit_Amp; };
    virtual float getTDCFitMean(){ return run.fTDC_Fit_Mean; };
    virtual float getTDCFitSigma(){ return run.fTDC_Fit_Sigma; };
    virtual float getTDCFitChi2(){ return run.fTDC_Fit_Chi2; };
    virtual float getTDCFitNDF(){ return run.fTDC_Fit_NDF; };
    
    virtual float getTDCFitConvoAmp(){ return run.fTDC_Fit_Convo_Amp; };
    virtual float getTDCFitConvoMean(){ return run.fTDC_Fit_Convo_Mean; };
    virtual float getTDCFitConvoSigma(){ return run.fTDC_Fit_Convo_Sigma; };
    virtual float getTDCFitConvoChi2(){ return run.fTDC_Fit_Convo_Chi2; };
    virtual float getTDCFitConvoNDF(){ return run.fTDC_Fit_Convo_NDF; };
    
    virtual float getTDCNumDeconvoTimeResp(){ return run.fTDC_NumDeconvo_TimeResp; };
    
    virtual float getTDCHistoPksInt_1stMax(){ return run.fTDC_Histo_PkInt_1stMax;};
    virtual float getTDCHistoPksInt_2ndMax(){ return run.fTDC_Histo_PkInt_2ndMax;};
    virtual float getTDCHistoPksInt_3rdMax(){ return run.fTDC_Histo_PkInt_3rdMax;};
    
    virtual float getTDCHistoPksPos_1stMax(){ return run.fTDC_Histo_PkPos_1stMax;};
    virtual float getTDCHistoPksPos_2ndMax(){ return run.fTDC_Histo_PkPos_2ndMax;};
    virtual float getTDCHistoPksPos_3rdMax(){ return run.fTDC_Histo_PkPos_3rdMax;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //Action Methods
    //template<class T>
    //virtual void setParameter(T &input, int methodIdx);
    //virtual void getParameter(int &input, int methodIdx);
    //virtual void getParameter(float &input, int methodIdx);
    //virtual void getParameter(std::string &input, int methodIdx);
    
    //Misc==================================================================
    virtual void calcGain(float fDepVar);  //fDepVar variable gain formula is parameterized by
    virtual void calcGain(float fDepVar, float &fGain, float &fGain_Err);
    
    //Setters===============================================================
    virtual void setRun(Run inputRun){ run = inputRun; return; };
    
    //Run Methods - These are methods 1 to 100
    virtual void setRunNumber(int input){run.iRun = input; return;};
    virtual void setBeam(std::string input);
    
    virtual void setTrigMode(std::string input);
    virtual void setTrigDelay(float input){run.fTrig_Delay = input; return; };
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //Detector Methods - These are methods 101 to 200
    virtual void setDetPos(int input){run.iDet_Pos = input; return;};
    virtual void setDetEta(int input){run.iDet_Eta = input; return;};
    virtual void setDetPhi(int input){run.iDet_Phi = input; return;};
    
    virtual void setDetCurrent(float input){
        run.fDet_Imon = input;
        run.fDet_VDrift = input * 1e-6 * (1.125e6 + 563e3 + 438e3 + 550e3 + 875e3 + 525e3 + 625e3);
        return;
    };
    virtual void setDetDriftV(float input){run.fDet_VDrift = input; return;};
    
    virtual void setDetName(std::string input){run.strDet_Name = input; return;};
    
    virtual void setDetGasFracAr(float input){run.fDet_GasFrac_Ar = input; return;};
    virtual void setDetGasFracCO2(float input){run.fDet_GasFrac_CO2 = input; return;};
    virtual void setDetGasFracCF4(float input){run.fDet_GasFrac_CF4 = input; return;};
    
    virtual void setDetGainConst(float input){ run.fDet_Gain_Const = input; return;};
    virtual void setDetGainConstErr(float input){ run.fDet_Gain_Const_Err = input; return;};
    virtual void setDetGainSlope(float input){ run.fDet_Gain_Slope = input; return;};
    virtual void setDetGainSlopeErr(float input){ run.fDet_Gain_Slope_Err = input; return;};
    
    virtual void setDetGainIndepVarIsCurrent(bool input){ run.bDet_Gain_IndepVar_Imon = input; return;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //VFAT Methods - These are methods 201 to 300
    virtual void setTURBOID(int input){ run.iTURBO_ID = input; return;};
    virtual void setVFATPos(int input){ run.iVFAT_Pos = input; return;};
    virtual void setVFATID(std::string input){ run.strVFAT_ID = input; return;};
    
    virtual void setVFATIPreAmpIn(int input){ run.iVFAT_IPreAmpIn = input; return;};
    virtual void setVFATIPreAmpFeed(int input){ run.iVFAT_IPreAmpFeed = input; return;};
    virtual void setVFATIPreAmpOut(int input){ run.iVFAT_IPreAmpOut = input; return;};
    
    virtual void setVFATIShaper(int input){ run.iVFAT_IShaper = input; return;};
    virtual void setVFATIShaperFeed(int input){ run.iVFAT_IShaperFeed = input; return;};
    
    virtual void setVFATIComp(int input){ run.iVFAT_IComp = input; return;};
    
    virtual void setVFATMSPL(int input){ run.iVFAT_MSPL = input; return;};
    
    virtual void setVFATLatency(int input){ run.iVFAT_Latency = input; return;};
    
    virtual void setVFATThresh(int input){ run.fVFAT_Thresh = input*0.08; return;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //TDC Methods - These are methods 301 to 400
    virtual void setTDCChanNumber(int input){ run.iTDC_CH_Number = input; return; };
    
    virtual void setTDCHisto(TH1F *input){ run.hTDC_Histo = input; return; };
    virtual void setTDCHistoMean(float input){ run.fTDC_Histo_Mean = input; return;};
    virtual void setTDCHistoRMS(float input){ run.fTDC_Histo_RMS = input; return;};
    virtual void setTDCHistoPks(float input){ run.fTDC_Histo_nPks = input; return;};
    
    //Gaussian Fit
    //------------------------------------------------------
    virtual void setTDCFit(TF1 *input){ run.func_Gaus = input; return; };
    virtual void setTDCFitAmp(float input){ run.fTDC_Fit_Amp = input; return; };
    virtual void setTDCFitAmpErr(float input){ run.fTDC_Fit_Amp_Err = input; return; };
    virtual void setTDCFitMean(float input){ run.fTDC_Fit_Mean = input; return; };
    virtual void setTDCFitMeanErr(float input){ run.fTDC_Fit_Mean_Err = input; return; };
    virtual void setTDCFitSigma(float input){ run.fTDC_Fit_Sigma = input; return; };
    virtual void setTDCFitSigmaErr(float input){ run.fTDC_Fit_Sigma_Err = input; return; };
    virtual void setTDCFitChi2(float input){ run.fTDC_Fit_Chi2 = input; return; };
    virtual void setTDCFitNDF(float input){ run.fTDC_Fit_NDF = input; return; };
    
    //Continuous Convolution Fit
    //------------------------------------------------------
    virtual void setTDCFitConvo(TF1 *input){ run.func_Convo = input; return; };
    virtual void setTDCFitConvoAmp(float input){ run.fTDC_Fit_Convo_Amp = input; return; };
    virtual void setTDCFitConvoAmpErr(float input){ run.fTDC_Fit_Convo_Amp_Err = input; return; };
    virtual void setTDCFitConvoMean(float input){ run.fTDC_Fit_Convo_Mean = input; return; };
    virtual void setTDCFitConvoMeanErr(float input){ run.fTDC_Fit_Convo_Mean_Err = input; return; };
    virtual void setTDCFitConvoSigma(float input){ run.fTDC_Fit_Convo_Sigma = input; return; };
    virtual void setTDCFitConvoSigmaErr(float input){ run.fTDC_Fit_Convo_Sigma_Err = input; return; };
    virtual void setTDCFitConvoChi2(float input){ run.fTDC_Fit_Convo_Chi2 = input; return; };
    virtual void setTDCFitConvoNDF(float input){ run.fTDC_Fit_Convo_NDF = input; return; };
    
    //Numeric Convolution Model Cross-check
    //------------------------------------------------------
    virtual void setTDCNumDeconvoTimeResp(float input){ run.fTDC_NumDeconvo_TimeResp = input; return; };
    
    //Sync Mode
    //------------------------------------------------------
    virtual void setTDCHistoPksInt_1stMax(float input){ run.fTDC_Histo_PkInt_1stMax = input; return;};
    virtual void setTDCHistoPksInt_2ndMax(float input){ run.fTDC_Histo_PkInt_2ndMax = input; return;};
    virtual void setTDCHistoPksInt_3rdMax(float input){ run.fTDC_Histo_PkInt_3rdMax = input; return;};
    
    virtual void setTDCHistoPksPos_1stMax(float input){ run.fTDC_Histo_PkPos_1stMax = input; return;};
    virtual void setTDCHistoPksPos_2ndMax(float input){ run.fTDC_Histo_PkPos_2ndMax = input; return;};
    virtual void setTDCHistoPksPos_3rdMax(float input){ run.fTDC_Histo_PkPos_3rdMax = input; return;};
    
        //New Methods are added below pre-existing ones to preserve backwards compatibile numbering
    
    //Action Methods
    //template<class T>
    //virtual void setParameter(T input, int methodIdx);
    virtual void setParameter(bool input, int methodIdx);
    virtual void setParameter(int input, int methodIdx);
    virtual void setParameter(float input, int methodIdx);
    virtual void setParameter(std::string input, int methodIdx);
    
private:
    Run run;
};

#endif /* defined(____TRunParameters__) */