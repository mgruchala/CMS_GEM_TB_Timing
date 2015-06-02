//
//  treeAnalyzer.cpp
//  
//
//  Created by Brian L Dorney on 30/03/15.
//
//

#include "treeAnalyzer.h"

treeAnalyzer::treeAnalyzer(){
    bVerbose_IO = false;
    
    strSecBegin_Sel = "[BEGIN_SELECTION]";
    strSecBegin_Canvas = "[BEGIN_CANVAS]";
    strSecBegin_Plot = "[BEGIN_PLOT]";
    
    strSecEnd_Sel = "[END_SELECTION]";
    strSecEnd_Canvas = "[END_CANVAS]";
    strSecEnd_Plot = "[END_PLOT]";
}; //End Constructor

void treeAnalyzer::setSelection(string strInputTextFile){
    //Variable Declaration
    string line;
    
    //Open the input text file
    //------------------------------------------------------
    if (bVerbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeAnalyzer::writeTree(): trying to open and read: " << strInputTextFile << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream dataInput( strInputTextFile.c_str() );
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!dataInput.is_open() && bVerbose_IO) {
        perror( ("treeAnalyzer::writeTree(): error while opening file: " + strInputTextFile).c_str() );
        printStreamStatus(dataInput);
    }
    
    //Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getlineNoSpaces(dataInput, line ) ) {
        std::pair<string,string> paramPair;
        
        //Does the user want to comment out this line?
        if ( line.compare(0,1,"#") == 0) continue;
        
        //Identify Section Header
        if ( line.compare(strSecBegin_Sel) == 0 ) { //Case: i^th Selection Definition
            SelInfo thisSelection;
            
            std::cout<<"New Selection!\n";
            
            while (getlineNoSpaces(dataInput, line ) ) { //Loop to Define Selection
                bool bExitSuccess;
                
                //Does the user want to comment out this line?
                if ( line.compare(0,1,"#") == 0) continue;
                
                //Has this section ended?
                if ( line.compare(strSecEnd_Sel) == 0 ){ //Case: Section Has Ended - Selection
                    vecSelInfo.push_back(thisSelection);
                    break;
                } //End Case: Section Has Ended - Selection
                
                //Check for next section
                if ( line.compare(strSecBegin_Canvas) == 0 ) { //Case: Definition of j^th Canvas of i^th Selection
                    CanvasInfo thisCanvas;
                    
                    std::cout<<"\tNew Canvas!\n";
                    
                    while (getlineNoSpaces(dataInput, line ) ) { //Loop to Define Canvas
                        
                        //Does the user want to comment out this line?
                        if ( line.compare(0,1,"#") == 0) continue;
                        
                        //Has this section ended?
                        if ( line.compare(strSecEnd_Canvas) == 0 ){ //Case: Section Has Ended - Canvases
                            //Check to make sure Key-Value does Not Exist
                            //If it does exist warn the user
                            //If It doesn't store the information
                            if (thisSelection.mapCanvas.count(thisCanvas.strName) > 0 ) { //Case: Key Value Already Exists, Warn User
                                cout<<"treeAnalyzer::setSelection - Multiple TCanvas'es with name = " << thisCanvas.strName << " for selection identifier = " << thisSelection.strIdent << "!!!\n";
                                cout<<"treeAnalyzer::setSelection - One or more of your TCanvas'es has not been stored!!!\n";
                                cout<<"treeAnalyzer::setSelection - Please cross-check input file:\n";
                                cout<<strInputTextFile<<endl;
                            } //End Case: Key Value Already Exists, Warn User
                            else{ //Case: Key Value Does Not Exist, Store!
                                thisSelection.mapCanvas[thisCanvas.strName] = thisCanvas;
                            } //End Case: Key Value Does Not Exist, Store!
                            
                            break;
                        } //End Case: Section Has Ended - Canvases
                        
                        //Check for next section
                        if ( line.compare(strSecBegin_Plot) == 0 ) { //Case: Definition of k^th Plot of j^th Canvas
                            PlotInfo thisPlot;
                            
                            std::cout<<"\t\tNew Plot!\n";
                            
                            while (getlineNoSpaces(dataInput, line ) ) { //Loop to Define Plot
                                
                                //Does the user want to comment out this line?
                                if ( line.compare(0,1,"#") == 0) continue;
                                
                                //Has this section ended?
                                if ( line.compare(strSecEnd_Plot) == 0 ){ //Case: Section Has Ended - Plots
                                    if (thisCanvas.mapPlot.count(thisPlot.strName) > 0 ) { //Case: Key Value Already Exists, Warn user
                                        cout<<"treeAnalyzer::setSelection - Multiple Plots with name = " << thisPlot.strName << " for Canvas name = " << thisCanvas.strName << "!!!\n";
                                        cout<<"treeAnalyzer::setSelection - One or more of your Plots has not been stored!!!\n";
                                        cout<<"treeAnalyzer::setSelection - Please cross-check input file:\n";
                                        cout<<strInputTextFile<<endl;
                                    } //End Case: Key Value Already Exists, Warn user
                                    else{ //Case: Key Value Does Not Exist, Store!
                                        thisCanvas.mapPlot[thisPlot.strName] = thisPlot;
                                    } //End Case: Key Value Does Not Exist, Store!
                                    
                                    break;
                                } //End Case: Section Has Ended - Plots
                                
                                //Gather Plot Parameters
                                paramPair = getParsedLine(line,bExitSuccess);
                                
                                //Debugging
                                //cout<<paramPair.first<<"\t"<<paramPair.second<<endl;
                                
                                if (bExitSuccess) setParamPlot(paramPair.first, paramPair.second, thisPlot);
                            } //End Loop to Define Plot
                            
                        } //End Case: Definition of k^th Plot of j^th Canvas
                        else{ //Case: Gathering Canvas Parameters
                            paramPair = getParsedLine(line,bExitSuccess);
                            
                            //Debugging
                            //cout<<paramPair.first<<"\t"<<paramPair.second<<endl;
                            
                            if (bExitSuccess) setParamCanvas(paramPair.first, paramPair.second, thisCanvas);
                        } //End Case: Gathering Canvas Parameters
                    } //End Loop to Define Canvas
                } //End Case: Definition of j^th Canvas of i^th Selection
                else{ //Case: Gathering Selection Parameters
                    paramPair = getParsedLine(line,bExitSuccess);
                    
                    //Debugging
                    //cout<<paramPair.first<<"\t"<<paramPair.second<<endl;
                    
                    //If parameter retrival was successful, set parameter
                    if(bExitSuccess) setParamSel(paramPair.first, paramPair.second, thisSelection);
                } //End Case: Gathering Selection Parameters
            } //End Loop to Define Selection
        } //End Case: Definition of i^th Selection
    } //End Loop Over dataInput
    
    //Close the File
    //------------------------------------------------------
    dataInput.close();
    
    //Debugging
    cout<<"Idx_Sel\tSel\tIdx_Canvas\tName_Canvas\tIdx_Plot\tName_Plot\tY_vs_X\tiColor\n";
    for (auto iterSel = vecSelInfo.begin(); iterSel != vecSelInfo.end(); ++iterSel) { //Loop Over vecSelInfo
        for (auto iterCanvas = (*iterSel).mapCanvas.begin(); iterCanvas != (*iterSel).mapCanvas.end(); ++iterCanvas) { //Loop Over Mapped Canvases
            for (auto iterPlot = (*iterCanvas).second.mapPlot.begin(); iterPlot != (*iterCanvas).second.mapPlot.end(); ++iterPlot) { //Loop Over Mapped Plots
                cout<<std::distance(vecSelInfo.begin(),iterSel)<<"\t"<<(*iterSel).strSel<<"\t";
                cout<<std::distance((*iterSel).mapCanvas.begin(),iterCanvas)<<"\t"<<(*iterCanvas).second.strName<<"\t";
                cout<<std::distance((*iterCanvas).second.mapPlot.begin(),iterPlot)<<"\t"<<(*iterPlot).second.strName<<"\t"<<(*iterPlot).second.strVarDepend<<" vs " <<(*iterPlot).second.strVarIndep<<"\t"<<(*iterPlot).second.iColor<<endl;
            } //End Loop Over Mapped Plots
        } //End Loop Over Mapped Canvases
        
        //cout<<endl;
    } //End Loop Over vecSelInfo
    
    return;
} //End treeAnalyzer::setSelection()

