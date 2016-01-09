/*--------------------------------------------------------------------------------
    DTrain is based on DTree. It creates a Tree using DTree functions.
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
  cout << prog <<  "("<< DTREE_VERSION  << "), training a decision tree" << endl;
  cout << "--------------------------------" << endl;
  cout << prog << " [options]" << endl;
  cout << "\t-d Data file" << endl 
       << "\t-c Description file" << endl 
       << "\t-t Output tree file" << endl
       << "\t-p Predictee field name (def: 0)" << endl 
       << "\t-r Number of section for float fields (def: 10)" << endl 
       << "\t-b Balance, minimum percentage of elements in a node (def: 0.1)" << endl 
       << "\t-s Minumum number of elements in a leaf (stop criteria) (def: 50)" << endl 
       << "\t-e Errortype. Options are: entropy, rmse, correlation, correct" << endl
       << "\t-i Information file, with results" << endl
       << "\t-n N fold cross-validation (def:1)"  << endl
       << "\t-v Print tree on screen" << endl
       << "\t-h Print this help" << endl
       << endl;
}

// -- Main --------------------
int main(int argc, const char **argv){

  string datafile="", descfile="", treefile, option, spredictee, infofile;
  int predictee=0, minelements=50, section=10, ndata, folds=1;
  errType errortype=ENTROPY;
  float balance=0.0;
  bool printtree=false, leaveoneout=false;
  DTree tree;
  

  /*  cout   << "int       " << sizeof(int) << " " << numeric_limits<int>::max() << endl;
  cout   << "short int " << sizeof(short int) << " " << numeric_limits<short int>::max() << endl;
  cout   << "long int  " << sizeof(long int) << " " << numeric_limits<long int>::max() << endl;
  cout   << "float     " << sizeof(float) << " " << numeric_limits<float>::max() << endl;
  cout   << "double    " << sizeof(double) << " " << numeric_limits<double>::max() << endl;
  cout   << "long double " << sizeof(double) << " " << numeric_limits<double>::max() << endl;

  exit(0);
  */
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
      case 'b':
	balance = atof(argv[++i]);
	break;
      case 'r':
	section = atoi(argv[++i]);
	break;
      case 's':
	minelements = atoi(argv[++i]);
	break;
      case 't':
	treefile = string(argv[++i]);
	break;
      case 'i':
	infofile = string(argv[++i]);
	break;
      case 'v':
	printtree=true;
	break;
      case 'e':
	option = string(argv[++i]);
	if (option == "entropy")
	  errortype = ENTROPY;
	else if(option == "rmse")
	  errortype = RMSE;
	else if(option == "correct")
	  errortype = CORRECT;
	else if(option == "correlation")
	  errortype = CORRELATION;
	else
	  cerr << "WARNING!: Wrong type of optimization function. Entropy will be used  " << endl;
	
	break;
      case 'n':
	folds = atoi(argv[++i]);
	break;
      case 'l':
	leaveoneout = true;
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

  if (argc < 6){
    cerr << "Wrong number of arguments" << endl;
    usage(string(argv[0]));
    exit(-1);
  }


  if (datafile.empty() || descfile.empty()){
    cerr << "Data and Description files must be provided" << endl;
    usage(string(argv[0]));
    exit (-1);
  }

  tree = DTree(predictee,balance,minelements,section,errortype);
  ndata = tree.dataLoad(datafile,descfile);
  if (leaveoneout) folds=ndata;

  vector<float> values;

  if (!spredictee.empty())
    tree.setPredictee(spredictee);

  cout << "------------------------" << endl
       << "DTree Training algorithm" << endl
       << "----------------------- " << endl;

  if (ndata <= 0){
    cerr << "ERROR: Loading data from " << datafile << " or  description file" << descfile << endl;
    exit(-1);
  }else
    cerr << ndata << " vectors loaded"  << endl;
  

  for (unsigned int i=0; i<folds; i++){
    if (folds <= 1){
      if(!tree.train()){
	cerr << "Problems on training " << endl;
	exit (-1);
      }
    }else{
      if(!tree.trainNfold((unsigned)folds,i)){
	cerr << "Problems on training " << endl;
	exit (-1);
      }
    }
    
    if (printtree)
      tree.dumpTree();
    
    if (!infofile.empty()){
      ofstream ofs(infofile.c_str(),ios::app);
      if (ofs.good())
	tree.dumpStatistics(ofs,true);      
      else
	cerr << "[ERROR]: Unable to open " << infofile << endl;
      ofs.close();
    }
    values.push_back(tree.dumpStatistics());      

    if (!treefile.empty())
      tree.saveTree(treefile);
    cout << "=======================" << endl;

  }
  if (folds > 1 && !values.empty()){
    float cumsum=0;
    
    size_t i=0;
    for (vector<float>::iterator it=values.begin(); it != values.end(); it++, i++){
      cumsum += *it;
      cout << i << "\t" << *it << endl;
    }
    cout << "MEAN:\t" << cumsum/values.size() << endl;
    cout << "------------------------" << endl;
  }


}


