//Originally Authored by Stefano Colafranceschi; Modified by Brian Dorney.

#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TROOT.h"

#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

using std::ceil;
using std::cout;
using std::cin;
using std::endl;

struct AnalysisCase{
    //Generated Distributions
    TH1F *hFlat;        //Flat signal (what is used to convolute the signal, i.e. the Kernel)
    
    TH1F *hGaus_Pure;   //Generated Gaussian
    TH1F *hGaus_Convo;  //Convoluted Gaussian
    TH1F *hGaus_Deconvo;//Deconvoluted Gaussian from Convoluted Gaussian
    
    //Fit Functions
    TF1 *func_Gaus_Pure;
    TF1 *func_Gaus_Convo;
    TF1 *func_Gaus_Deconvo;
    
    //Distribution / Fit
    TH1F *hGausOverFit_Pure;    //Histogram Divided By Fit Function
    TH1F *hGausOverFit_Convo;
    TH1F *hGausOverFit_Deconvo;
};

struct AllCase{
    AnalysisCase Num;   //Case: Numeric
    AnalysisCase Cont;  //Case: Continuous
    AnalysisCase Disc;  //Case: Discrete
};

//void timeResponseCalibration(){
int main(){
    //Variable Declaration
    //int iNEvt = 30000;      //Number of Events generated
    //int iNEvt = 1e5;    //Number of Events generated
    long iNEvt = 1e6;   //Number of Events generated
    
    int iNBins = 1000;  //Number of bins in histograms
    int iNPoints=100;   //Defines step size (sigma_max-sigma_min)/npoints = step size in ns.
    //int iNPoints=40;   //Defines step size (sigma_max-sigma_min)/npoints = step size in ns.
    
    bool bPause4NextIter = true;
    
    float fWalk = 12.;      //Time walk of VFAT2 (for 1.2->10 fC signal @ T=1 fC)
    float fClkLength = 25.; //Length of VFAT2 clock cycle
    float fClkLengthOver2 = fClkLength / 2.;    //Half the clock cycle length
    float fTimeMin = -100;  //This w/fTimeMax defines the time span in nanoseconds
    float fTimeMax = 100;   //See above
    float fDeltaT = (fTimeMax - fTimeMin ) / iNBins;    //Temporal step size in nanoseconds.
    
    float f1stNonzeroSignalTime = 1e9;
    
    float fSigma;           //Simulated time response
    float fSigma_min=1;		//Range of simulated time response (low)
    float fSigma_max=20;	//Range of simulated time response (high)
    
    float fStepSize=(fSigma_max-fSigma_min)/iNPoints;   //Time response step size in nanoseconds
    
    std::string empty = ""; //For clearing string stream below
    
    Double_t *dSimSigma = new Double_t[iNPoints];   //Stored Sigma's
    
    Double_t *dNum_GausPure         = new Double_t[iNPoints];		//Gaussian time response of detector before entering electronics (i.e. not convoluted)
    Double_t *dNum_GausPure_Err     = new Double_t[iNPoints];
    Double_t *dNum_GausPure_normChi2= new Double_t[iNPoints];
    
    Double_t *dNum_GausConvo        = new Double_t[iNPoints];	//Convoluted data by hardware
    Double_t *dNum_GausConvo_Err    = new Double_t[iNPoints];
    Double_t *dNum_GausConvo_normChi2=new Double_t[iNPoints];
    
    Double_t *dCont_GausPure        = new Double_t[iNPoints];
    Double_t *dCont_GausPure_Err    = new Double_t[iNPoints];
    Double_t *dCont_GausPure_normChi2=new Double_t[iNPoints];
    
    Double_t *dDisc_GausPure        = new Double_t[iNPoints];
    Double_t *dDisc_GausPure_Err    = new Double_t[iNPoints];
    Double_t *dDisc_GausPure_normChi2=new Double_t[iNPoints];
    
    Double_t *dDisc_GausConvo       = new Double_t[iNPoints];
    Double_t *dDisc_GausConvo_Err   = new Double_t[iNPoints];
    Double_t *dDisc_GausConvo_normChi2=new Double_t[iNPoints];
    
    TRandom3 rndm(0);       //Random number generator
    
    std::vector<float> vec_Disc_GausPure;
    std::vector<float> vec_Disc_Flat;
    
    AllCase allAnalysisCase[100];   //Should be iNPoints but dynamic allocation not allowed.
    
    //Continous Convolution Functional Form (amplitude and mean terms neglected)
    //TF1 *func_ContConvo = new TF1("func_ContConvo","[0]*sqrt(TMath::Pi()/2.)*(TMath::Erf( ([1] + x) / ([0] * sqrt(2.) ) ) - TMath::Erf( (x - [1]) / ([0] * sqrt(2.) ) ) )",fTimeMin,fTimeMax);
    //func_ContConvo->SetParameter(1,fClkLengthOver2);
    //func_ContConvo->SetNpx(iNBins);
    
    //Output File
    TFile *outputFile = new TFile("TimeRespCal_RMS.root","RECREATE");
    
    TDirectory *dir_Sim = outputFile->mkdir("Simulation");
        TDirectory *dir_Steps = dir_Sim->mkdir("Steps");

    //Set sigma to the initial time response of the detector
    fSigma=fSigma_min;
    
    //Set Canvases
    //TCanvas *cCase_Numeric = new TCanvas("cCase_Numeric","Case: Numeric Pseudo Deconvolution (i.e. Rndm Gen.)",700,700);
    //TCanvas *cCase_ContDeconvo = new TCanvas("cCase_ContDeconvo","Case: Continuous Deconvolution", 700, 700);
    //TCanvas *cCase_DiscDeconvo = new TCanvas("cCase_DiscDeconvo","Case: Discrete Deconvolution", 700, 700);
    
    //TCanvas *cEvtClkCycle = new TCanvas("cEvtClkCycle","Clock Cycle", 700,700);
    
    //Setup the Response Function created by the clock
    TH1F *hDiscFlatClock = new TH1F( "hDiscConvo_FlatRndm", "",iNBins,fTimeMin,fTimeMax );
    
    //cout<<"i\tTime\tVal\n";
    //for (int i=0; (i * fDeltaT) < fClkLength; ++i) {
    for (int i=0; i<hDiscFlatClock->GetNbinsX(); ++i) {
        if ( 0. <= hDiscFlatClock->GetBinLowEdge(i) && hDiscFlatClock->GetBinLowEdge(i) < fClkLength ) { //Case: In Window
            vec_Disc_Flat.push_back(1);
            
            //hDiscFlatClock->Fill(i * fDeltaT);    //Round-off error when comparing to bin edges causes some bins to be assigned 0, and others to be assigned 2...not what we want
            hDiscFlatClock->SetBinContent(i,1.);
        } //End Case: In Window
    } //End
    
    dir_Sim->cd();
    hDiscFlatClock->SetLineColor(kBlue+2);
    hDiscFlatClock->Write();
    
    //Loop From fSigma_min to fSigma_max
    //cout << "Sigma\tGaus_Corr\tConvo_Gaus_Corr\n";
    for (int j=0; j < iNPoints; ++j) { //Loop from fSigma_min to fSigma_max
        //Clear Discrete Convolution Vectors (NOT THE CLOCK VECTOR)
        vec_Disc_GausPure.clear();
        //vec_discConvo_Time.clear();
        
        //Set The Current Detector Time Response
        fSigma = fSigma_min + j*fStepSize;
        
        std::stringstream sstream;
        std::string strSigma, strIdx, strName;
        
        sstream<<j;
        strIdx = sstream.str();
        
        sstream.str(empty);
        
        sstream<<fSigma;
        strSigma = sstream.str();
        
        //Initialize Objects - General
        TH1F *hEvtPerClkCyle = new TH1F(Form("hEvtPerClkCyle_%s",strIdx.c_str() ),Form("#sigma_{pure} = %s", strSigma.c_str() ), 7, fTimeMin+fClkLengthOver2, fTimeMax-fClkLengthOver2);
        
        //Initialize Histograms - Numeric Case
        allAnalysisCase[j].Num.hFlat = new TH1F(Form("hNum_Flat_%s",strIdx.c_str() ),"",iNBins,fTimeMin,fTimeMax);
        allAnalysisCase[j].Num.hGaus_Pure = new TH1F(Form("hNum_GausPure_%s",strIdx.c_str() ),Form("#sigma_{pure} = %s", strSigma.c_str() ),iNBins,fTimeMin,fTimeMax);
        allAnalysisCase[j].Num.hGaus_Convo = new TH1F(Form("hNum_GausConvo_%s",strIdx.c_str() ),Form("#sigma_{pure} = %s", strSigma.c_str() ),iNBins,fTimeMin,fTimeMax);
        
        allAnalysisCase[j].Num.func_Gaus_Pure = new TF1( ( "func_NumGausPure_" + strIdx ).c_str(), "[0]*exp(-0.5*((x-[1])/[2])**2)", fTimeMin, fTimeMax);
        allAnalysisCase[j].Num.func_Gaus_Convo= new TF1( ( "func_NumGausConvo_" + strIdx ).c_str(), "[0]*exp(-0.5*((x-[1])/[2])**2)", fTimeMin, fTimeMax);
        
        //Initialize Objects - Continous
        //allAnalysisCase[j].Cont.func_Gaus_Convo = new TF1( ( "func_ContGausConvo_" + strIdx).c_str(),"[2]*[0]*sqrt(TMath::Pi()/2.)*(TMath::Erf( ([1] + (x-[3])) / ([0] * sqrt(2.) ) ) - TMath::Erf( ((x-[3]) - [1]) / ([0] * sqrt(2.) ) ) )",fTimeMin,fTimeMax );
        allAnalysisCase[j].Cont.func_Gaus_Convo = new TF1( ( "func_ContGausConvo_" + strIdx).c_str(),"[2]*[0]*sqrt(TMath::Pi()/2.)*(TMath::Erf( (12.5 + (x-[3])) / ([0] * sqrt(2.) ) ) - TMath::Erf( ((x-[3]) - 12.5) / ([0] * sqrt(2.) ) ) )",fTimeMin,fTimeMax );
        
        //Initialize Objects - Discrete
        allAnalysisCase[j].Disc.hGaus_Pure = new TH1F(Form("hDisc_GausPure_%s",strIdx.c_str() ),Form("#sigma_{pure} = %s", strSigma.c_str() ),iNBins,fTimeMin,fTimeMax);
        allAnalysisCase[j].Disc.hGaus_Convo = new TH1F(Form("hDisc_GausConvo_%s",strIdx.c_str() ),Form("#sigma_{pure} = %s", strSigma.c_str() ),iNBins,fTimeMin,fTimeMax);
        
        allAnalysisCase[j].Disc.func_Gaus_Pure = new TF1( ( "func_DiscGausPure_" + strIdx ).c_str(), "[0]*exp(-0.5*((x-[1])/[2])**2)", fTimeMin, fTimeMax);
        allAnalysisCase[j].Disc.func_Gaus_Convo= new TF1( ( "func_DiscGausConvo_" + strIdx ).c_str(), "[0]*exp(-0.5*((x-[1])/[2])**2)", fTimeMin, fTimeMax);
        
        //Set Errors
        allAnalysisCase[j].Num.hFlat->Sumw2();
        allAnalysisCase[j].Num.hGaus_Pure->Sumw2();
        allAnalysisCase[j].Num.hGaus_Convo->Sumw2();
        
        allAnalysisCase[j].Disc.hGaus_Pure->Sumw2();
        allAnalysisCase[j].Disc.hGaus_Convo->Sumw2();
        
        /*if (j == 1) {
         allAnalysisCase[j].Num.hFlat->Sumw2();
         allAnalysisCase[j].Num.hGaus_Pure->Sumw2();
         allAnalysisCase[j].Num.hGaus_Convo->Sumw2();
         
         allAnalysisCase[j].Disc.hGaus_Pure->Sumw2();
         allAnalysisCase[j].Disc.hGaus_Convo->Sumw2();
         }*/
        
        //Set Num Points
        allAnalysisCase[j].Num.func_Gaus_Pure->SetNpx(iNBins*10);
        allAnalysisCase[j].Num.func_Gaus_Convo->SetNpx(iNBins*10);
        allAnalysisCase[j].Cont.func_Gaus_Convo->SetNpx(iNBins*10);
        allAnalysisCase[j].Disc.func_Gaus_Pure->SetNpx(iNBins*10);
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetNpx(iNBins*10);
        
        //Set Style
        allAnalysisCase[j].Num.hFlat->SetLineColor(kBlue);
        allAnalysisCase[j].Num.hGaus_Pure->SetLineColor(kRed);
        
        allAnalysisCase[j].Disc.hGaus_Pure->SetLineColor(kRed+2);
        
        //Event Loop - Numeric Case
        for (int i = 0 ; i < iNEvt ; i++) {
            float fTime = rndm.Gaus(0.,fSigma);
            float fTimeClock = rndm.Uniform(0.,fClkLength); //Distance from next leading clock edge
            
            //Fill Histograms
            hEvtPerClkCyle->Fill( fTime + fWalk );
            allAnalysisCase[j].Num.hFlat->Fill( fTimeClock );
            
            allAnalysisCase[j].Num.hGaus_Pure->Fill( fTime );
            allAnalysisCase[j].Num.hGaus_Convo->Fill( fTime + fTimeClock );
        } //End Event Loop - Numeric Case
        
        //Initial Guess & Fitting - Numeric Case
        allAnalysisCase[j].Num.func_Gaus_Pure->SetParameter(0, allAnalysisCase[j].Num.hGaus_Pure->GetMaximum() );
        allAnalysisCase[j].Num.func_Gaus_Pure->SetParameter(1, allAnalysisCase[j].Num.hGaus_Pure->GetMean() );
        allAnalysisCase[j].Num.func_Gaus_Pure->SetParameter(2, allAnalysisCase[j].Num.hGaus_Pure->GetRMS() );    //I could also try fSigma
        
        cout<<"==================================================================================\n";
        cout<<"Numeric Case: Fitting Pure Gaussian!\n";
        allAnalysisCase[j].Num.hGausOverFit_Pure = (TH1F*)allAnalysisCase[j].Num.hGaus_Pure->Clone(Form("hNum_GausOverFit_Pure_%s",strIdx.c_str() ) );
        
        allAnalysisCase[j].Num.hGaus_Pure->Fit(allAnalysisCase[j].Num.func_Gaus_Pure);
        //TH1F *temp = (TH1F*) allAnalysisCase[j].Num.hGaus_Pure->Clone( "temp" );
        //temp->Fit(allAnalysisCase[j].Num.func_Gaus_Pure);
        
        allAnalysisCase[j].Num.hGausOverFit_Pure->Divide(allAnalysisCase[j].Num.func_Gaus_Pure);
        cout<<"==================================================================================\n";
        
        allAnalysisCase[j].Num.func_Gaus_Convo->SetParameter(0, allAnalysisCase[j].Num.hGaus_Convo->GetMaximum() );
        allAnalysisCase[j].Num.func_Gaus_Convo->SetParameter(1, allAnalysisCase[j].Num.hGaus_Convo->GetMean() );
        allAnalysisCase[j].Num.func_Gaus_Convo->SetParameter(2, allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );    //I could also try fSigma
        
        cout<<"==================================================================================\n";
        cout<<"Numeric Case: Fitting Convoluted Gaussian!\n";
        allAnalysisCase[j].Num.hGausOverFit_Convo = (TH1F*)allAnalysisCase[j].Num.hGaus_Convo->Clone(Form("hNum_GausOverFit_Convo_%s",strIdx.c_str() ) );
        allAnalysisCase[j].Cont.hGausOverFit_Convo = (TH1F*)allAnalysisCase[j].Num.hGaus_Convo->Clone(Form("hCont_GausOverFit_Convo_%s",strIdx.c_str() ) ); //Place this Here Before Fitting So it doesn't have the Gaussian Function of the fit below overlaid when the continous case is actually fit
        
        allAnalysisCase[j].Num.hGaus_Convo->Fit(allAnalysisCase[j].Num.func_Gaus_Convo);
        //temp = (TH1F*) allAnalysisCase[j].Num.hGaus_Convo->Clone( "temp" );
        //temp->Fit(allAnalysisCase[j].Num.func_Gaus_Convo);
        
        allAnalysisCase[j].Num.hGausOverFit_Convo->Divide(allAnalysisCase[j].Num.func_Gaus_Convo);
        cout<<"==================================================================================\n";
        
        //Initial Guess & Fitting - Continuous Case
        allAnalysisCase[j].Cont.func_Gaus_Convo->SetParameter(0, allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );
        //allAnalysisCase[j].Cont.func_Gaus_Convo->SetParameter(1, fClkLengthOver2);
        allAnalysisCase[j].Cont.func_Gaus_Convo->SetParameter(2, allAnalysisCase[j].Num.hGaus_Convo->GetMaximum() );
        allAnalysisCase[j].Cont.func_Gaus_Convo->SetParameter(3, allAnalysisCase[j].Num.hGaus_Convo->GetMean() );
        
        //allAnalysisCase[j].Cont.func_Gaus_Convo->SetParLimits(0, 0, 100 * allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );
        
        cout<<"==================================================================================\n";
        cout<<"Continuous Case: Fitting Convoluted Gaussian!\n";
        
        //allAnalysisCase[j].Num.hGaus_Convo->Fit(allAnalysisCase[j].Cont.func_Gaus_Convo);
        //allAnalysisCase[j].Num.hGaus_Convo->Fit(allAnalysisCase[j].Cont.func_Gaus_Convo,"","",allAnalysisCase[j].Num.hGaus_Convo->GetMean() - 2. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS(), allAnalysisCase[j].Num.hGaus_Convo->GetMean() + 2. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );
        //allAnalysisCase[j].Num.hGaus_Convo->Fit(allAnalysisCase[j].Cont.func_Gaus_Convo,"","",allAnalysisCase[j].Num.hGaus_Convo->GetMean() - 3. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS(), allAnalysisCase[j].Num.hGaus_Convo->GetMean() + 3. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );
        allAnalysisCase[j].Num.hGaus_Convo->Fit(allAnalysisCase[j].Cont.func_Gaus_Convo,"","",allAnalysisCase[j].Num.hGaus_Convo->GetMean() - 4. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS(), allAnalysisCase[j].Num.hGaus_Convo->GetMean() + 4. * allAnalysisCase[j].Num.hGaus_Convo->GetRMS() );
        //temp = (TH1F*) allAnalysisCase[j].Num.hGaus_Convo->Clone( "temp" ); //Already This way but why not just refresh...
        //temp->Fit(allAnalysisCase[j].Cont.func_Gaus_Convo);
        
        allAnalysisCase[j].Cont.hGausOverFit_Convo->Divide(allAnalysisCase[j].Cont.func_Gaus_Convo);
        cout<<"==================================================================================\n";
        
        //Discrete Convolution
        //Setup Gaussian to be convoluted in a Loop
        //Setup the Gaussian from the Fit that is performed to the Numeric Case Data
        /*for (int i=0; i<iNBins; ++i) { //Loop Over Time Bins
            if (allAnalysisCase[j].Num.func_Gaus_Pure->Eval( fTimeMin + (i * fDeltaT) ) > 0){ //Check if Signal Amplitude is non-zero
                vec_Disc_GausPure.push_back( allAnalysisCase[j].Num.func_Gaus_Pure->Eval( fTimeMin + (i * fDeltaT) ) );
                
                if (f1stNonzeroSignalTime > (fTimeMin + (i * fDeltaT) ) ) { //Check if this is the first time this has happened
                    f1stNonzeroSignalTime = fTimeMin + (i * fDeltaT);
                } //End Check if this is the first time this has happened
                
                allAnalysisCase[j].Disc.hGaus_Pure->SetBinContent( i+1, allAnalysisCase[j].Num.func_Gaus_Pure->Eval( fTimeMin + (i * fDeltaT) ) );
                allAnalysisCase[j].Disc.hGaus_Pure->SetBinError( i+1, sqrt(allAnalysisCase[j].Num.func_Gaus_Pure->Eval( fTimeMin + (i * fDeltaT) ) ) );
            } //End Check if Signal Amplitude is non-zero
        } //Loop Over Time Bins
        */
        
        //Setup Gaussian to be convoluted in a Loop
        //Use the histogram that was created from the Numeric Case Data
        bool b1stNonZeroFound = false;
        for (int i=1; i<=allAnalysisCase[j].Num.hGaus_Pure->GetNbinsX() ; ++i) {
            //Do this for all Bins
            vec_Disc_GausPure.push_back( allAnalysisCase[j].Num.hGaus_Pure->GetBinContent(i) );
            
            //Check if this is the 1st non-zero entry
            if (!b1stNonZeroFound && allAnalysisCase[j].Num.hGaus_Pure->GetBinContent(i) > 0 && f1stNonzeroSignalTime > allAnalysisCase[j].Num.hGaus_Pure->GetBinLowEdge(i) ) { //Case: First non-zero entry
                f1stNonzeroSignalTime = allAnalysisCase[j].Num.hGaus_Pure->GetBinLowEdge(i);
                b1stNonZeroFound = true;
            } //End Case: First non-zero entry
            
            allAnalysisCase[j].Disc.hGaus_Pure->SetBinContent( i, allAnalysisCase[j].Num.hGaus_Pure->GetBinContent(i) );
            allAnalysisCase[j].Disc.hGaus_Pure->SetBinError( i, allAnalysisCase[j].Num.hGaus_Pure->GetBinError(i) );
        }
        
        //Remove all non-zero entries Outside the Data Signal, Starting from the beginning
        std::vector<float>::iterator iterLastZero = vec_Disc_GausPure.end();      //Set this to the End
        for (auto iter = vec_Disc_GausPure.begin(); iter != vec_Disc_GausPure.end(); ++iter) { //Loop over vec_Disc_GausPure
            if ( (*iter) > 0 ) { //End Case: Current Member is Zero
                //iterLastZero = --iter;
                iterLastZero = iter;    //Because Erase removes all iterators up to the last iterator
                break;
            } //End Case: Current Member is Zero
        } //End Loop over vec_Disc_GausPure
        
        //Debugging
        //cout<<"vec_Disc_GausPure.size() = " << vec_Disc_GausPure.size() << endl;
        //cout<<"Position of iterLastZero = " << std::distance(vec_Disc_GausPure.begin(), iterLastZero) << endl;
        
        vec_Disc_GausPure.erase( vec_Disc_GausPure.begin(), iterLastZero);
        
        //Debugging
        //cout<<"vec_Disc_GausPure.size() = " << vec_Disc_GausPure.size() << endl;
        
        //Remove all non-zero entries Outside the Data Signal, Starting from the End
        iterLastZero = vec_Disc_GausPure.begin();   //Set this to the Beginning
        for (auto iter = --(vec_Disc_GausPure.end()); iter != vec_Disc_GausPure.begin(); --iter) { //Loop over vec_Disc_GausPure
            if ( (*iter) > 0 ) { //End Case: Current Member is Zero
                iterLastZero = ++iter;
                break;
            } //End Case: Current Member is Zero
        } //End Loop over vec_Disc_GausPure
        
        //Debugging
        //cout<<"vec_Disc_GausPure.size() = " << vec_Disc_GausPure.size() << endl;
        //cout<<"Position of iterLastZero = " << std::distance(vec_Disc_GausPure.begin(), iterLastZero) << endl;
        
        vec_Disc_GausPure.erase( iterLastZero, vec_Disc_GausPure.end() );
        
        //Debugging
        //cout<<"vec_Disc_GausPure.size() = " << vec_Disc_GausPure.size() << endl;
        
        //Debugging
        //cout<<"f1stNonzeroSignalTime = " << f1stNonzeroSignalTime << endl;
        
        //Debugging
        /*cout<<"Pure Histogram"<<endl;
        cout<<"============================================\n";
        cout<<"i\tTime\tVal\n";
        for (int i=1; i<allAnalysisCase[j].Disc.hGaus_Pure->GetNbinsX(); ++i) {
            cout<<i<<"\t"<<allAnalysisCase[j].Disc.hGaus_Pure->GetBinLowEdge(i)<<"\t"<<allAnalysisCase[j].Disc.hGaus_Pure->GetBinContent(i)<<endl;
        }*/
        
        //Debugging
        /*cout<<"Pure Vector"<<endl;
        cout<<"============================================\n";
        cout<<"i\tVal\n";
        for (int i=0; i<vec_Disc_GausPure.size(); ++i) {
            cout<<i<<"\t"<<vec_Disc_GausPure[i]<<endl;
        }*/
        
        //Initialize Discrete Convolution Container
        std::vector<float> vec_Disc_GausConvo(vec_Disc_GausPure.size() + vec_Disc_Flat.size() - 1);
        
        //cout<<"============================================\n";
        //cout<<"i\tTime\tConvolution\n";
        for (int i=0; i < vec_Disc_GausConvo.size(); ++i) { //Loop Over Convolution Product
            size_t kmin, kmax, k;
            
            vec_Disc_GausConvo[i] = 0; //Initialize Convolution value to 0
            
            //Setup range of convolution product
            kmin = (i >= vec_Disc_Flat.size() -1 ) ? i - (vec_Disc_Flat.size() -1) : 0;
            kmax = (i < vec_Disc_GausPure.size() -1 ) ? i : vec_Disc_GausPure.size() - 1;
            
            //Debugging
            //cout<<vec_discConvo_Time[0]+i*fDeltaT<<"\ty["<<i<<"] = ";
            
            //Debugging
            //cout<<"k\ti-k\t\tGaus\tFlat\n";
            /*for (k = kmin; k <= kmax; ++k) {
                cout<<k<<"\t"<<i-k<<"\t"<<vec_Disc_GausPure[k]<<"\t"<<vec_Disc_Flat[i-k]<<endl;
            }*/
            
            for (k = kmin; k <= kmax ; ++k) { //Loop: Pass Kernel (Gate) through Signal
                vec_Disc_GausConvo[i] += vec_Disc_GausPure[k] * vec_Disc_Flat[i-k];
                
                //Debugging
                /*if (k == kmax) {
                    cout<<"x["<<k<<"]*h["<<i-k<<"] = " << vec_Disc_GausConvo[i] << endl;
                }
                else{
                    cout<<"x["<<k<<"]*h["<<i-k<<"] + ";
                }*/
            } //End Loop: Pass Kernel (Gate) through Signal
            
            allAnalysisCase[j].Disc.hGaus_Convo->Fill( f1stNonzeroSignalTime + ( (i+0.5) * fDeltaT), vec_Disc_GausConvo[i] );
            
        } //End Loop Over Convolution Product
        
        //Debugging
        /*cout<<"Convoluted Histogram"<<endl;
        cout<<"============================================\n";
        cout<<"i\tTime\tVal\n";
        for (int i=1; i<allAnalysisCase[j].Disc.hGaus_Convo->GetNbinsX(); ++i) {
            cout<<i<<"\t"<<allAnalysisCase[j].Disc.hGaus_Convo->GetBinLowEdge(i)<<"\t"<<allAnalysisCase[j].Disc.hGaus_Convo->GetBinContent(i)<<endl;
        }*/
        
        //Debugging
        /*cout<<"Convoluted Vector"<<endl;
        cout<<"============================================\n";
        cout<<"i\tVal\n";
        for (int i=0; i<vec_Disc_GausConvo.size(); ++i) {
            cout<<i<<"\t"<<vec_Disc_GausConvo[i]<<endl;
        }*/
        
        //Set Error Bins on allAnalysisCase[j].Disc.hGaus_Convo
        for (int i=1; i<=allAnalysisCase[j].Disc.hGaus_Convo->GetNbinsX(); ++i) {
            allAnalysisCase[j].Disc.hGaus_Convo->SetBinError(i, sqrt(allAnalysisCase[j].Disc.hGaus_Convo->GetBinContent(i) ) );
        }
        
        //Scale allAnalysisCase[j].Disc.hGaus_Convo to have the event content as allAnalysisCase[j].Disc.hGaus_Pure
        allAnalysisCase[j].Disc.hGaus_Convo->Scale( allAnalysisCase[j].Disc.hGaus_Pure->Integral() / allAnalysisCase[j].Disc.hGaus_Convo->Integral() );
        
        //Initial Guess & Fitting - Discrete Case
        allAnalysisCase[j].Disc.func_Gaus_Pure->SetParameter(0, allAnalysisCase[j].Disc.hGaus_Pure->GetMaximum() );
        allAnalysisCase[j].Disc.func_Gaus_Pure->SetParameter(1, allAnalysisCase[j].Disc.hGaus_Pure->GetMean() );
        allAnalysisCase[j].Disc.func_Gaus_Pure->SetParameter(2, allAnalysisCase[j].Disc.hGaus_Pure->GetRMS() );    //I could also try fSigma
        
        cout<<"==================================================================================\n";
        cout<<"Discrete Case: Fitting Pure Gaussian!\n";
        allAnalysisCase[j].Disc.hGausOverFit_Pure = (TH1F*)allAnalysisCase[j].Disc.hGaus_Pure->Clone(Form("hDisc_GausOverFit_Pure_%s",strIdx.c_str() ) );
        
        allAnalysisCase[j].Disc.hGaus_Pure->Fit(allAnalysisCase[j].Disc.func_Gaus_Pure,"","",allAnalysisCase[j].Disc.hGaus_Pure->GetMean() - 2. * allAnalysisCase[j].Disc.hGaus_Pure->GetRMS(), allAnalysisCase[j].Disc.hGaus_Pure->GetMean() + 2. * allAnalysisCase[j].Disc.hGaus_Pure->GetRMS());
        //temp = (TH1F*) allAnalysisCase[j].Disc.hGaus_Pure->Clone( "temp" );
        //temp->Fit(allAnalysisCase[j].Disc.func_Gaus_Pure);
        
        allAnalysisCase[j].Disc.hGausOverFit_Pure->Divide(allAnalysisCase[j].Disc.func_Gaus_Pure);
        cout<<"==================================================================================\n";
        
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetParameter(0, allAnalysisCase[j].Disc.hGaus_Convo->GetMaximum() );
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetParameter(1, allAnalysisCase[j].Disc.hGaus_Convo->GetMean() );
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetParameter(2, allAnalysisCase[j].Disc.hGaus_Convo->GetRMS() );    //I could also try fSigma
        
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetParLimits(0,0., 100. * allAnalysisCase[j].Disc.hGaus_Convo->GetMaximum() );
        allAnalysisCase[j].Disc.func_Gaus_Convo->SetParLimits(2,0., 100. * allAnalysisCase[j].Disc.hGaus_Convo->GetRMS() );
        
        cout<<"==================================================================================\n";
        cout<<"Discrete Case: Fitting Convoluted Gaussian!\n";
        allAnalysisCase[j].Disc.hGausOverFit_Convo = (TH1F*)allAnalysisCase[j].Disc.hGaus_Convo->Clone(Form("hDisc_GausOverFit_Convo_%s",strIdx.c_str() ) );
        
        allAnalysisCase[j].Disc.hGaus_Convo->Fit(allAnalysisCase[j].Disc.func_Gaus_Convo,"","",allAnalysisCase[j].Disc.hGaus_Convo->GetMean() - 2. * allAnalysisCase[j].Disc.hGaus_Convo->GetRMS(), allAnalysisCase[j].Disc.hGaus_Convo->GetMean() + 2. * allAnalysisCase[j].Disc.hGaus_Convo->GetRMS());
        //temp = (TH1F*) allAnalysisCase[j].Disc.hGaus_Convo->Clone( "temp" );
        //temp->Fit(allAnalysisCase[j].Disc.func_Gaus_Convo);
        
        allAnalysisCase[j].Disc.hGausOverFit_Convo->Divide(allAnalysisCase[j].Disc.func_Gaus_Convo);
        cout<<"==================================================================================\n";
        
        //Set Windows On Discrete Case
        allAnalysisCase[j].Disc.hGaus_Pure->GetXaxis()->SetRangeUser(allAnalysisCase[j].Disc.hGaus_Pure->GetMean() - 3 * allAnalysisCase[j].Disc.hGaus_Pure->GetRMS(), allAnalysisCase[j].Disc.hGaus_Pure->GetMean() + 3 * allAnalysisCase[j].Disc.hGaus_Pure->GetRMS() );
        allAnalysisCase[j].Disc.hGaus_Convo->GetXaxis()->SetRangeUser(allAnalysisCase[j].Disc.hGaus_Convo->GetMean() - 3 * allAnalysisCase[j].Disc.hGaus_Convo->GetRMS(), allAnalysisCase[j].Disc.hGaus_Convo->GetMean() + 3 * allAnalysisCase[j].Disc.hGaus_Convo->GetRMS() );
        
        //Store the Numeric Case
        //dNum_GausPure[j]  = fSigma;
        //dNum_GausConvo[j] = allAnalysisCase[j].Num.hGaus_Convo->GetRMS();
        dSimSigma[j]                  = fSigma;
        
        //dNum_GausPure[j]              = allAnalysisCase[j].Num.func_Gaus_Pure->GetParameter(2);
        //dNum_GausPure_Err[j]          = allAnalysisCase[j].Num.func_Gaus_Pure->GetParError(2);
        dNum_GausPure[j]              = allAnalysisCase[j].Num.hGaus_Pure->GetRMS(1);   //Sigma
        dNum_GausPure_Err[j]          = allAnalysisCase[j].Num.hGaus_Pure->GetRMS(11);  //Err_Sigma, see root ref guide
        dNum_GausPure_normChi2[j]     = allAnalysisCase[j].Num.func_Gaus_Pure->GetChisquare() / allAnalysisCase[j].Num.func_Gaus_Pure->GetNDF();
        
        //dNum_GausConvo[j]              = allAnalysisCase[j].Num.func_Gaus_Convo->GetParameter(2);
        //dNum_GausConvo_Err[j]          = allAnalysisCase[j].Num.func_Gaus_Convo->GetParError(2);
        dNum_GausConvo[j]              = allAnalysisCase[j].Num.hGaus_Convo->GetRMS(1);     //Sigma
        dNum_GausConvo_Err[j]          = allAnalysisCase[j].Num.hGaus_Convo->GetRMS(11);    //Err_Sigma, see root ref guide
        dNum_GausConvo_normChi2[j]     = allAnalysisCase[j].Num.func_Gaus_Convo->GetChisquare() / allAnalysisCase[j].Num.func_Gaus_Pure->GetNDF();
        
        dCont_GausPure[j]              = allAnalysisCase[j].Cont.func_Gaus_Convo->GetParameter(0);
        dCont_GausPure_Err[j]          = allAnalysisCase[j].Cont.func_Gaus_Convo->GetParError(0);
        dCont_GausPure_normChi2[j]     = allAnalysisCase[j].Cont.func_Gaus_Convo->GetChisquare() / allAnalysisCase[j].Cont.func_Gaus_Convo->GetNDF(); //Okay here the array is for the "PURE" case b/c it is getting the sigma_pure, but this comes from the Convo function (recall the math!!!)
        
        //dDisc_GausPure[j]              = allAnalysisCase[j].Disc.func_Gaus_Pure->GetParameter(2);
        //dDisc_GausPure_Err[j]          = allAnalysisCase[j].Disc.func_Gaus_Pure->GetParError(2);
        dDisc_GausPure[j]              = allAnalysisCase[j].Disc.hGaus_Pure->GetRMS(1);     //Sigma
        dDisc_GausPure_Err[j]          = allAnalysisCase[j].Disc.hGaus_Pure->GetRMS(11);    //Err_Sigma, see root ref guide
        dDisc_GausPure_normChi2[j]     = allAnalysisCase[j].Disc.func_Gaus_Pure->GetChisquare() / allAnalysisCase[j].Disc.func_Gaus_Pure->GetNDF();
        
        //dDisc_GausConvo[j]              = allAnalysisCase[j].Disc.func_Gaus_Convo->GetParameter(2);
        //dDisc_GausConvo_Err[j]          = allAnalysisCase[j].Disc.func_Gaus_Convo->GetParError(2);
        dDisc_GausConvo[j]              = allAnalysisCase[j].Disc.hGaus_Convo->GetRMS(1);   //Sigma
        dDisc_GausConvo_Err[j]          = allAnalysisCase[j].Disc.hGaus_Convo->GetRMS(11);  //Err_Sigma, see root ref guide
        dDisc_GausConvo_normChi2[j]     = allAnalysisCase[j].Disc.func_Gaus_Convo->GetChisquare() / allAnalysisCase[j].Disc.func_Gaus_Pure->GetNDF();
        
        //Debugging
        /*cout<<"time\tGaus\tConvoGaus\n";
         for (int i=0; i < vec_Disc_GausConvo.size(); i++) {
         if (i<vec_Disc_GausPure.size() ) {
         cout<<vec_discConvo_Time[0]+i*fDeltaT<<"\t"<<vec_Disc_GausPure[i]<<"\t"<<vec_Disc_GausConvo[i]<<endl;
         }
         else{
         cout<<vec_discConvo_Time[0]+i*fDeltaT<<"\t"<<0.<<"\t"<<vec_Disc_GausConvo[i]<<endl;
         }
         }*/
        
        //Setup a TLegend
        /*TLegend *leg_timeRespCal = new TLegend(0.3,0.7,0.5,0.9);
         leg_timeRespCal->SetFillColor(kWhite);
         leg_timeRespCal->SetLineColor(kWhite);
         leg_timeRespCal->AddEntry(allAnalysisCase[j].Num.hGaus_Pure,"Input Signal","LP");
         leg_timeRespCal->AddEntry(allAnalysisCase[j].Num.hFlat, "Gate" ,"LP" );
         leg_timeRespCal->AddEntry(allAnalysisCase[j].Num.hGaus_Convo, "Convoluted Output","LP");*/
        
        //Draw the Numeric Case
        /*cCase_Numeric->cd();
         allAnalysisCase[j].Num.hGaus_Pure->SetTitle( Form("#sigma_{Direct} = %s", strSigma.c_str() ) );
         allAnalysisCase[j].Num.hFlat->Draw("hist");
         allAnalysisCase[j].Num.hGaus_Pure->Draw("same");
         allAnalysisCase[j].Num.hGaus_Convo->Draw("same");
         leg_timeRespCal->Draw("same");*/
        
        //Draw the Continuous Convolution Case
        /*cCase_ContDeconvo->cd();
         allAnalysisCase[j].Num.hGaus_Convo->SetTitle( Form("#sigma_{Direct} = %s", strSigma.c_str() ) );
         allAnalysisCase[j].Num.hGaus_Convo->Draw();
         allAnalysisCase[j].Cont.func_Gaus_Convo->Draw("same");*/
        
        //Draw the Discrete Convolution Case
        /*cCase_DiscDeconvo->cd();
         allAnalysisCase[j].Disc.hGaus_Pure->SetTitle( Form("#sigma_{Direct} = %s", strSigma.c_str() ) );
         allAnalysisCase[j].Disc.hGaus_Pure->Draw();
         allAnalysisCase[j].Disc.hGaus_Convo->Draw("same");*/
        
        //cEvtClkCycle->cd();
        //hEvtPerClkCyle->Draw();
        
        //Output To user
        //cout << fSigma << "\t" << dNum_GausPure[j] << "\t" << dNum_GausConvo[j] << endl;
        
        //cout<<"No Fault - 1\n";
        
        //Write
        //TDirectory *dir_CurrStep = dir_Sim->mkdir( ("Step_" + strIdx).c_str() );
        TDirectory *dir_CurrStep    = dir_Steps->mkdir( ("Step_" + strIdx).c_str() );
        
        //dir_CurrStep->cd();
        
        //cout<<"No Fault - 2\n";
        
        //Case: Numeric
        TDirectory *dir_Numeric = dir_CurrStep->mkdir("Case_Numeric");
        dir_Numeric->cd();
        
        //cout<<"No Fault - 3\n";
        
        hEvtPerClkCyle->Write();
        
        allAnalysisCase[j].Num.hFlat->Write();
        allAnalysisCase[j].Num.hGaus_Pure->Write();
        allAnalysisCase[j].Num.hGaus_Convo->Write();
        
        allAnalysisCase[j].Num.func_Gaus_Pure->Write();
        allAnalysisCase[j].Num.func_Gaus_Convo->Write();
        
        allAnalysisCase[j].Num.hGausOverFit_Pure->Write();
        allAnalysisCase[j].Num.hGausOverFit_Convo->Write();
        
        //Case: Continuous Convolution
        TDirectory *dir_Continous = dir_CurrStep->mkdir("Case_Continuous");
        dir_Continous->cd();
        
        //cout<<"No Fault - 4\n";
        
        allAnalysisCase[j].Cont.func_Gaus_Convo->Write();
        
        allAnalysisCase[j].Cont.hGausOverFit_Convo->Write();
        
        //Case: Discrete Convolution
        TDirectory *dir_Discrete = dir_CurrStep->mkdir("Case_Discrete");
        dir_Discrete->cd();
        
        //cout<<"No Fault - 5\n";
        
        allAnalysisCase[j].Disc.hGaus_Pure->Write();
        allAnalysisCase[j].Disc.hGaus_Convo->Write();
        
        allAnalysisCase[j].Disc.func_Gaus_Pure->Write();
        allAnalysisCase[j].Disc.func_Gaus_Convo->Write();
        
        allAnalysisCase[j].Disc.hGausOverFit_Pure->Write();
        allAnalysisCase[j].Disc.hGausOverFit_Convo->Write();
        
        //Delete Pointers
        /*if (fSigma != fSigma_max) { //End Delete Pointers
         //General Info
         delete hEvtPerClkCyle;
         delete leg_timeRespCal;
         
         //Numeric Case
         delete hFlatRndm;
         delete hGaus;
         delete hConvoGaus;
         delete func_ContConvo_CurrStep_Fitted;  //This is fitted to the numeric case so we put it here
         
         //Continous Convo
         delete func_ContConvo_CurrStep;
         
         //Discrete Convo
         delete hGaus_4_DiscConvo;
         delete hConvoGaus_4_DiscConvo;
         } //End Delete Pointers
         */
        
        //Doesn't Really Work???
        /*if (bPause4NextIter) {
         int iPause;
         cout<<"You Have a Moment To Review Plots for Current Iteration\n";
         cout<<"Pause Again After Next Iter? (Y=1/N=0)\n";
         cin>>iPause;
         
         (iPause == 1) ? bPause4NextIter = true : bPause4NextIter = false;
         }*/
    } //End Loop from fSigma_min to fSigma_max
    
    //Set Maximum Number of Digits
    //TGaxis::SetMaxDigits(3);
    
    //Initialize All the TGraphErrors
    //TGraph *gTimeRespCal = new TGraph(iNPoints, dNum_GausConvo, dNum_GausPure);
    TGraphErrors *gNum_SimSigma_v_ConvoSigma = new TGraphErrors(iNPoints, dNum_GausConvo, dSimSigma);
    TGraphErrors *gCont_PureSigma_v_ConvoSigma = new TGraphErrors(iNPoints, dNum_GausConvo, dCont_GausPure, dNum_GausConvo_Err, dCont_GausPure_Err );
    TGraphErrors *gDisc_SimSigma_v_ConvoSigma = new TGraphErrors(iNPoints, dDisc_GausConvo, dSimSigma);
    
    TGraphErrors *gNum_PureSigma_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dNum_GausPure);
    TGraphErrors *gCont_PureSigma_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dCont_GausPure);
    TGraphErrors *gDisc_PureSigma_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dDisc_GausPure);
    
    TGraphErrors *gNum_PureOverSim_v_SimSigma = (TGraphErrors*) gNum_PureSigma_v_SimSigma->Clone("gNum_PureOverSim_v_SimSigma");
    TGraphErrors *gCont_PureOverSim_v_SimSigma = (TGraphErrors*) gCont_PureSigma_v_SimSigma->Clone("gCont_PureOverSim_v_SimSigma");
    TGraphErrors *gDisc_PureOverSim_v_SimSigma = (TGraphErrors*) gDisc_PureSigma_v_SimSigma->Clone("gDisc_PureOverSim_v_SimSigma");
    
    TGraphErrors *gNum_PureNormChi2_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dNum_GausPure_normChi2);
    TGraphErrors *gNum_ConvoNormChi2_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dNum_GausConvo_normChi2);
    TGraphErrors *gCont_PureNormChi2_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dCont_GausPure_normChi2);
    TGraphErrors *gDisc_PureNormChi2_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dDisc_GausPure_normChi2);
    TGraphErrors *gDisc_ConvoNormChi2_v_SimSigma = new TGraphErrors(iNPoints, dSimSigma, dDisc_GausConvo_normChi2);
    
    //Set TName on All TGraphErrors
    gNum_SimSigma_v_ConvoSigma->SetName("gNum_SimSigma_v_ConvoSigma");
    gDisc_SimSigma_v_ConvoSigma->SetName("gDisc_SimSigma_v_ConvoSigma");
    gCont_PureSigma_v_ConvoSigma->SetName("gCont_PureSigma_v_ConvoSigma");
    
    gNum_PureSigma_v_SimSigma->SetName("gNum_PureSigma_v_SimSigma");
    gCont_PureSigma_v_SimSigma->SetName("gCont_PureSigma_v_SimSigma");
    gDisc_PureSigma_v_SimSigma->SetName("gDisc_PureSigma_v_SimSigma");
    
    gNum_PureNormChi2_v_SimSigma->SetName("gNum_PureNormChi2_v_SimSigma");
    gNum_ConvoNormChi2_v_SimSigma->SetName("gNum_ConvoNormChi2_v_SimSigma");
    gCont_PureNormChi2_v_SimSigma->SetName("gCont_PureNormChi2_v_SimSigma");
    gDisc_PureNormChi2_v_SimSigma->SetName("gDisc_PureNormChi2_v_SimSigma");
    gDisc_ConvoNormChi2_v_SimSigma->SetName("gDisc_ConvoNormChi2_v_SimSigma");
    
    /*cout<<"i\tStoredErr\tHistoError\n";
    for (int i=0; i<iNPoints-1; ++i) {
        cout<<i<<"\t"<<dDisc_GausConvo_Err[i]<<"\t"<<allAnalysisCase[i].Disc.hGaus_Convo->GetRMS(11)<<endl;
    }*/
    
    //Set Errors on All TGraphErrors & Perform Division for Select TGraphs
    for (int i=0; i<iNPoints; ++i) { //Loop Over Points in TGraphErrors
        gNum_SimSigma_v_ConvoSigma->SetPointError(i,dNum_GausConvo_Err[i],0.);
        gDisc_SimSigma_v_ConvoSigma->SetPointError(i,dDisc_GausConvo_Err[i],0.);
        
        gNum_PureSigma_v_SimSigma->SetPointError(i,0.,dNum_GausPure_Err[i]);
        gCont_PureSigma_v_SimSigma->SetPointError(i,0.,dCont_GausPure_Err[i]);
        gDisc_PureSigma_v_SimSigma->SetPointError(i,0.,dDisc_GausPure_Err[i]);
        
        gNum_PureOverSim_v_SimSigma->SetPoint(i,dSimSigma[i], dNum_GausPure[i] / dSimSigma[i]);
        gCont_PureOverSim_v_SimSigma->SetPoint(i,dSimSigma[i], dCont_GausPure[i] / dSimSigma[i]);
        gDisc_PureOverSim_v_SimSigma->SetPoint(i,dSimSigma[i], dDisc_GausPure[i] / dSimSigma[i]);
        
        gNum_PureOverSim_v_SimSigma->SetPointError(i,0., dNum_GausPure_Err[i] / dSimSigma[i]);
        gCont_PureOverSim_v_SimSigma->SetPointError(i,0., dCont_GausPure_Err[i] / dSimSigma[i]);
        gDisc_PureOverSim_v_SimSigma->SetPointError(i,0., dDisc_GausPure_Err[i] / dSimSigma[i]);
    } //End Loop Over Points in TGraphErrors
    
    //Fit Pure Sigma vs Sim Sigma
    //TF1 *func_Num_PureSigma_v_SimSigma = new TF1("func_Num_PureSigma_v_SimSigma","[0]*x+[1]",fSigma_min,fSigma_max);
    //TF1 *func_Cont_PureSigma_v_SimSigma = new TF1("func_Cont_PureSigma_v_SimSigma","[0]*x+[1]",fSigma_min,fSigma_max);
    //TF1 *func_Disc_PureSigma_v_SimSigma = new TF1("func_Disc_PureSigma_v_SimSigma","[0]*x+[1]",fSigma_min,fSigma_max);
    
    //Fit Sim Sigma v Convo Sigma
    TF1 *func_Num_SimSigma_v_ConvoSigma = new TF1("func_Num_SimSigma_v_ConvoSigma","sqrt( ( ( x-[0] )/ [1] )^2 - [2] )",0.,fSigma_max);
    func_Num_SimSigma_v_ConvoSigma->SetParameter(0,0.02);   //Initial Guess
    func_Num_SimSigma_v_ConvoSigma->SetParameter(1,1.);
    func_Num_SimSigma_v_ConvoSigma->SetParameter(2,52.);
    
    //Lines To Guide the Eye
    TF1 *func_Line_m0 = new TF1("func_Line_m0","[0]*x+[1]",-2.*fSigma_max,2.*fSigma_max);
    func_Line_m0->SetParameter(0,0.);
    func_Line_m0->SetParameter(1,1.);
    func_Line_m0->SetLineColor(kBlack);
    func_Line_m0->SetNpx(1000);
    //func_Line_m0->SetLineWidth(2);
    func_Line_m0->SetLineStyle(2);
    
    TF1 *func_Line_m1 = new TF1("func_Line_m1","[0]*x+[1]",-2.*fSigma_max,2.*fSigma_max);
    func_Line_m1->SetParameter(0,1.);
    func_Line_m1->SetParameter(1,0.);
    func_Line_m1->SetLineColor(kBlack);
    func_Line_m1->SetNpx(10000);
    //func_Line_m1->SetLineWidth(2);
    //func_Line_m1->SetLineStyle(3);
    
    //Plot Pure Sigma Versus Simulated Sigma (Should be Line w/slope = 1, intercept = 0)
    TCanvas *cVersusSimSigma = new TCanvas("cVersusSimSigma","Pure Sigma vs. Sim. Sigma",700,700);
    cVersusSimSigma->cd();
    
    //gNum_PureSigma_v_SimSigma->Fit(func_Num_PureSigma_v_SimSigma);
    //gCont_PureSigma_v_SimSigma->Fit(func_Cont_PureSigma_v_SimSigma);
    //gDisc_PureSigma_v_SimSigma->Fit(func_Disc_PureSigma_v_SimSigma);
    
    gNum_PureSigma_v_SimSigma->SetTitle("");
    gNum_PureSigma_v_SimSigma->SetMarkerStyle(21);
    gNum_PureSigma_v_SimSigma->SetMarkerSize(0.7);
    gNum_PureSigma_v_SimSigma->SetLineColor(kRed);
    gNum_PureSigma_v_SimSigma->SetMarkerColor(kRed);
    //func_Num_PureSigma_v_SimSigma->SetLineColor(kRed);
    //func_Num_PureSigma_v_SimSigma->SetLineStyle(2);
    //func_Num_PureSigma_v_SimSigma->SetLineWidth(2);
    gNum_PureSigma_v_SimSigma->GetYaxis()->SetTitle("#sigma_{pure} #left(ns#right)");
    gNum_PureSigma_v_SimSigma->GetXaxis()->SetTitle("#sigma_{sim.} #left(ns#right)");
    
    gCont_PureSigma_v_SimSigma->SetMarkerStyle(22);
    gCont_PureSigma_v_SimSigma->SetMarkerSize(0.7);
    gCont_PureSigma_v_SimSigma->SetLineColor(kRed+2);
    gCont_PureSigma_v_SimSigma->SetMarkerColor(kRed+2);
    //func_Cont_PureSigma_v_SimSigma->SetLineColor(kRed+2);
    //func_Cont_PureSigma_v_SimSigma->SetLineStyle(3);
    //func_Cont_PureSigma_v_SimSigma->SetLineWidth(2);
    
    gDisc_PureSigma_v_SimSigma->SetMarkerStyle(23);
    gDisc_PureSigma_v_SimSigma->SetMarkerSize(0.7);
    gDisc_PureSigma_v_SimSigma->SetLineColor(kRed+4);
    gDisc_PureSigma_v_SimSigma->SetMarkerColor(kRed+4);
    //func_Disc_PureSigma_v_SimSigma->SetLineColor(kRed+4);
    //func_Disc_PureSigma_v_SimSigma->SetLineStyle(4);
    //func_Disc_PureSigma_v_SimSigma->SetLineWidth(2);
    
    TLegend *leg_PureSigma_v_SimSigma = new TLegend(0.2,0.2,0.4,0.4);
    leg_PureSigma_v_SimSigma->SetLineColor(kWhite);
    leg_PureSigma_v_SimSigma->SetFillColor(kWhite);
    leg_PureSigma_v_SimSigma->AddEntry(gNum_PureSigma_v_SimSigma,"Numeric","LP");
    leg_PureSigma_v_SimSigma->AddEntry(gCont_PureSigma_v_SimSigma,"Continuous","LP");
    leg_PureSigma_v_SimSigma->AddEntry(gDisc_PureSigma_v_SimSigma,"Discrete","LP");
    
    gNum_PureSigma_v_SimSigma->Draw("AP");
    gCont_PureSigma_v_SimSigma->Draw("sameP");
    gDisc_PureSigma_v_SimSigma->Draw("sameP");
    //func_Num_PureSigma_v_SimSigma->Draw("same");
    //func_Cont_PureSigma_v_SimSigma->Draw("same");
    //func_Disc_PureSigma_v_SimSigma->Draw("same");
    func_Line_m1->Draw("same");
    leg_PureSigma_v_SimSigma->Draw("same");
    
    //Plot Pure/Sim Sigma Versus Sim Sigma (Should be Line w/slope = 0, intercept = 1)
    TCanvas *cRatioVersusSimSigma = new TCanvas("cRatioVersusSimSigma","Pure/Sim Sigma vs. Sim. Sigma",700,700);
    cRatioVersusSimSigma->cd();
    
    gNum_PureOverSim_v_SimSigma->SetTitle("");
    gNum_PureOverSim_v_SimSigma->SetMarkerStyle(21);
    gNum_PureOverSim_v_SimSigma->SetMarkerSize(0.7);
    gNum_PureOverSim_v_SimSigma->SetLineColor(kRed);
    gNum_PureOverSim_v_SimSigma->SetMarkerColor(kRed);
    //func_Num_PureSigma_v_SimSigma->SetLineColor(kRed);
    //func_Num_PureSigma_v_SimSigma->SetLineStyle(2);
    //func_Num_PureSigma_v_SimSigma->SetLineWidth(2);
    gNum_PureOverSim_v_SimSigma->GetYaxis()->SetTitle("#sigma_{pure} / #sigma_{sim.} #left(A.U.#right)");
    gNum_PureOverSim_v_SimSigma->GetYaxis()->SetRangeUser(0.95,1.05);
    gNum_PureOverSim_v_SimSigma->GetYaxis()->SetDecimals(true);
    gNum_PureOverSim_v_SimSigma->GetXaxis()->SetTitle("#sigma_{sim.} #left(ns#right)");
    
    gCont_PureOverSim_v_SimSigma->SetMarkerStyle(22);
    gCont_PureOverSim_v_SimSigma->SetMarkerSize(0.7);
    gCont_PureOverSim_v_SimSigma->SetLineColor(kRed+2);
    gCont_PureOverSim_v_SimSigma->SetMarkerColor(kRed+2);
    //func_Cont_PureSigma_v_SimSigma->SetLineColor(kRed+2);
    //func_Cont_PureSigma_v_SimSigma->SetLineStyle(3);
    //func_Cont_PureSigma_v_SimSigma->SetLineWidth(2);
    
    gDisc_PureOverSim_v_SimSigma->SetMarkerStyle(23);
    gDisc_PureOverSim_v_SimSigma->SetMarkerSize(0.7);
    gDisc_PureOverSim_v_SimSigma->SetLineColor(kRed+4);
    gDisc_PureOverSim_v_SimSigma->SetMarkerColor(kRed+4);
    //func_Disc_PureSigma_v_SimSigma->SetLineColor(kRed+4);
    //func_Disc_PureSigma_v_SimSigma->SetLineStyle(4);
    //func_Disc_PureSigma_v_SimSigma->SetLineWidth(2);
    
    gNum_PureOverSim_v_SimSigma->Draw("AP");
    gCont_PureOverSim_v_SimSigma->Draw("sameP");
    gDisc_PureOverSim_v_SimSigma->Draw("sameP");
    leg_PureSigma_v_SimSigma->Draw("same");
    func_Line_m0->Draw("same");
    leg_PureSigma_v_SimSigma->Draw("same");
    
    //Plot (Pure) Sim Sigma Versus Convoluted Sigma
    TCanvas *cVersusConvoSigma = new TCanvas("cVersusConvoSigma","Pure/Sim. Sigma vs. Convo. Sigma",700,700);
    cVersusConvoSigma->cd();
    
    gNum_SimSigma_v_ConvoSigma->SetTitle("");
    gNum_SimSigma_v_ConvoSigma->SetMarkerStyle(21);
    gNum_SimSigma_v_ConvoSigma->SetMarkerSize(0.7);
    gNum_SimSigma_v_ConvoSigma->SetLineColor(kRed);
    gNum_SimSigma_v_ConvoSigma->SetMarkerColor(kRed);
    gNum_SimSigma_v_ConvoSigma->GetYaxis()->SetTitle("#sigma_{sim.} #left(ns#right)");
    gNum_SimSigma_v_ConvoSigma->GetXaxis()->SetTitle("#sigma_{convo.} #left(ns#right)");
    gNum_SimSigma_v_ConvoSigma->Fit(func_Num_SimSigma_v_ConvoSigma);
    
    gCont_PureSigma_v_ConvoSigma->SetMarkerStyle(22);
    gCont_PureSigma_v_ConvoSigma->SetMarkerSize(0.7);
    gCont_PureSigma_v_ConvoSigma->SetLineColor(kRed+2);
    gCont_PureSigma_v_ConvoSigma->SetMarkerColor(kRed+2);
    
    gDisc_SimSigma_v_ConvoSigma->SetMarkerStyle(23);
    gDisc_SimSigma_v_ConvoSigma->SetMarkerSize(0.7);
    gDisc_SimSigma_v_ConvoSigma->SetLineColor(kRed+4);
    gDisc_SimSigma_v_ConvoSigma->SetMarkerColor(kRed+4);
    
    TLegend *leg_SimSigma_v_ConvoSigma = new TLegend(0.2,0.2,0.4,0.4);
    leg_SimSigma_v_ConvoSigma->SetLineColor(kWhite);
    leg_SimSigma_v_ConvoSigma->SetFillColor(kWhite);
    leg_SimSigma_v_ConvoSigma->AddEntry(gNum_SimSigma_v_ConvoSigma,"Numeric","LP");
    leg_SimSigma_v_ConvoSigma->AddEntry(gCont_PureSigma_v_ConvoSigma,"Continuous","LP");
    leg_SimSigma_v_ConvoSigma->AddEntry(gDisc_SimSigma_v_ConvoSigma,"Discrete","LP");
    
    gNum_SimSigma_v_ConvoSigma->Draw("AP");
    gCont_PureSigma_v_ConvoSigma->Draw("sameP");
    gDisc_SimSigma_v_ConvoSigma->Draw("sameP");
    leg_SimSigma_v_ConvoSigma->Draw("same");
    func_Num_SimSigma_v_ConvoSigma->Draw("same");
    
    //Plot normChi2 Versus Sim Sigma
    TCanvas *cNormChi2 = new TCanvas("cNormChi2","Norm. Chi2 vs. Sim. Sigma",700,700);
    cNormChi2->cd();
    cNormChi2->cd()->SetLogy(1);
    
    gNum_PureNormChi2_v_SimSigma->SetTitle("");
    gNum_PureNormChi2_v_SimSigma->SetMarkerStyle(21);
    gNum_PureNormChi2_v_SimSigma->SetMarkerSize(0.7);
    gNum_PureNormChi2_v_SimSigma->SetMarkerColor(kRed);
    gNum_PureNormChi2_v_SimSigma->SetLineColor(kRed);
    gNum_PureNormChi2_v_SimSigma->GetYaxis()->SetRangeUser(1e-1,1e5);
    gNum_PureNormChi2_v_SimSigma->GetYaxis()->SetTitle("#chi^{2}/NDF");
    gNum_PureNormChi2_v_SimSigma->GetXaxis()->SetTitle("#sigma_{sim.} #left(ns#right)");
    
    gNum_ConvoNormChi2_v_SimSigma->SetMarkerStyle(22);
    gNum_ConvoNormChi2_v_SimSigma->SetMarkerSize(0.7);
    gNum_ConvoNormChi2_v_SimSigma->SetMarkerColor(kRed+2);
    gNum_ConvoNormChi2_v_SimSigma->SetLineColor(kRed+2);
    
    gCont_PureNormChi2_v_SimSigma->SetMarkerStyle(21);
    gCont_PureNormChi2_v_SimSigma->SetMarkerSize(0.7);
    gCont_PureNormChi2_v_SimSigma->SetMarkerColor(kGreen);
    gCont_PureNormChi2_v_SimSigma->SetLineColor(kGreen);
    
    gDisc_PureNormChi2_v_SimSigma->SetMarkerStyle(21);
    gDisc_PureNormChi2_v_SimSigma->SetMarkerSize(0.7);
    gDisc_PureNormChi2_v_SimSigma->SetMarkerColor(kBlue);
    gDisc_PureNormChi2_v_SimSigma->SetLineColor(kBlue);
    
    gDisc_ConvoNormChi2_v_SimSigma->SetMarkerStyle(22);
    gDisc_ConvoNormChi2_v_SimSigma->SetMarkerSize(0.7);
    gDisc_ConvoNormChi2_v_SimSigma->SetMarkerColor(kBlue+2);
    gDisc_ConvoNormChi2_v_SimSigma->SetLineColor(kBlue+2);
    
    TLegend *leg_NormChi2 = new TLegend(0.2,0.2,0.4,0.4);
    leg_NormChi2->SetLineColor(kWhite);
    leg_NormChi2->SetFillColor(kWhite);
    leg_NormChi2->AddEntry(gNum_PureNormChi2_v_SimSigma,"Numeric, Pure","LP");
    leg_NormChi2->AddEntry(gNum_ConvoNormChi2_v_SimSigma,"Numeric, Convo","LP");
    leg_NormChi2->AddEntry(gCont_PureNormChi2_v_SimSigma,"Continuous, Pure","LP");
    leg_NormChi2->AddEntry(gDisc_PureNormChi2_v_SimSigma,"Discrete, Pure","LP");
    leg_NormChi2->AddEntry(gDisc_ConvoNormChi2_v_SimSigma,"Discrete, Convo","LP");
    
    gNum_PureNormChi2_v_SimSigma->Draw("AP");
    gNum_ConvoNormChi2_v_SimSigma->Draw("sameP");
    gCont_PureNormChi2_v_SimSigma->Draw("sameP");
    gDisc_PureNormChi2_v_SimSigma->Draw("sameP");
    gDisc_ConvoNormChi2_v_SimSigma->Draw("sameP");
    leg_NormChi2->Draw("same");
    
    //Store
    TDirectory *dir_Summary = dir_Sim->mkdir("Summary");
    TDirectory *dir_normChi2 = dir_Summary->mkdir("normChi2");
    TDirectory *dir_VsSimSigma = dir_Summary->mkdir("VersusSimSigma");
    TDirectory *dir_VsConvoSigma = dir_Summary->mkdir("VersusConvoSigma");
    
    dir_normChi2->cd();
    gNum_PureNormChi2_v_SimSigma->Write();
    gNum_ConvoNormChi2_v_SimSigma->Write();
    gCont_PureNormChi2_v_SimSigma->Write();
    gDisc_PureNormChi2_v_SimSigma->Write();
    gDisc_ConvoNormChi2_v_SimSigma->Write();
    cNormChi2->Write();
    
    dir_VsSimSigma->cd();
    //func_Num_PureSigma_v_SimSigma->Write();
    //func_Cont_PureSigma_v_SimSigma->Write();
    //func_Disc_PureSigma_v_SimSigma->Write();
    gNum_PureSigma_v_SimSigma->Write();
    gNum_PureOverSim_v_SimSigma->Write();

    gCont_PureSigma_v_SimSigma->Write();
    gCont_PureOverSim_v_SimSigma->Write();

    gDisc_PureSigma_v_SimSigma->Write();
    gDisc_PureOverSim_v_SimSigma->Write();
    
    cVersusSimSigma->Write();
    cRatioVersusSimSigma->Write();
    
    dir_VsConvoSigma->cd();
    gNum_SimSigma_v_ConvoSigma->Write();
    gDisc_SimSigma_v_ConvoSigma->Write();
    gCont_PureSigma_v_ConvoSigma->Write();
    cVersusConvoSigma->Write();
    func_Num_SimSigma_v_ConvoSigma->Write();
    
    //Close the File
    outputFile->Close();
    
    return 0;
} //End timeResponseCalibration()