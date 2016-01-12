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

#ifndef _DTREE_H_
#define _DTREE_H_


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <math.h>
#include <vector>
#include <set>
#include <stdio.h>
#include <limits>
#include "enumcode.h"

/// The percentage of sections with mean-sigma and mean-sigma 
/// in the question building process
#define CTRSEC 0.7// center section
#define MAXLINELENGTH 2048
#define INF 100000000
#define PI 3.14159265

#ifdef PACKAGE_VERSION
#define DTREE_VERSION PACKAGE_VERSION
#else
#define DTREE_VERSION 1.5
#endif

#ifndef SOFT_VERSION
#define SOFT_VERSION 
#endif 


/// This defines the format to name field type 
/// in the description file                    
#define ENUM "enum"
#define FLOAT "float"

/// The list of possible opimization functions
typedef enum{ENTROPY, CORRELATION, CORRECT, RMSE} errType;

using namespace std;

/// The class that does all the job
/// It contains the data, the tree, the questions, etc...
class DTree {

  public:
  // --------------------
  /// Question and Sparse Tree are internal classes
#include "Question.h"
#include "SparseTree.h"
  // -----------------------------

  typedef vector<float> dataElement;
  typedef vector<vector<float > > TData;
  typedef pair<vector<bool>,vector<bool> > Leaf; /// elements & questions
  typedef pair<short int, int > FieldDef; /// type, fieldnumber for each fiels
  
  // functions to work with DTree.
  int dataLoad(string datafile, string descfile);
  int descriptionLoad (string descfile);
  int databaseLoad (string datafile);
  bool addElement(dataElement element);
  bool addElement(vector<string> element);
  void clearData(){data.clear();}
  bool trainNfold(unsigned int folds, unsigned int N);
  bool train();
  bool train(Leaf l);
  bool setStatistics();
  float entropy(vector<bool> l);  
  bool writeAnswers(string outfile,bool onscreen);
  bool isLoaded();

  // io.cpp
  bool saveTree(ofstream &fout,double node=0);
  bool saveTree(string treefile,double node=0);
  bool saveUPCt(ofstream &fout,double node=0);
  bool saveUPCt(string treefile,double node=0);
  bool loadTree(ifstream &fin,double node=0);
  bool loadTree(string treefile,double node=0);
  float  getAnswer(vector<float > element);
  string getAnswer(vector<string> element);
  string getStringAnswer(vector<float > element);
  float  getFloatAnswer (vector<string> element);
  float getAnswerProb(vector<string> element);
  float getAnswerProb(vector<float> element);
  
  // dump.cpp   // dumping functions, for debuging and info.
  void dumpTree (double begin=0,string tab="");
  void dumpData (ofstream &ofs);
  float dumpStatistics(ostream &os, bool simple=false);
  float dumpStatistics(){return (dumpStatistics(cout));}
  void dumpCodification();
  void dumpQuestions();

  
  string getFieldName(int i){return fieldsName.at(i);}
  int getFieldId(string s){return numFieldsName[s];}
  vector <string> getFieldsName(){return fieldsName;}
  short int getFieldType(string s){return getFieldType(numFieldsName[s]);}
  short int getFieldType(int i){return fieldsType.at(i).first;}

  // optimization.cpp
  float correct(vector<bool> l);
  float rmse(vector<bool> l);
  float correlation(vector<bool> l);
  float probability(vector<bool> l);

  // Configuration Functions
  int setPredictee(int p);
  int setPredictee(string p);
  int setMinElements(unsigned int mine){minelements=mine; return 0;}
  int setBalance(float b){balance=b; return 0;}
  int setErrorType(errType e){errortype=e; return 0;}
  string getPredictee();


  // Ask for a value
  string predictValue(vector<float> &element);
  string predictValue(vector<string> &element);


 private:

  // Different type of functions to minimize
  float optimizationFunction(vector<bool> l);

  // variable members
  vector<dataElement>  data;
  SparseTree tree;
  EnumCode code;
  vector <FieldDef> fieldsType;
  vector <string> fieldsName;
  map <string,int> numFieldsName;
  set <Question> Qlist;
  pair<float,int> progress;

  //Main functions
  bool split(Leaf l, SparseTree& tree, double nleaf);
  size_t buildQuestionList(unsigned int sections, set<DTree::Question> &Qlist);
  map<float,float> calculateLeafValue(Leaf l);
  double predictLeaf (vector<float> &element, double node=0);
  float nodeValue(vector<bool> &l);

  // stop criterias
  bool stop(int nq, Leaf &l);
  bool stop(Leaf &l);
  bool stopQ(vector<bool> &qs);
  bool stopE(int nq,vector<bool> &elements);

  // Checking errors before training
  bool errorcheck();
  void error(string msg, int value=-1);
  void warn(string msg);


  // parameters of the algorithm
  short int predictee;
  unsigned int minelements;
  float balance;  
  unsigned int section; // step for float range spliting
  errType errortype;

  // progressbar
  void progressbar_init(int n);
  int progressbar(int n);
  void progressbar_add(int n);

 public:

  const static short int ENU=0;
  const static short int FLT=1;
  const static short int INT=2;
  const static short int LEAF=-1;
  const static short int NONE=-2;


  DTree(){
    predictee = 0;
    balance = 0.1;
    minelements = 50;
    section = 10;
    errortype = ENTROPY;
  }

  DTree(int p,float b,int mine,int s, errType e=ENTROPY){
    predictee = p;
    balance = b;
    minelements = mine;
    section = s;
    errortype=e;
  }
  
  DTree(string p,float b,int mine,int s, errType e=ENTROPY){
    predictee = numFieldsName[p];
    balance = b;
    minelements = mine;
    section = s;
    errortype=e;
  }

  DTree(string dscfile, string treefile, string datafile){
    dataLoad(datafile, dscfile);
    loadTree(treefile);
  }
  DTree(string dscfile, string treefile){
    if (!descriptionLoad(dscfile))
      loadTree(treefile);
  }

  ~DTree(){
    numFieldsName.clear();
  }

};


#endif
