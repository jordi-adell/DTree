/*--------------------------------------------------------------------------------
    DTree Is a Decision Tree which can do Classification and also Regression.
    Copyright (C) 2005 - Jordi Adell - TALP / UPC
-----------------------------------------------------------------------------------
                    GNU GENERAL PUBLIC LICENSE
		        Version 3, 29 June 2007

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA  02110-1301, USA
-------------------------------------------------------------------------------*/


#include "DTree.h"

void disclaimer(const std::string &prog)
{
  cout << prog << " Copyright (C) 2005 - Jordi Adell" << endl;
  cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions." << endl;
}


void usage(string prog){
  disclaimer(prog);
  cout << endl;
  cout << "--------------------------------" << endl;
  cout << prog << "(" << DTREE_VERSION << ") evaluating a decision tree" << endl;
  cout << "--------------------------------" << endl;
  cout << prog << " [options]" << endl;
  cout << "\t-d Data file" << endl 
       << "\t-c Description file" << endl 
       << "\t-t Input tree file" << endl
       << "\t-o Output file" << endl
       << "\t-p Predictee fields (def: 0)" << endl 
       << "\t-i Information file, with results" << endl
       << "\t-v Verbose predictions on stdout" << endl
       << "\t-h Print this help" << endl
       << endl;
}
    


// -- Main --------------------
int main(int argc, const char **argv){

  string datafile, descfile, treefile, outfile, spredictee, infofile;
  int predictee=0,  ndata;
  vector<bool> l;
  bool verbose=false;
  
  if (argc < 6 ){
    cerr << "Wrong number of arguments" << endl;
    usage(string(argv[0]));
    exit(-1);
  }
    
  
  // Parsing the command line
  for (int i=1; i<argc; i++){
    if (argv[i][0] == '-')
      switch (argv[i][1]){
      case 'd':
	datafile=string(argv[++i]);
	break;
      case 'c':
	descfile=string(argv[++i]);
	break;
      case 'p':
	spredictee = string(argv[++i]);
	break;
      case 't':
	treefile = string(argv[++i]);
	break;
      case 'o':
	outfile = string(argv[++i]);
	break;
      case 'i':
	infofile = string(argv[++i]);
	break;
      case 'v':
	verbose = true;
	break;
      case 'h':
	usage(string(argv[0]));
	exit(0);
	break;
      default:
	cerr << "Wrong argument " << argv[i] << endl;
	exit (-1);
      }
    else{
      cerr << "Wrong argument " << argv[i] << endl;
      usage(string(argv[0]));
      exit(-1);
    }
  }

  if (datafile.empty() || descfile.empty() || treefile.empty()){
    cerr << "Data, Description and Tree files must be provided" << endl;
    usage(string(argv[0]));
    exit (-1);
  }

  DTree *tree = new DTree(predictee,0,1,1);
  ndata = tree->dataLoad(datafile,descfile);
 
  if (ndata == -1){
    cerr << "ERROR: Loading data from " << datafile << " or  description file" << descfile << endl;
    exit(-1);
  }
  l.resize(ndata,true);
  if (!tree->loadTree(treefile)){
    cerr << "[DTest]: Error Loading tree file" << endl;
    exit(0);
  }

  if (!spredictee.empty())
    tree->setPredictee(spredictee);
  
  if (!outfile.empty())
    tree->writeAnswers(outfile, verbose);

  if (!infofile.empty()){
    ofstream ofs (infofile.c_str(),ios::app);
    if (ofs.good())
      tree->dumpStatistics(ofs,true);
    else
      cerr << "[ERROR]: problems opening " << infofile;
    ofs.close();
  }

  cout << endl 
       << "------------------------" << endl
       << "DTree (" << DTREE_VERSION << ") Testing algorithm" << endl
       << "----------------------- " << endl
       << "Predicting " << tree->getPredictee() << endl
       << "Dataset entropy: " << tree->entropy(l) << endl;
    
  tree->dumpStatistics();
  cout << "=======================" << endl;
  
  delete tree;
}