//For each member of vecSelGlobal create all plots defined with the addPlots methods, local selections may be applied
//A total of vecSelGlobal.size() * vecPlotInfo.size() plots will be created
void treeAnalyzer::applySelection(string strOutputROOTFile){
    //Variable Declaration
    bool bExitSuccess = false;
    
    //Create the Output Data File
    //------------------------------------------------------
    TFile *file_ROOT_Output = getFile(strOutputROOTFile, "RECREATE", bExitSuccess);
    
    if (!bExitSuccess) { //Case: Output File Was Not Created Successfully
        if(bVerbose_IO) {
        cout<< ("treeAnalyzer::applySelection() - Output ROOT File: " + strOutputROOTFile + " was not successfully created, stopping\n").c_str();
        cout<<"treeAnalyzer::applySelection() - Please Cross-Check (Maybe you do not have write-permission in working directory or filepath does not exist???)"<<endl;
        }
            
        return;
    } //End Case: Output File Was Not Created Successfully
    
    //Loop Over vecSelInfo
    for (auto iterSel = vecSelInfo.begin(); iterSel != vecSelInfo.end(); ++iterSel) { //Loop Over vecSelInfo
        //Create the storage directory for this element of vecSelInfo
        (*iterSel).strDirectory = (*iterSel).strIdent;  //This is done so it can be accessed easily by inherited classes
        TDirectory *dir_thisSel = file_ROOT_Output->mkdir( (*iterSel).strDirectory.c_str() );
        
        //Loop Over input Canvas Info
        for (auto iterCanvas = (*iterSel).mapCanvas.begin(); iterCanvas != (*iterSel).mapCanvas.end(); ++iterCanvas) { //Loop Over (*iterSel).mapCanvas
            //Create the storage directory for this Canvas, use un-modified canvas name here
            ((*iterCanvas).second).strDirectory = ((*iterCanvas).second).strName; //This is done so it can be accessed easily by inherited classes
            TDirectory *dir_thisCanvas = dir_thisSel->mkdir( ((*iterCanvas).second).strDirectory.c_str() );
            
            //Create the map for point labels
            //std::vector<std::tuple<float,float,std::string> > vec_PlotLabels;
            
            //Set the name of the TCanvas
            ((*iterCanvas).second).strName = "canvas_" + (*iterSel).strIdent + "_" + ((*iterCanvas).second).strName;
            
            //Create the Canvas
            TCanvas *cCurrentCanvas = getCanvas( (*iterCanvas).second );
            
            //Initialize the Multigraph
            ((*iterCanvas).second).mGraph = new TMultiGraph( ("mGraph_" + (*iterSel).strIdent + "_" + ((*iterCanvas).second).strName).c_str()," ;X TITLE;Y TITLE" );
            
            //Initialize the Legend
            if (((*iterCanvas).second).fLegNDCPos_X1 > 0 && ((*iterCanvas).second).fLegNDCPos_X2 > 0
                && ((*iterCanvas).second).fLegNDCPos_Y1 > 0 && ((*iterCanvas).second).fLegNDCPos_Y2 > 0) { //Case: User Specified Legend Size
                ((*iterCanvas).second).leg = new TLegend(((*iterCanvas).second).fLegNDCPos_X1,((*iterCanvas).second).fLegNDCPos_Y1,((*iterCanvas).second).fLegNDCPos_X2,((*iterCanvas).second).fLegNDCPos_Y2);
            } //End Case: User Specified Legend Size
            else{ //Case: User Has Not Specified Legend Size
                cout<<"treeAnalyzer::applySelection() - No Legend Size Specified for TCanvas:\n";
                cout<<"treeAnalyzer::applySelection() - " << ((*iterCanvas).second).strName << endl;
                cout<<"treeAnalyzer::applySelection() - Using Default Size\n";
                
                ((*iterCanvas).second).leg = new TLegend(0.2,0.2,0.4,0.4);
            } //End Case: User Has Not Specified Legend Size
            
            //Loop Over input Plot Info
            for (auto iterPlot = ((*iterCanvas).second).mapPlot.begin(); iterPlot != ((*iterCanvas).second).mapPlot.end(); ++iterPlot) { //Loop Over (*iterCanvas).mapPlot
                //Create the storage directory for this Canvas, use un-modified canvas name here
                ((*iterPlot).second).strDirectory = ((*iterPlot).second).strName; //This is done so it can be accessed easily by inherited classes
                TDirectory *dir_thisPlot = dir_thisCanvas->mkdir( ((*iterPlot).second).strDirectory.c_str() );
                
                //Set the Name of the Plots as:
                //  g_<Identifier>_<y>_vs_<x>
                ((*iterPlot).second).strName = "graph_" + (*iterSel).strIdent + "_" + ((*iterPlot).second).strName;
                
                //Debugging
                //std::cout<<"Creating Plot: " << ((*iterPlot).second).strName << endl;
                
                //Check to make sure the user has set a root file for this plot
                if (((*iterPlot).second).strNameROOTFile.length() == 0 ) { //Case: User has not specified a root file for this plot
                    std::cout << ("treeAnalyzer::applySelection() - You have not set a ROOT file for plot:" + ((*iterPlot).second).strName ).c_str() << endl;
                    std::cout << "treeAnalyzer::applySelection() - This plot will be skipped!!!\n";
                    
                    continue;
                } //End Case: User has not specified a root file for this plot
                
                //Check to make sure the user has set a tree name for this plot
                if (((*iterPlot).second).strNameROOTFile.length() == 0 ) { //Case: User has not specified a tree name for this plot
                    std::cout << ("treeAnalyzer::applySelection() - You have not set a ROOT::TTree for plot:" + ((*iterPlot).second).strName ).c_str() << endl;
                    std::cout << "treeAnalyzer::applySelection() - This plot will be skipped!!!\n";
                    
                    continue;
                } //End Case: User has not specified a tree name for this plot
                
                //Open the Input Data File
                //------------------------------------------------------
                TFile * file_ROOT_Input = getFile( ((*iterPlot).second).strNameROOTFile, "READ", bExitSuccess );
                
                if (!bExitSuccess) { //Case: Input File Failed To Open Successfully
                    if (bVerbose_IO) {
                        std::cout << ("treeAnalyzer::applySelection(): error while opening file: " + ((*iterPlot).second).strNameROOTFile).c_str() << endl;
                        std::cout << "Skipping Plot: " << ((*iterPlot).second).strName << endl;
                    }
                    
                    continue;
                } //End Case: Input File Failed To Open Successfully
                
                //Load the TTree from file_ROOT_Input
                //------------------------------------------------------
                TTree *treeInput = (TTree*) file_ROOT_Input->Get( ((*iterPlot).second).strNameTree.c_str() );
                
                if (treeInput == nullptr) { //Case: Tree Not Found
                    std::cout<<("treeAnalyzer::applySelection(): error while fetching tree: " + ((*iterPlot).second).strNameTree ).c_str() << endl;
                    std::cout << "Skipping Plot: " << ((*iterPlot).second).strName << endl;

                    continue;
                } //End Case: Tree Not Found
                
                //Set the Selection
                if ( ((*iterPlot).second).strSelLocal.length() > 0 ) { //Case: Local + Global Selection
                    ((*iterPlot).second).strSelLocal = (*iterSel).strSel + " && " + ((*iterPlot).second).strSelLocal;
                } //End Case: Local + Global Selection
                else{ //Case: Only Global Selection
                    ((*iterPlot).second).strSelLocal = (*iterSel).strSel;
                } //End Case: Only Global Selection
                
                //Declare the Pointer to the TGraphErrors (note a TGraph can be cast as a TGraphErrors since TGraphErrors inherits from TGraph)
                TGraphErrors *gCurrentPlot;
                
                //Create the TGraphErrors
                //Currently x-error bars are not supported
                if ( ((*iterPlot).second).strVarDependErr.length() > 0 ) { //Case: Y Errors are non-null
                    //Setup the Expression
                    ((*iterPlot).second).strExpress = ((*iterPlot).second).strVarIndep + ":" + ((*iterPlot).second).strVarDepend + ":" + ((*iterPlot).second).strVarDependErr;
                    
                    //Setup TGraphErrors & Draw the Tree simultaneously
                    gCurrentPlot = new TGraphErrors(treeInput->Draw(((*iterPlot).second).strExpress.c_str(),((*iterPlot).second).strSelLocal.c_str(), "goff" ), treeInput->GetV1(), treeInput->GetV2(), 0, treeInput->GetV3() );
                } //End Case: Y Errors are non-null
                else{ //Case: No Error Bars
                    //Setup the Expression
                    ((*iterPlot).second).strExpress = ((*iterPlot).second).strVarIndep + ":" + ((*iterPlot).second).strVarDepend;
                    
                    //Setup TGraphErrors & Draw the Tree simultaneously
                    gCurrentPlot = new TGraphErrors(treeInput->Draw(((*iterPlot).second).strExpress.c_str(),((*iterPlot).second).strSelLocal.c_str(), "goff" ), treeInput->GetV1(), treeInput->GetV2(), 0, 0 );
                    
                    //Other Option
                    //Seems to cause a seg fault; probably a problem with the gPad not being defined here?
                    //gCurrentPlot = (TGraphErrors*) gPad->GetPrimitive("Graph");
                } //End Case: No Error Bars
                
                //Update gCurrentPlot with all stored settings
                gCurrentPlot = getGraph( (*iterPlot).second, gCurrentPlot);
                
                //Plot the Graph
                //cCurrentCanvas->cd();   //It should already be the active canvas but, hey just to be sure
                
                /*//Draw the current plot
                if ( std::distance(((*iterCanvas).second).mapPlot.begin(), iterPlot) > 0 ) { //Case: All Subsequent Plots
                    gCurrentPlot->Draw( ("same " + (*iterPlot).second.strOptionDraw).c_str() );
                } //End Case: All Subsequent Plots
                else { //Case: First Plot
                    gCurrentPlot->Draw( (*iterPlot).second.strOptionDraw.c_str() );
                } //End Case: First Plot
                */
                
                //Store the Plot in a MultiGraph
                if ( std::distance(((*iterCanvas).second).mapPlot.begin(), iterPlot) > 0 ) { //Case: All Subsequent Plots
                    ((*iterCanvas).second).mGraph->Add(gCurrentPlot, ("same " + (*iterPlot).second.strOptionDraw).c_str() );
                } //End Case: All Subsequent Plots
                else { //Case: First Plot
                    ((*iterCanvas).second).mGraph->Add(gCurrentPlot, (*iterPlot).second.strOptionDraw.c_str() );
                } //End Case: First Plot
                
                //Store plot labels if requested
                if ( (*iterPlot).second.bPointLabels ) {
                    //vec_PlotLabels = getSelOrderedPairs( (*iterPlot).second, treeInput);
                    (*iterCanvas).second.mapPlotLabels[((*iterPlot).second).strName] = getSelOrderedPairs( (*iterPlot).second, treeInput);
                    
                    //Draw all plot labels requested by the user onto this canvas
                    /*for (auto iterLabels = vec_PlotLabels.begin(); iterLabels != vec_PlotLabels.end(); ++iterLabels) { //Loop over vec_PlotLabels
                        //Initialize the TLatex
                        TLatex *thisLatex = new TLatex();
                        
                        //Set the TName to be unique
                        thisLatex->SetName( ("thisLatex_" + (*iterPlot).second.strName ).c_str() );
                        
                        //Set the text size
                        thisLatex->SetTextSize(0.03);
                        
                        //Set the text angle
                        thisLatex->SetTextAngle( (*iterPlot).second.fPtLbls_Angle );
                        
                        //Draw
                        thisLatex->DrawLatex( getOffsetPosition(std::get<0>(*iterLabels), (*iterPlot).second.fPtLbls_Angle, true ), getOffsetPosition(std::get<1>(*iterLabels), (*iterPlot).second.fPtLbls_Angle, false ),( std::get<2>(*iterLabels) ).c_str() );
                    } //End Loop Over vec_PlotLabels */
                } //End Case: Draw Plot Labels
                
                //Add to the Legend
                ((*iterCanvas).second).leg->AddEntry(gCurrentPlot, (*iterPlot).second.strLegEntry.c_str(), "LPE");
                
                //Store the current plot
                dir_thisPlot->cd();
                gCurrentPlot->Write();
                
                //Close the Input Root File
                file_ROOT_Input->Close();
            } //End Loop Over (*iterCanvas).mapPlot
            
            //Plot the Multi Graph
            cCurrentCanvas->cd();   //It should already be the active canvas; but hey let's just be sure
            ((*iterCanvas).second).mGraph->Draw("a"); //Do this the 1st time to make TMultiGraph::GetHistogram() a valid pointer
            ((*iterCanvas).second).mGraph = getMultiGraph( (*iterCanvas).second );
            ((*iterCanvas).second).mGraph->Draw("a");
            
            //Plot the Legend
            cCurrentCanvas->cd();   //It should already be the active canvas; but hey let's just be sure
            ((*iterCanvas).second).leg->Draw("same");
            
            //Draw All TLatex's requested by the user onto this canvas
            for (int i=0; i < ((*iterCanvas).second).vec_strLatexLine.size(); ++i) { //Loop Over inputInfo.vec_strLatexLine
                //initialize the TLatex
                TLatex *thisLatex = new TLatex();
                
                //set the TName to be unique
                thisLatex->SetName( ("thisLatex_" + getString(i) ).c_str() );
                
                //set the text size
                thisLatex->SetTextSize(0.03);
                
                //Check to see if the user wants to print just the global selection
                //  e.g. what is stored in SelInfo::strSel
                //If so, set this element of to SelInfo::strSel
                if ( ((*iterCanvas).second).vec_strLatexLine[i].compare("GLOBAL_SELECTION") == 0) { //Case: User requests global selection written
                    ((*iterCanvas).second).vec_strLatexLine[i] = (*iterSel).strSel;
                } //End Case: User requests global selection written
                
                //Draw the latex line
                if (((*iterCanvas).second).vec_strLatexLine.size() != ((*iterCanvas).second).vec_fLatexNDCPos_Y.size()
                    || ((*iterCanvas).second).vec_strLatexLine.size() != ((*iterCanvas).second).vec_fLatexNDCPos_X.size() ) { //Case: Sizes unequal
                    cout<<"treeAnalyzer::getCanvas() - Number of TLatex lines: " << ((*iterCanvas).second).vec_strLatexLine.size() << endl;
                    cout<<"treeAnalyzer::getCanvas() - Does not match number of TLatex NDC coordinates\n";
                    cout<<"treeAnalyzer::getCanvas() - N_X = " << ((*iterCanvas).second).vec_fLatexNDCPos_X.size() << " N_Y = " << ((*iterCanvas).second).vec_fLatexNDCPos_Y.size() << endl;
                    cout<<"treeAnalyzer::getCanvas() - Using Default Assignment Algorithm!!!\n";
                    
                    //Default Algorithm uses NDC_X = 0.15 and then starts at NDC_Y = 0.85 and extends to NDC_Y = 0.15
                    thisLatex->DrawLatexNDC(0.15,0.85 - i*0.05,((*iterCanvas).second).vec_strLatexLine[i].c_str() );
                } //End Case: Sizes unequal
                else{ //Case: Sizes Equal
                    thisLatex->DrawLatexNDC(((*iterCanvas).second).vec_fLatexNDCPos_X[i],((*iterCanvas).second).vec_fLatexNDCPos_Y[i],((*iterCanvas).second).vec_strLatexLine[i].c_str() );
                } //Case: Position of TLatex Sizes Equal
            } //End Loop Over inputInfo.vec_strLatexLine
            
            //Store plot labels if requested
            if ( (*iterCanvas).second.mapPlotLabels.size() > 0 ) {
                //Draw all plot labels requested by the user onto this canvas
                for (auto iterPlotLabels = (*iterCanvas).second.mapPlotLabels.begin(); iterPlotLabels != (*iterCanvas).second.mapPlotLabels.end(); ++iterPlotLabels) { //Loop over map of "plots"
                    
                    for (auto iterLabels = (*iterPlotLabels).second.begin(); iterLabels != (*iterPlotLabels).second.end(); ++iterLabels) { //Loop Over vector of labels for plot iterPlotLabels
                        //Initialize the TLatex
                        TLatex *thisLatex = new TLatex();
                        
                        //Set the TName to be unique
                        thisLatex->SetName( ("thisLatex_" + (*iterPlotLabels).first ).c_str() );
                        
                        //Set the text size
                        thisLatex->SetTextSize(0.03);
                        
                        //Set the text angle
                        thisLatex->SetTextAngle( (*iterCanvas).second.mapPlot[(*iterPlotLabels).first].fPtLbls_Angle );
                        
                        //Draw
                        thisLatex->DrawLatex(
                            getOffsetPosition(std::get<0>(*iterLabels), (*iterCanvas).second.mapPlot[(*iterPlotLabels).first].fPtLbls_Angle, true ),
                            getOffsetPosition(std::get<1>(*iterLabels), (*iterCanvas).second.mapPlot[(*iterPlotLabels).first].fPtLbls_Angle, false ),
                            ( std::get<2>(*iterLabels) ).c_str()
                        );
                    } //End Loop Over vector of labels for plot iterPlotLabels
                } //End Loop Over map of "plots"
            } //End Case: Draw Plot Labels
            
            //Store the Canvas
            dir_thisCanvas->cd();
            cCurrentCanvas->Write();
            ((*iterCanvas).second).mGraph->Write();
        } //End Loop Over (*iterSel).mapCanvas
    } //End Loop Over vecSelInfo
    
    //Close the output root file
    file_ROOT_Output->Close();
    
    return;
} //End treeAnalyzer::analyzeRuns()

