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
  cout <<         "-----------------------------------------" << endl;
  cout << prog << " Print the tree nicely on screen (" << DTREE_VERSION <<  ")" << endl;
  cout <<         "-----------------------------------------" << endl;
  cout << "\t-t\tTree file" << endl
       << "\t-c\tDescription file" << endl << endl
       << "Optional:" << endl
       << "\t-d\tDatafile" << endl
       << "\t-q\tDump question list" << endl
       << "\t-o\tPrint the tree in a new file (integrity check)" << endl << endl;
}
    


// -- Main --------------------
int main(int argc, const char **argv){

  string treefile, datafile, descfile, newtreefile;
  bool upct = false;
  bool questions=false;
  
  if (argc < 4 ){
    cerr << "Wrong number of arguments" << endl;
    usage(string(argv[0]));
    exit(-1);
  }
  
  // Parsing the command line
  for (int i=1; i<argc; i++){
    if (argv[i][0] == '-')
      switch (argv[i][1]){
      case 't':
	treefile = string(argv[++i]);
	break;
      case 'c':
	descfile = string(argv[++i]);
	break;
      case 'd':
	datafile = string(argv[++i]);
	break;
      case 'u':
	upct = true;
	break;
      case 'q':
	questions=true;
	break;
      case 'o':
	newtreefile = string(argv[++i]);
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

  if (treefile.empty()){
    usage(string(argv[0]));
    cerr << "Tree file must be provided" << endl;
    exit (-1);
  }

  DTree tree(0,0,1,1);
  tree.descriptionLoad(descfile);

  
  if (tree.loadTree(treefile)){
    if (upct)
      tree.saveUPCt(treefile+".upct",0);
    cout << ";; Tree Generate with DTree" << endl;
    cout << ";; Predictee is " << tree.getPredictee() << endl;
    cout << ";; " << treefile << endl;
    cout << ";;" << endl;

    if (!datafile.empty()){
      tree.databaseLoad(datafile);
      tree.setStatistics();
      if (questions){
	tree.dumpQuestions();
      }
    }
    tree.dumpTree();
    
    if (!newtreefile.empty())
      tree.saveTree(newtreefile,0);
      
    //    tree.dumpStatistics();
    //    tree.dumpCodification();

  }else
    cerr << "ERROR loading Tree" << endl;
}



