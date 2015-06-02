//
//  treeAnalyzerTDC.cpp
//  
//
//  Created by Brian L Dorney on 30/03/15.
//
//

#include "treeAnalyzerTDC.h"

treeAnalyzerTDC::treeAnalyzerTDC(){
    bVerbose_IO = false;
    
    strSecBegin_Sel = "[BEGIN_SELECTION]";
    strSecBegin_Canvas = "[BEGIN_CANVAS]";
    strSecBegin_Plot = "[BEGIN_PLOT]";
    
    strSecEnd_Sel = "[END_SELECTION]";
    strSecEnd_Canvas = "[END_CANVAS]";
    strSecEnd_Plot = "[END_PLOT]";
}; //End Constructor

//Calls treeAnalyzer::applySelection(strInputTextFile) and then for each plot made here it analyzes the raw data that went into making each plot
//The output is stored inside the Plots directory for each Canvas created
void treeAnalyzerTDC::applySelectionTDC(string strOutputROOTFile){
    //Variable Declaration
    bool bExitSuccess = false;
    
    //Apply the Selection
    //------------------------------------------------------
    applySelection(strOutputROOTFile);
    
    //Load the ROOT file that was just created in the UPDATE mode
    //------------------------------------------------------
    //TFile *file_ROOT_Output = new TFile(strOutputROOTFile.c_str(), "UPDATE", "", 1);
    //TFile file_ROOT_Output = getFile(strOutputROOTFile, "UPDATE", bExitSuccess);
    TFile * file_ROOT_Output = getFile(strOutputROOTFile, "UPDATE", bExitSuccess);
    
    if (!bExitSuccess) { //Case: Output File Was Not Created Successfully
        if(bVerbose_IO) {
            cout<< ("treeAnalyzerTDC::applySelectionTDC() - Output ROOT File: " + strOutputROOTFile + " was not successfully created, stopping\n").c_str();
            cout<<"treeAnalyzerTDC::applySelectionTDC() - Please Cross-Check (Maybe you do not have write-permission in working directory or filepath does not exist???)"<<endl;
        }
        
        return;
    } //End Case: Output File Was Not Created Successfully
    
    //Loop Over Selection Input to create the indepth TDC Analysis
    //------------------------------------------------------
    for (auto iterSel = vecSelInfo.begin(); iterSel != vecSelInfo.end(); ++iterSel) { //Loop Over vecSelInfo
        //Get the TDirectory that was made for this SelInfo
        TDirectory *dir_thisSel = file_ROOT_Output->GetDirectory( (*iterSel).strDirectory.c_str(), false, "GetDirectory" );
        
        //Loop over all CanvasInfo's that were requested for this SelInfo
        //------------------------------------------------------
        for (auto iterCanvas = (*iterSel).mapCanvas.begin(); iterCanvas != (*iterSel).mapCanvas.end(); ++iterCanvas) { //Loop Over (*iterSel).mapCanvas
            //Get the TDirectory that was made for this CanvasInfo
            TDirectory *dir_thisCanvas = dir_thisSel->GetDirectory( ((*iterCanvas).second).strDirectory.c_str(), false, "GetDirectory" );
            
            //Loop over all PlotInfo's that were requested for this CanvasInfo
            //------------------------------------------------------
            for (auto iterPlot = ((*iterCanvas).second).mapPlot.begin(); iterPlot != ((*iterCanvas).second).mapPlot.end(); ++iterPlot) { //Loop Over ((*iterCanvas).second).mapPlot
                //Skip this plot if no friend branch was defined by the user
                if ( ((*iterPlot).second).strFriendBranch.length() == 0 ) continue;
                
                //Get the TDirectory that was made for this PlotInfo
                //------------------------------------------------------
                TDirectory *dir_thisPlot = dir_thisCanvas->GetDirectory( ((*iterPlot).second).strDirectory.c_str(), false, "GetDirectory" );
                TDirectory *dir_HistoWithFit = dir_thisPlot->mkdir( ("HistoWithFit_" + ((*iterPlot).second).strVarIndep).c_str() );
                TDirectory *dir_HistoOverFit = dir_thisPlot->mkdir( ("HistoOverFit_" + ((*iterPlot).second).strVarIndep).c_str() );
                
                //Open the Input Data File
                //------------------------------------------------------
                TFile * file_ROOT_Input = getFile( ((*iterPlot).second).strNameROOTFile, "READ", bExitSuccess );
                
                if (!bExitSuccess) { //Case: Input File Failed To Open Successfully
                    if (bVerbose_IO) {
                        std::cout << ("treeAnalyzerTDC::applySelectionTDC(): error while opening file: " + ((*iterPlot).second).strNameROOTFile).c_str() << endl;
                        std::cout << "Skipping Plot: " << ((*iterPlot).second).strName << endl;
                    }
                    
                    continue;
                } //End Case: Input File Failed To Open Successfully
                
                //Load the TTree from file_ROOT_Input
                //------------------------------------------------------
                TTree *treeInput = (TTree*) file_ROOT_Input->Get( ((*iterPlot).second).strNameTree.c_str() );
                
                //Note the Selection is already setup due to calling "applySelection()" of the parent class
                
                //Declaration of TTree's leaf types
                //------------------------------------------------------
                int iRun;                           //value of the run number
                
                float fVarIndep;                    //value of the independent variable for the i^th point
                
                TH1F *hTDC_Histo = new TH1F();      //Histogram from the i^th point
                
                TF1 *func_FriendBranch = new TF1(); //Fit Function from the i^th point
                
                //Set the branch addresses
                //------------------------------------------------------
                treeInput->SetBranchAddress("iRun",&iRun);
                treeInput->SetBranchAddress( ((*iterPlot).second).strVarIndep.c_str(), &fVarIndep);
                treeInput->SetBranchAddress("hTDC_Histo",&hTDC_Histo);
                treeInput->SetBranchAddress( ((*iterPlot).second).strFriendBranch.c_str(), &func_FriendBranch);
                
                //Loop Over the Entries in treeInput that pass the selection
                //------------------------------------------------------
                //Inform the user we have moved to a new SelInfo and then draw the tree
                //cout<< ("Selection = '" + (*iterSel).strSel + "'" ) << endl;
                //treeInput->Draw( ">>listSelEvts", (*iterSel).strSel.c_str(), "entrylist" );
                
                cout<< ("Selection = '" + ((*iterPlot).second).strSelLocal + "'" ) << endl;
                treeInput->Draw( ">>listSelEvts", ((*iterPlot).second).strSelLocal.c_str(), "entrylist" );
                
                //Get the Entry List
                TEntryList *listSelEvts = (TEntryList*) gDirectory->Get("listSelEvts");
                
                //Loop Over the events passing ((*iterPlot).second).strSelLocal stored in eventList
                //cout<<"i\tidx_EvtList\tSigma_Fit\n";
                for (int i=0; i < listSelEvts->GetN(); ++i) { //Loop over events stored in eventList
                    //int iEvtIdx = listSelEvts->Next();  //Should probably use listSelEvts->GetEntry(i) because duplicate calls of Next() per loop iteration cause undesired iteration through the list
                    
                    //treeInput->GetEntry( iEvtIdx );
                    treeInput->GetEntry( listSelEvts->GetEntry(i) );
                    
                    //Skip this Event if the Histogram pointer is a null pointer
                    if (hTDC_Histo == nullptr){
                        continue;
                    }
                    else{ //Otherwise Set the Histogram Style
                        hTDC_Histo = getHistogram( (*iterPlot).second, hTDC_Histo);
                    }
                    
                    //treeInput->Show();
                    //if(i==0) treeInput->Show();
                    
                    //Set the Style
                    
                    //Create the Canvas - Histogram w/Fit
                    //------------------------------------------------------
                    //set the style based on user input
                    //Make a copy of the CanvasInfo, reset the name, and then pass it to treeAnalyzer::getCanvas()
                    CanvasInfo tempCanvasInfo = (*iterCanvas).second;
                    tempCanvasInfo.strName = "canvas_DataHistoWithFit_R" + getString(iRun) + "_" + ((*iterPlot).second).strVarDepend + "_" + ((*iterPlot).second).strVarIndep + getString(fVarIndep);
                    
                    TCanvas *cHistoWithFit = getCanvas(tempCanvasInfo);
                    cHistoWithFit->cd();
                    
                    TLegend *leg = (TLegend *) ((*iterCanvas).second).leg->Clone( ("leg_HistoWithFit_" + getString(fVarIndep) ).c_str() );
                    leg->Clear();   //Wipe all previous entries
                    
                    leg->AddEntry(hTDC_Histo,"Data","LPE");
                    
                    //Plot Histogram on the Canvas
                    cHistoWithFit->cd();
                    hTDC_Histo->Draw("E1");
                    
                    //Check that the fit function exists
                    if ( func_FriendBranch != nullptr) {
                        //Set the style
                        func_FriendBranch->SetLineColor( ((*iterPlot).second).iColor );
                        func_FriendBranch->SetLineStyle( ((*iterPlot).second).iStyleLine );
                        func_FriendBranch->SetLineWidth( ((*iterPlot).second).fSizeLine );
                        
                        //Add to the Legend
                        leg->AddEntry(func_FriendBranch, "Fit", "L");
                        
                        //Draw
                        func_FriendBranch->Draw("same");
                    }
                    
                    //Draw the legend
                    leg->Draw("same");
                    
                    //Store
                    dir_HistoWithFit->cd();
                    cHistoWithFit->Write();
                    hTDC_Histo->Write();
                    func_FriendBranch->Write();
                    //if ( func_FriendBranch != nullptr) func_FriendBranch->Write();
                    
                    //Create the Canvas - Histogram DIVIDED by Fit
                    //------------------------------------------------------
                    //set the style based on user input
                    //Make a copy of the CanvasInfo, reset the name, and then pass it to treeAnalyzer::getCanvas()
                    tempCanvasInfo.strName = "canvas_DataHistoOverFit_R" + getString(iRun) + "_" + ((*iterPlot).second).strVarDepend + "_" + ((*iterPlot).second).strVarIndep + getString(fVarIndep);
                    
                    TCanvas *cHistoOverFit = getCanvas(tempCanvasInfo);
                    cHistoOverFit->cd();
                    
                    TH1F *hTDC_HistoOverFit = (TH1F *) hTDC_Histo->Clone( (hTDC_Histo->GetName() + getString("_OverFit") ).c_str() );
                    
                    hTDC_HistoOverFit->Divide(func_FriendBranch);
                    
                    hTDC_HistoOverFit->GetYaxis()->SetRangeUser(0.,2.);
                    
                    hTDC_HistoOverFit->Draw("E1");
                    
                    //store
                    dir_HistoOverFit->cd();
                    cHistoOverFit->Write();
                    hTDC_HistoOverFit->Write();
                    
                    //Delete pointers
                    delete cHistoWithFit;
                    delete cHistoOverFit;
                    //delete func_FriendBranch; //No this breaks the TTree::GetEntry() Call
                    //delete hTDC_Histo;        //No this breaks the TTree::GetEntry() Call
                    delete leg;
                } //End Loop over events stored in eventList
                
                //Close the Input ROOT File
                //------------------------------------------------------
                file_ROOT_Input->Close();
            } //End Loop Over ((*iterCanvas).second).mapPlot
        } //End Loop Over (*iterSel).mapCanvas
    } //End Loop Over vecSelInfo
    
    //Close the output ROOT file
    //------------------------------------------------------
    file_ROOT_Output->Close();
    
    return;
} //End treeAnalyzerTDC::applySelectionTDC()