std::istream & treeAnalyzer::getlineNoSpaces(std::istream & stream, string & str){
    //get the line
    getline(stream, str);
    
    if (str.find("'",0) == std::string::npos ) { //Case: Header
        str.erase(remove(str.begin(),str.end(), ' ' ), str.end() ); //spaces
        str.erase(remove(str.begin(),str.end(), '\t' ), str.end() );//tabs
    } //End Case: Header
    else{ //Case: line input
        int iFirstQuote = str.find("'",0);
        int iLastQuote = str.rfind("'");
        
        //Spaces
        str.erase(remove(str.begin(),str.begin()+iFirstQuote, ' ' ), str.begin()+iFirstQuote ); //Until first single-quote
        str.erase(remove(str.begin()+iLastQuote,str.end(), ' ' ), str.end() ); //Until last single-quote
        
        //tabs
        str.erase(remove(str.begin(),str.begin()+iFirstQuote, '\t' ), str.begin()+iFirstQuote ); //Until first single-quote
        str.erase(remove(str.begin()+iLastQuote,str.end(), '\t' ), str.end() ); //Until last single-quote
    } //End Case: line input
    
    return stream;
} //End treeAnalyzer::getlineNoSpaces()


float treeAnalyzer::getOffsetPosition(float fPos, float fAngle, bool bXCord){
    if (bXCord) {
        if ( (0 <= getPosAngle(fAngle) && getPosAngle(fAngle) < 90 )
            || (270 <= getPosAngle(fAngle) && getPosAngle(fAngle) < 360 ) ) { //Case: Quadrants I && IV
            fPos+=0.02*fPos;
        } //End Case: Quadrants I && IV
        else{ //Case: Quadrants II && III
            fPos-=0.02*fPos;
        } //End Case: Quadrants II && III
    } //End Case: x-coordinate
    else{ //Case: y-coordinate
        if ( 0 <= getPosAngle(fAngle) && getPosAngle(fAngle) < 180 ) { //Case: Quadrants I && II
            fPos+=0.02*fPos;
        } //End Case: Quadrants I && II
        else{ //Case: Quadrants III && IV
            fPos-=0.02*fPos;
        } //End Case: Quadrants III && IV
    } //End Case: y-coordinate
    
    return fPos;
} //End treeAnalyzer::getOffsetPosition()

