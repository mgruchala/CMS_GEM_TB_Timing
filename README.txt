# CMS_GEM_TB_Timing
# ========================================================
	Software tools for the analysis of timing data collected at CMS GEM test beams

# Installation Instructions
# ========================================================
	Navigate to the scripts directory:

		cd scripts/

	Run the setup_CMS_GEM.sh script:

		source setup_CMS_GEM.csh	#(csh)
		source setup_CMS_GEM.sh		#(bash)

	Navigate to the source directory:

		cd ../src/

	Open makefile_analyzer, ensure the compiler is correct for your system

		i.e. CC = g++ or clang (for Mac OS > 10.9 ???)

	Perform the same check for makefile_producer

	Build the project:

		make -f makefile_analyzer	#(Tree Analyzer)
		make -f makefile_producer	#(Tree Producer, experts only)

	This will create two executables "analyzeTree" and "produceTree"
	
# Usage - analyzeTree
# ========================================================

	On new shell execute the setup script:

		source scripts/setup_CMS_GEM.csh	#(csh)
		source scripts/setup_CMS_GEM.sh		#(bash)

	To run the analysis:

		./src/analyzeTree <plot_list_file>.txt

	Several sample <plot_list_files>.txt have been included in this repository, for example:

		./src/analyzeTree lists_plots/PlotList_GE11_IV_iMSPL_iT.txt

	will generate an output file "PlotList_GE11_IV_iMSPL_iT.root" containing the prepared results.

	Please see "lists_plots/PlotList_Template.txt" for an explanation of the expected input and available options

# Usage - produceTree
# ========================================================
	
	EXPERTS ONLY

	Call:

		./src/produceTree -h

	More coming soon...
