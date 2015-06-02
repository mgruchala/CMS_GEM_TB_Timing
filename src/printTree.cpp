//
//  printTree.C
//  
//
//  Created by Brian L Dorney on 24/04/15.
//
//


//C++ Includes
#include <stdio.h>
#include <string>

//ROOT Includes
#include "TFile.h"
#include "TTree.h"

//My Includes
#include "treeProducerTDC.cc"

int main(){
	//Variable Declaration	
	treeProducerTDC printer;

	TFile *file_GE11_IV	= new TFile("TestBeamDataset_TDC_Fall2014_H2_GE11_IV.root",	"READ","",1);
	TFile *file_GE11_IV_GIF	= new TFile("TestBeamDataset_TDC_Fall2014_H2_GE11_IV_GIF.root",	"READ","",1);
	TFile *file_GE11_V	= new TFile("TestBeamDataset_TDC_Fall2014_H2_GE11_V.root",	"READ","",1); 

	TTree *tree_GE11_IV	= (TTree*) file_GE11_IV->Get("GE11_IV");
	TTree *tree_GE11_IV_GIF	= (TTree*) file_GE11_IV_GIF->Get("GE11_IV_GIF");
	TTree *tree_GE11_V	= (TTree*) file_GE11_V->Get("GE11_V");

	int iTree;
	cout<<"Print Which Tree? (1 = GE1/1-IV; 2 = GE1/1-IV-GIF; 3 = GE1/1-V)\n";
	cin>>iTree;
	
	switch(iTree){
		case 1:	printer.printStoredData(tree_GE11_IV);		break;
		case 2: printer.printStoredData(tree_GE11_IV_GIF);	break;
		case 3: printer.printStoredData(tree_GE11_V);		break;
		default:
			cout<<"Option Not Recognized; Exitting!!!\n";
			break;
	}

	return 0;
}