std::pair<string,string> treeAnalyzer::getParsedLine(string strInputLine, bool &bExitSuccess){
    //Variable Declaration
    int iPos_Equals = strInputLine.find("=",0);
    int iPos_End    = strInputLine.find(";",0);
    int iPos_Quote1 = strInputLine.find("'",0); //Position of first single-quote
    int iPos_Quote2 = strInputLine.rfind("'");  //Position of last single-quite
    
    string strFieldName;
    string strFieldValue;
    
    //Check to make sure iPos_Equals found in input string
    if (iPos_Equals == std::string::npos && strInputLine.find("[",0) == std::string::npos ) {
        cout<<"Character '=' Not Found in line:\n";
        cout<<strInputLine<<endl;
        cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_End found in input string
    if (iPos_End == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        cout<<"Character ';' Not Found in line:\n";
        cout<<strInputLine<<endl;
        cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_Quote1 found in input string
    if (iPos_Quote1 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        cout<<"First Single-Quote (e.g. ' ) Not Found in line:\n";
        cout<<strInputLine<<endl;
        cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Check to make sure iPos_Quote2 found in input string
    if (iPos_Quote2 == std::string::npos && strInputLine.find("[",0) == std::string::npos) {
        cout<<"Last Single-Quote (e.g. ' ) Not Found in line:\n";
        cout<<strInputLine<<endl;
        cout<<"Exiting treeProducer::getParsedLine(), Cross-Check Selcetion Setup File\n";
        
        bExitSuccess = false;
        
        return std::make_pair("","");
    }
    
    //Get the Strings
    strFieldName    = strInputLine.substr(0,iPos_Equals);
    //strFieldValue   = strInputLine.substr(iPos_Equals+1, iPos_End - iPos_Equals - 1);
    strFieldValue   = strInputLine.substr(iPos_Quote1+1, iPos_Quote2 - iPos_Quote1 - 1);
    
    //Set the Exit Flag
    bExitSuccess = true;
    
    //Return the Pair
    return std::make_pair(strFieldName,strFieldValue);
} //End treeAnalyzer::getParsedLine()

std::vector<std::tuple<float,float,string> > treeAnalyzer::getSelOrderedPairs(PlotInfo &inputInfo, TTree *treeInput){
    //Variable Declaration
    std::vector<std::tuple<float, float, string> > ret_vecOfLabels; //Ordered Pairs (x_pos,y_pos,label)
    
    float fVarDep;
    float fVarIndep;
    
    UserInputFormat labels;
    
    //Transform Input Data Type to Upper Case
    //------------------------------------------------------
    std::transform(inputInfo.strPtLbls_Type.begin(), inputInfo.strPtLbls_Type.end(), inputInfo.strPtLbls_Type.begin(), toupper);
    
    //Set the branch addresses
    //------------------------------------------------------
    treeInput->SetBranchAddress(inputInfo.strVarDepend.c_str(), &fVarDep);
    treeInput->SetBranchAddress(inputInfo.strVarIndep.c_str(),  &fVarIndep);
    
    if (inputInfo.strPtLbls_Type.compare("INT") == 0 ) { //Case: int
        treeInput->SetBranchAddress(inputInfo.strPtLbls_Brnch.c_str(), &labels.iInput);
    } //End Case: int
    else if (inputInfo.strPtLbls_Type.compare("FLOAT") == 0 ) { //Case: float
        treeInput->SetBranchAddress(inputInfo.strPtLbls_Brnch.c_str(), &labels.fInput);
    } //End Case: float
    else if (inputInfo.strPtLbls_Type.compare("STRING") == 0 ) { //Case: string
        treeInput->SetBranchAddress(inputInfo.strPtLbls_Brnch.c_str(), &labels.strInput );
    } //End Case: string
    else{ //Case: Unrecognized Data Type
        std::cout<<("treeAnalyzer::getPointLabels - Point Label Data Type: " + inputInfo.strPtLbls_Type + " Not Recognized!!!\n").c_str();
        std::cout<<"treeAnalyzer::getPointLabels - No Point Labels Will Be Added!!!\n";
        std::cout<<"treeAnalyzer::getPointLabels - Please Cross-Check Plot List File!!!\n";
        
        return ret_vecOfLabels;
    } //End Case: Unrecognized Data Type
    
    //Loop Over the Entries in treeInput that pass the selection
    //------------------------------------------------------
    treeInput->Draw( ">>listSelEvts", inputInfo.strSelLocal.c_str(), "entrylist" );
    
    //Get the Entry List
    TEntryList *listSelEvts = (TEntryList*) gDirectory->Get("listSelEvts");
    
    //Loop Over the events passing selection stored in listSelEvts
    for (int i=0; i < listSelEvts->GetN(); ++i) { //Loop Over listSelEvts
        treeInput->GetEntry( listSelEvts->GetEntry(i) );
        
        if (inputInfo.strPtLbls_Type.compare("INT") == 0 ) { //Case: int
            ret_vecOfLabels.push_back( std::make_tuple(fVarIndep, fVarDep, inputInfo.strPtLbls_Brnch + " = " + getString(labels.iInput) ) );
        } //End Case: int
        else if (inputInfo.strPtLbls_Type.compare("FLOAT") == 0 ) { //Case: float
            ret_vecOfLabels.push_back( std::make_tuple(fVarIndep, fVarDep, inputInfo.strPtLbls_Brnch + " = " + getString(labels.fInput) ) );
        } //End Case: float
        else if (inputInfo.strPtLbls_Type.compare("STRING") == 0 ) { //Case: string
            ret_vecOfLabels.push_back( std::make_tuple(fVarIndep, fVarDep, inputInfo.strPtLbls_Brnch + " = " + labels.strInput ) );
        } //End Case: string
    } //End Loop Over listSelEvts
    
    return ret_vecOfLabels;
} //End treeAnalyzer::getPointLabels()

TCanvas * treeAnalyzer::getCanvas(CanvasInfo inputInfo){
    //Variable Declaration
    TCanvas *ret_Canvas = new TCanvas(inputInfo.strName.c_str(), inputInfo.strTitle.c_str(), inputInfo.iSize_X, inputInfo.iSize_Y );
    
    //X-Axis Style
    ret_Canvas->SetGridx(inputInfo.bGrid_X);
    ret_Canvas->SetLogx(inputInfo.bLog_X);
    
    //Y-Axis Style
    ret_Canvas->SetGridy(inputInfo.bGrid_Y);
    ret_Canvas->SetLogy(inputInfo.bLog_Y);
    
    return ret_Canvas;
} //End treeAnalyzer::getCanvas()

TFile * treeAnalyzer::getFile(string strInputFile, string strInputOption, bool &bExitSuccess){
    //Open the Input Data File
    //------------------------------------------------------
    if (bVerbose_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeAnalyzer::getFile(): trying to open and read: " << strInputFile << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    //Declaration the File
    TFile * ret_File = new TFile(strInputFile.c_str(), strInputOption.c_str(), "", 1);
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if ( !ret_File->IsOpen() || ret_File->IsZombie() ) { //Case: File Failed To Open Successfully
        if (bVerbose_IO) {
            perror( ("treeAnalyzer::getFile(): error while opening file: " + strInputFile).c_str() );
            std::cout << "Input ROOT File Status:\n";
            std::cout << " IsOpen()=" << ret_File->IsOpen() << endl;
            std::cout << " IsZombie()=" << ret_File->IsZombie() << endl;
            //std::cout << "Skipping Plot: " << ((*iterPlot).second).strName << endl;
        }
        
        bExitSuccess = false;
    } //End Case: File Failed To Open Successfully
    else{ //Case: File Opened Successfully
        if (bVerbose_IO) {
            std::cout << ("treeAnalyzer::getFile(): file " + strInputFile + " opened successfully!").c_str() << endl;
            std::cout << "Input ROOT File Status:\n";
            std::cout << " IsOpen()=" << ret_File->IsOpen() << endl;
            std::cout << " IsZombie()=" << ret_File->IsZombie() << endl;
        }
        
        bExitSuccess = true;
    } //End Case: File Opened Successfully
    
    return ret_File;
} //End treeAnalyzer::getFile()

TGraphErrors * treeAnalyzer::getGraph(PlotInfo inputInfo, TGraphErrors *gInput){
    //Color
    gInput->SetLineColor(inputInfo.iColor);
    gInput->SetMarkerColor(inputInfo.iColor);
    
    //Style
    gInput->SetLineStyle(inputInfo.iStyleLine);
    gInput->SetMarkerStyle(inputInfo.iStyleMarker);
    
    //Size
    gInput->SetLineWidth(inputInfo.fSizeLine);
    gInput->SetMarkerSize(inputInfo.fSizeMarker);
    
    //TName
    gInput->SetName(inputInfo.strName.c_str() );
    
    //X-Axis
    if (inputInfo.strXAxis_Title.length() == 0) { //Case: Default X-Axis Title
        gInput->GetXaxis()->SetTitle( inputInfo.strVarIndep.c_str() );
    } //End Case: Default X-Axis Title
    else{ //Case: User specified X-Axis Title
        gInput->GetXaxis()->SetTitle( inputInfo.strXAxis_Title.c_str() );
    } //End Case: User specified X-Axis Title
    
    if (inputInfo.bXAxis_UserRange) {
        gInput->GetXaxis()->SetRangeUser(inputInfo.fXAxis_Min, inputInfo.fXAxis_Max);
    }
    
    //Y-Axis
    if (inputInfo.strYAxis_Title.length() == 0) { //Case: Default Y-Axis Title
        gInput->GetYaxis()->SetTitle( inputInfo.strVarDepend.c_str() );
    } //End Case: Default Y-Axis Title
    else{ //Case: User specified Y-Axis Title
        gInput->GetYaxis()->SetTitle( inputInfo.strYAxis_Title.c_str() );
    } //End Case: User specified Y-Axis Title
    
    if (inputInfo.bYAxis_UserRange) {
        gInput->GetYaxis()->SetRangeUser(inputInfo.fYAxis_Min, inputInfo.fYAxis_Max);
    }
    
    //Title
    gInput->SetTitle("");
    
    return gInput;
} //End treeAnalyzer::getGraph()

TH1F * treeAnalyzer::getHistogram(PlotInfo inputInfo, TH1F * hInput){
    //Color
    hInput->SetLineColor(kBlack);       //For now make the histogram black
    hInput->SetMarkerColor(kBlack);     //For now make the histogram black
    
    //Style
    hInput->SetLineStyle(inputInfo.iStyleLine);
    hInput->SetMarkerStyle(inputInfo.iStyleMarker);
    
    //Size
    hInput->SetLineWidth(inputInfo.fSizeLine);
    hInput->SetMarkerSize(inputInfo.fSizeMarker);
    
    //TName
    
    //X-Axis
    
    //Y-Axis
    
    //Title
    
    return hInput;
} //End treeAnalyzer::getHistogram()

TMultiGraph * treeAnalyzer::getMultiGraph(CanvasInfo inputInfo){
    //Variable Declaration
    bool bXAxis_UserRange;
    bool bYAxis_UserRange;
    
    float fXAxis_Min = std::numeric_limits<float>::max();
    float fXAxis_Max = std::numeric_limits<float>::min();
    
    float fYAxis_Min = std::numeric_limits<float>::max();
    float fYAxis_Max = std::numeric_limits<float>::min();
    
    //X-Axis
    //Sets the Axis Title to the Title of the First Plot in the Map
    if ( (*inputInfo.mapPlot.begin() ).second.strXAxis_Title.length() == 0) { //Case: Default X-Axis Title
        inputInfo.mGraph->GetHistogram()->GetXaxis()->SetTitle( (*inputInfo.mapPlot.begin() ).second.strVarIndep.c_str() );
    } //End Case: Default X-Axis Title
    else{ //Case: User specified X-Axis Title
        inputInfo.mGraph->GetHistogram()->GetXaxis()->SetTitle( (*inputInfo.mapPlot.begin() ).second.strXAxis_Title.c_str() );
    } //End Case: User specified X-Axis Title
    
    //Loops Through the Map to find if a user range is specified for the x-axis, applies the maximum possible range
    for (auto iterPlot = inputInfo.mapPlot.begin(); iterPlot != inputInfo.mapPlot.end(); ++iterPlot) { //Loop Over iterPlot
        if ( (*iterPlot).second.bXAxis_UserRange ) { //Case: User Requested X-Axis range
            bXAxis_UserRange = true;
            
            if ( (*iterPlot).second.fXAxis_Min < fXAxis_Min) fXAxis_Min = (*iterPlot).second.fXAxis_Min;
            if ( (*iterPlot).second.fXAxis_Max > fXAxis_Max) fXAxis_Max = (*iterPlot).second.fXAxis_Max;
        } //End Case: User Requested X-Axis range
    } //End Loop Over iterPlot
    
    if (bXAxis_UserRange) {
        inputInfo.mGraph->GetHistogram()->GetXaxis()->SetRangeUser(fXAxis_Min, fXAxis_Max);
    }
    
    //Y-Axis
    //Sets the Axis Title to the Title of the First Plot in the Map
    if ( (*inputInfo.mapPlot.begin() ).second.strYAxis_Title.length() == 0) { //Case: Default Y-Axis Title
        inputInfo.mGraph->GetHistogram()->GetYaxis()->SetTitle( (*inputInfo.mapPlot.begin() ).second.strVarDepend.c_str() );
    } //End Case: Default Y-Axis Title
    else{ //Case: User specified Y-Axis Title
        inputInfo.mGraph->GetHistogram()->GetYaxis()->SetTitle( (*inputInfo.mapPlot.begin() ).second.strYAxis_Title.c_str() );
    } //End Case: User specified Y-Axis Title
    
    //Loops Through the Map to find if a user range is specified for the x-axis, applies the maximum possible range
    for (auto iterPlot = inputInfo.mapPlot.begin(); iterPlot != inputInfo.mapPlot.end(); ++iterPlot) { //Loop Over iterPlot
        if ( (*iterPlot).second.bYAxis_UserRange ) { //Case: User Requested X-Axis range
            bYAxis_UserRange = true;
            
            if ( (*iterPlot).second.fYAxis_Min < fYAxis_Min) fYAxis_Min = (*iterPlot).second.fYAxis_Min;
            if ( (*iterPlot).second.fYAxis_Max > fYAxis_Max) fYAxis_Max = (*iterPlot).second.fYAxis_Max;
        } //End Case: User Requested X-Axis range
    } //End Loop Over iterPlot
    
    if (bYAxis_UserRange) {
        inputInfo.mGraph->GetHistogram()->GetYaxis()->SetRangeUser(fYAxis_Min, fYAxis_Max);
    }
    
    //Title
    inputInfo.mGraph->SetTitle("");
    
    return inputInfo.mGraph;
} //End treeAnalyzer::getMultiGraph()

bool treeAnalyzer::convert2bool(string str, bool &bExitSuccess){
    //convert to upper case
    transform(str.begin(), str.end(), str.begin(), toupper);
    
    //Empty String?
    if ( str.empty() ) {
        bExitSuccess = false;
        return false;
    }
    
    //Input recognized?
    if ( !(str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0
        || str.compare("F")==0 || str.compare("FALSE")==0 || str.compare("0")==0) ) {
        bExitSuccess = false;
        return false;
    }
    
    bExitSuccess = true;
    
    return (str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0);
} //End treeAnalyzer::convert2bool()

//Converts an input expression into a EColor value defined in Rtypes.h of ROOT
int treeAnalyzer::convert2Color(string str){
    //Variable Declaration
    int ret_Color = kBlack;
    
    int iPos_Operator = std::min(str.find("+",0),str.find("-",0) );
    
    //Three Cases
    if (str.find_first_not_of( "0123456789" ) == string::npos) { //Case: Only Numeric Input
        ret_Color = stoi(str);
    } //End Case: Only Numeric Input
    else if( iPos_Operator == string::npos ) { //Case: Only kColor Input
        //Transform to upper case to be case-insensitive
        transform(str.begin(), str.end(), str.begin(), toupper);
        
        //Evaluate Color
        if (str.compare("KWHITE") == 0) {       ret_Color = kWhite;}
        else if(str.compare("KBLACK") == 0) {   ret_Color = kBlack;}
        else if(str.compare("KGRAY") == 0) {    ret_Color = kGray;}
        else if(str.compare("KRED") == 0) {     ret_Color = kRed;}
        else if(str.compare("KGREEN") == 0) {   ret_Color = kGreen;}
        else if(str.compare("KBLUE") == 0) {    ret_Color = kBlue;}
        else if(str.compare("KYELLOW") == 0) {  ret_Color = kYellow;}
        else if(str.compare("KMAGENTA") == 0) { ret_Color = kMagenta;}
        else if(str.compare("KCYAN") == 0) {    ret_Color = kCyan;}
        else if(str.compare("KORANGE") == 0) {  ret_Color = kOrange;}
        else if(str.compare("KSPRING") == 0) {  ret_Color = kSpring;}
        else if(str.compare("KTEAL") == 0) {    ret_Color = kTeal;}
        else if(str.compare("KAZURE") == 0) {   ret_Color = kAzure;}
        else if(str.compare("KVIOLET") == 0) {  ret_Color = kViolet;}
        else if(str.compare("KPINK") == 0) {    ret_Color = kPink;}
        else{ //Case: Color Not Recognized
            cout<<"treeAnalyzer::convert2Color() - Input color "<< str << " not recognized; setting to kBlack!!!"<<endl;
            return ret_Color;
        } //End Case: Color Not Recognized
        
    } //End Case: Only kColor Input
    else{ //Case: Full Expression
        //Find Color
        //string strColor = str.substr(0,iPos_Operator-1);
        string strColor = str.substr(0,iPos_Operator);
        
        //Transform to upper case to be case-insensitive
        transform(strColor.begin(), strColor.end(), strColor.begin(), toupper);
        
        //Evaluate Color
        if (strColor.compare("KWHITE") == 0) {       ret_Color = kWhite;}
        else if(strColor.compare("KBLACK") == 0) {   ret_Color = kBlack;}
        else if(strColor.compare("KGRAY") == 0) {    ret_Color = kGray;}
        else if(strColor.compare("KRED") == 0) {     ret_Color = kRed;}
        else if(strColor.compare("KGREEN") == 0) {   ret_Color = kGreen;}
        else if(strColor.compare("KBLUE") == 0) {    ret_Color = kBlue;}
        else if(strColor.compare("KYELLOW") == 0) {  ret_Color = kYellow;}
        else if(strColor.compare("KMAGENTA") == 0) { ret_Color = kMagenta;}
        else if(strColor.compare("KCYAN") == 0) {    ret_Color = kCyan;}
        else if(strColor.compare("KORANGE") == 0) {  ret_Color = kOrange;}
        else if(strColor.compare("KSPRING") == 0) {  ret_Color = kSpring;}
        else if(strColor.compare("KTEAL") == 0) {    ret_Color = kTeal;}
        else if(strColor.compare("KAZURE") == 0) {   ret_Color = kAzure;}
        else if(strColor.compare("KVIOLET") == 0) {  ret_Color = kViolet;}
        else if(strColor.compare("KPINK") == 0) {    ret_Color = kPink;}
        else{ //Case: Color Not Recognized
            cout<<"treeAnalyzer::convert2Color() - Input color "<< strColor << " not recognized; setting to kBlack!!!"<<endl;
            return ret_Color;
        } //End Case: Color Not Recognized
        
        //Debugging
        //cout<<"ret_Color = " << ret_Color << endl;
        
        //Increment the kColor by the desired amount
        if( (str.substr(iPos_Operator,1)).compare("+") == 0 ){ //Case: Increase kColor
            //ret_Color=+stoi( str.substr( iPos_Operator+1, str.length()-iPos_Operator-1 ) );
            ret_Color = ret_Color + stoi( str.substr( iPos_Operator+1, str.length()-iPos_Operator-1 ) );
        } //End Case: Increase kColor
        else{ //Case: Decrease kColor
            //ret_Color=-stoi( str.substr( iPos_Operator+1, str.length()-iPos_Operator-1 ) );
            ret_Color = ret_Color - stoi( str.substr( iPos_Operator+1, str.length()-iPos_Operator-1 ) );
        } //End Case: Decrease kColor
        
        //Debugging
        //cout<<"ret_Color = " << ret_Color << endl;
    } //End Case: Full Expression
    
    return ret_Color;
} //End treeAnalyzer::convert2Color()

//Prints All Bit Flags for an input ifstream
void treeAnalyzer::printStreamStatus(ifstream &inputStream){
    std::cout << "Input File Stream Bit Status:\n";
    std::cout << " good()=" << inputStream.good() << endl;
    std::cout << " eof()=" << inputStream.eof() << endl;
    std::cout << " fail()=" << inputStream.fail() << endl;
    std::cout << " bad()=" << inputStream.bad() << endl;
    
    return;
} //End treeAnalyzer::printStreamStatus()

void treeAnalyzer::setParamCanvas(string strField, string strVal, CanvasInfo &inputInfo){
    //Variable Declaration
    bool bExitSuccess = false;
    
    if (strField.compare( "name" ) == 0){inputInfo.strName = strVal;}
    else if(strField.compare("log_x") == 0 ){ //Case: Logarithmic X-Axis
        //Convert value to upper case to be case insensitive
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), toupper );
        
        bool bVal = convert2bool(strVal,bExitSuccess);
        
        if (!bExitSuccess) { //Case: User input not recognized
            cout<<"treeAnalyzer::setParamSel() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamSel() - Only 'TRUE' or 'FALSE' input is accepted!!!\n";
        } //End Case: User input not recognized
        else{ //Case: Input Recognized
            inputInfo.bLog_X = bVal;
        } //End Case: Input Recognized
    } //End Case: Logarithmic X-Axis
    else if(strField.compare("log_y") == 0 ){ //Case: Logarithmic Y-Axis
        //Convert value to upper case to be case insensitive
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), toupper );
        
        bool bVal = convert2bool(strVal,bExitSuccess);
        
        if (!bExitSuccess) { //Case: User input not recognized
            cout<<"treeAnalyzer::setParamSel() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamSel() - Only 'TRUE' or 'FALSE' input is accepted!!!\n";
        } //End Case: User input not recognized
        else{ //Case: Input Recognized
            inputInfo.bLog_Y = bVal;
        } //End Case: Input Recognized
    } //End Case: Logarithmic Y-Axis
    else if(strField.compare("grid_x") == 0 ){ //Case: X-Grid Lines
        //Convert value to upper case to be case insensitive
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), toupper );
        
        bool bVal = convert2bool(strVal,bExitSuccess);
        
        if (!bExitSuccess) { //Case: User input not recognized
            cout<<"treeAnalyzer::setParamSel() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamSel() - Only 'TRUE' or 'FALSE' input is accepted!!!\n";
        } //End Case: User input not recognized
        else{ //Case: Input Recognized
            inputInfo.bGrid_X = bVal;
        } //End Case: Input Recognized
    } //End Case: X-Grid Lines
    else if(strField.compare("grid_y") == 0 ){ //Case: Y-Grid Lines
        //Convert value to upper case to be case insensitive
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), toupper );
        
        bool bVal = convert2bool(strVal,bExitSuccess);
        
        if (!bExitSuccess) { //Case: User input not recognized
            cout<<"treeAnalyzer::setParamSel() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamSel() - Only 'TRUE' or 'FALSE' input is accepted!!!\n";
        } //End Case: User input not recognized
        else{ //Case: Input Recognized
            inputInfo.bGrid_Y = bVal;
        } //End Case: Input Recognized
    } //End Case: Y-Grid Lines
    else if(strField.compare( "title" ) == 0 ){inputInfo.strTitle = strVal;}
    else if(strField.compare( "width" ) == 0 ){ //Case: Set Canvas Width
        if (strVal.find_first_not_of( "0123456789" ) == string::npos ) {
            inputInfo.iSize_X = std::stoi(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: Set Canvas Width
    else if(strField.compare( "height" ) == 0 ){ //Case: Set Canvas Height
        if (strVal.find_first_not_of( "0123456789" ) == string::npos ) {
            inputInfo.iSize_Y = std::stoi(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: Set Canvas Height
    else if(strField.compare("latex_line") == 0 ){ //Case: TLatex Lines
        inputInfo.vec_strLatexLine.push_back(strVal);
    } //End Case: TLatex Lines
    else if(strField.compare("latex_line_ndcpos_x") == 0 ){ //Case: TLatex Line X-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.vec_fLatexNDCPos_X.push_back( std::stof(strVal) );
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLatex Line X-Position
    else if(strField.compare("latex_line_ndcpos_y") == 0 ){ //Case: TLatex Line Y-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.vec_fLatexNDCPos_Y.push_back( std::stof(strVal) );
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLatex Line Y-Position
    else if(strField.compare("leg_ndcpos_x1") == 0 ){ //Case: TLegend Initial X-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.fLegNDCPos_X1 = std::stof(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLegend Initial X-Position
    else if(strField.compare("leg_ndcpos_x2") == 0 ){ //Case: TLegend Final X-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.fLegNDCPos_X2 = std::stof(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLegend Final X-Position
    else if(strField.compare("leg_ndcpos_y1") == 0 ){ //Case: TLegend Initial Y-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.fLegNDCPos_Y1 = std::stof(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLegend Initial Y-Position
    else if(strField.compare("leg_ndcpos_y2") == 0 ){ //Case: TLegend Final Y-Position
        if (strVal.find_first_not_of( "0123456789." ) == string::npos ) {
            inputInfo.fLegNDCPos_Y2 = std::stof(strVal);
        }
        else{
            cout<<"treeAnalyzer::setParamCanvas() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamCanvas() - Please Enter Only Numeric Data!!!\n";
        }
    } //End Case: TLegend Final Y-Position
    else{ //Case: Unrecgonized Field
        cout<<"treeAnalyzer::setParamCanvas() - Unrecognized field!!!\n";
        cout<<("treeAnalyzer::setParamCanvas() - " + strField + " = " + strVal ).c_str() << endl;
        cout<<"treeAnalyzer::setParamCanvas() - Please cross-check input!!!\n";
    } //End Case: Unrecgonized Field
    
    return;
} //End treeAnalyzer::setParamCanvas()

void treeAnalyzer::setParamPlot(string strField, string strVal, PlotInfo &inputInfo){
    
    if( strField.compare("color" ) == 0 ) {                 inputInfo.iColor = convert2Color(strVal);}
    else if( strField.compare("style_line") == 0 ){         inputInfo.iStyleLine = stoi(strVal);}
    else if( strField.compare("style_marker") == 0 ){       inputInfo.iStyleMarker = stoi(strVal);}
    else if( strField.compare("point_labels_angle") == 0){  inputInfo.fPtLbls_Angle = stof(strVal); inputInfo.bPointLabels = true; }
    else if( strField.compare("size_line") == 0 ){          inputInfo.fSizeLine = stof(strVal);}
    else if( strField.compare("size_marker") == 0 ){        inputInfo.fSizeMarker = stof(strVal);}
    else if( strField.compare("x_range_min") == 0 ){        inputInfo.fXAxis_Min = stof(strVal);    inputInfo.bXAxis_UserRange = true;}
    else if( strField.compare("x_range_max") == 0 ){        inputInfo.fXAxis_Max = stof(strVal);    inputInfo.bXAxis_UserRange = true;}
    else if( strField.compare("y_range_min") == 0 ){        inputInfo.fYAxis_Min = stof(strVal);    inputInfo.bYAxis_UserRange = true;}
    else if( strField.compare("y_range_max") == 0 ){        inputInfo.fYAxis_Max = stof(strVal);    inputInfo.bYAxis_UserRange = true;}
    else if( strField.compare("fit_formula") == 0 ){        inputInfo.strFitFormula = strVal;}
    else if( strField.compare("var_dep_assoc") == 0 ){      inputInfo.strFriendBranch = strVal;}
    else if( strField.compare("leg_entry") == 0 ){          inputInfo.strLegEntry = strVal;}
    else if( strField.compare("name") == 0 ) {              inputInfo.strName = strVal;}
    else if( strField.compare("name_root_file") == 0 ) {    inputInfo.strNameROOTFile = strVal;}
    else if( strField.compare("name_tree") == 0 ) {         inputInfo.strNameTree = strVal;}
    else if( strField.compare("option_draw") == 0 ){        inputInfo.strOptionDraw = strVal;}
    else if( strField.compare("option_fit") == 0 ){         inputInfo.strOptionFit = strVal;}
    else if( strField.compare("point_labels") == 0 ){       inputInfo.strPtLbls_Brnch = strVal;inputInfo.bPointLabels = true;}
    else if( strField.compare("point_labels_type") == 0 ){  inputInfo.strPtLbls_Type = strVal;inputInfo.bPointLabels = true;}
    else if( strField.compare("selection" ) == 0 ) {        inputInfo.strSelLocal = strVal;}
    else if( strField.compare("var_indep" ) == 0 ) {        inputInfo.strVarIndep = strVal;}
    else if( strField.compare("var_indep_err" ) == 0 ) {    inputInfo.strVarIndepErr = strVal;}
    else if( strField.compare("var_dep" ) == 0 ) {          inputInfo.strVarDepend = strVal;}
    else if( strField.compare("var_dep_err" ) == 0 ) {      inputInfo.strVarDependErr = strVal;}
    else if( strField.compare("x_title") == 0 ){            inputInfo.strXAxis_Title = strVal;}
    else if( strField.compare("y_title") == 0 ){            inputInfo.strYAxis_Title = strVal;}
    else{ //Case: Unrecgonized Field
        cout<<"treeAnalyzer::setParamPlot() - Unrecognized field!!!\n";
        cout<<("treeAnalyzer::setParamPlot() - " + strField + " = " + strVal ).c_str() << endl;
        cout<<"treeAnalyzer::setParamPlot() - Please cross-check input!!!\n";
    } //End Case: Unrecgonized Field
    
    return;
} //End treeAnalyzer::setParamPlot()

void treeAnalyzer::setParamSel(string strField, string strVal, SelInfo &inputInfo){
    //Variable Declaration
    bool bExitSuccess = false;
    
    if (        strField.compare("identifier") == 0 ) { inputInfo.strIdent  = strVal;}
    else if(    strField.compare("selection") == 0 ) {  inputInfo.strSel    = strVal;}
    else if(    strField.compare("clone_prev") == 0 ){
        //Convert value to upper case to be case insensitive
        std::transform(strVal.begin(), strVal.end(), strVal.begin(), toupper );
        
        bool bVal = convert2bool(strVal,bExitSuccess);
        
        if (bVal && bExitSuccess && vecSelInfo.size() > 0 ) { //Case: Clone Previous && Previous Entry Exists
            //It is assumed that SelInfo::strIdent && Selinfo::strSel have already been set
            //The previously set identifier and selection for the i^th SelInfo are stored in a temp container
            //The i^th SelInfo is set to equal the (i-1)^th SelInfo, resetting the identifier and selection in the process
            //The identifier and selection of the (i-1)^th SelInfo are over-riddent by the current values from the temp container
            //This is implemented in this way in case additional parameters are added to SelInfo
            
            //Store the input sel info
            SelInfo tempSelInfo = inputInfo;
            
            //Store the input sel info as the last stored sel info
            inputInfo = vecSelInfo.back();
            
            //Restore the current selection and identifier
            inputInfo.strIdent  = tempSelInfo.strIdent;
            inputInfo.strSel    = tempSelInfo.strSel;
        } //End Case: Clone Previous && Previous Entry Exists
        else if( bVal && bExitSuccess && vecSelInfo.size() == 0  ){ //Case: Previous Does Not Exist
            cout<<"treeAnalyzer::setParamSel() - Request to Clone Previous Selection Failed, Previous Selection Does Not Exist!!!\n";
        } //End Case: Previous Does Not Exist
        else if( !bExitSuccess ){ //Case: User Input Not Recognized
            cout<<"treeAnalyzer::setParamSel() - User input " << strVal << " for field " << strField << " Not Recognized!!!\n";
            cout<<"treeAnalyzer::setParamSel() - Only 'TRUE' or 'FALSE' input is accepted!!!\n";
        } //End Case: User Input Not Recognized
    } //End Case: Clone Previous
    else{ //Case: Unrecgonized Field
        cout<<"treeAnalyzer::setParamSel() - Unrecognized field!!!\n";
        cout<<("treeAnalyzer::setParamSel() - " + strField + " = " + strVal ).c_str() << endl;
        cout<<"treeAnalyzer::setParamSel() - Please cross-check input!!!\n";
    } //End Case: Unrecgonized Field
        
    return;
} //End treeAnalyzer::setParamSel()