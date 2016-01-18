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


/// -------------

#include "DTree.h"

/// Builds the list of all the possible questions given that data loaded.
size_t DTree::buildQuestionList(unsigned int sections, set<DTree::Question> &Qlist){

  //  set <DTree::Question> Qlist;
  set <DTree::Question>::iterator it_q;
  vector <FieldDef>::iterator it_type;
  vector <dataElement>::iterator it_data;
  vector <float> max(fieldsType.size(),-INF);
  vector <float> min(fieldsType.size(),INF);
  vector <float> mean(fieldsType.size(),0);
  vector <float> mean2(fieldsType.size(),0);
  vector <float> sigma(fieldsType.size(),0);
  vector<set<float> > ordered_values(fieldsType.size());
  DTree::Question q;
  float value;
  int delta;

  Qlist.clear();
  
  // Looking for enum question and calculating values
  // for float fields
  for (it_type=fieldsType.begin(); it_type != fieldsType.end(); it_type++)
    if (it_type->second != predictee)
      for (it_data=data.begin(); it_data != data.end(); it_data++){
	if (it_type->first == (int)DTree::ENU){

           q=Question(DTree::ENU, it_data->at(it_type->second), it_type->second);
          Qlist.insert(q);

	}else if (it_type->first == (int)DTree::FLT){
           ordered_values[it_type->second].insert(it_data->at(it_type->second));
	}else
	  warn("Wrong field format");
      }
  
  // Generating float questions.
  for (it_type=fieldsType.begin(); it_type != fieldsType.end(); it_type++)
    if (it_type->second != predictee)     
      if (it_type->first == (int) DTree::FLT){
	if (sections >= ordered_values[it_type->second].size() || sections <= 0)
	  delta = 1;
	else
	  delta = (int) (ordered_values[it_type->second].size() / sections);
	
	set<float>::iterator it_ord = ordered_values[it_type->second].begin(); 
	for (unsigned int i=delta;it_ord != ordered_values[it_type->second].end(); it_ord ++, i++){ //= delta){
	  if (i%delta == 0 && i < ordered_values[it_type->second].size()){
	    value = (*it_ord + *(++it_ord))/2;
	    it_ord--;
	    q=Question(DTree::FLT, value, it_type->second);
	    if (Qlist.find(q)!=Qlist.end()){
	      error("Trying to add an existing question",-1);
	    }
	    Qlist.insert(q);
	  }
	}
      }
  return Qlist.size();
}




/// \brief Says true if the node has number of elements smaller than a given value.
inline bool DTree::stop(Leaf &l){
  return stop(minelements,l);
}

bool DTree::stop(int nq, Leaf& l){
  if (tree.full()){
    warn("Tree has reached is maximum capacity");
    return true;
  }
  if (stopE(nq,l.first))
    return true;
  if (stopQ(l.second))
    return true;
  return false;
}


/// \brief Return answer to Stop? given the number of elements
inline bool DTree::stopE(int nq,vector<bool> &elements){
  int count=0;
  vector<bool>::iterator it_el;  
  
  for (it_el = elements.begin(); it_el != elements.end(); it_el ++){
    if (*it_el)
      count++;
  }
  if (count > nq)
    return false;
  return true;
}

/// Returns true if All questions used.
inline bool DTree::stopQ(vector<bool> &qs){

  vector<bool>::iterator it_q;
  for (it_q = qs.begin(); it_q != qs.end(); it_q++){
    if (*it_q)
	  return false;
  }
  return true;
}



/// \brief Autorrecursive splitting function

/// This is the core of the tree.
/// It splits recursively all the nodes, and uses previous functions to
/// calculate entropy ask for stopping critirie and splits nodes according
/// to the list of possible questions.
/// The criteria used to choose the best question is:
///    \f$argmin_{question}($H_{yes} + H_{no})/2\f$

bool DTree::split(Leaf l, SparseTree& tree, double nleaf){

  // boolean question -> bq
  // bolean element   -> be
  // choosen question -> choosen

  vector<bool> newleafyes, newleafno, tmpnewleafyes, tmpnewleafno, newq; 
  vector<bool>::iterator it_bq, it_be, it_nbe, it_bchosen;
  set<Question>::iterator it_q, it_chosen;
  vector<dataElement>::iterator it_data;
  float ey=INF,en=INF,minex=INF; //optimizationFunction(l.first);
  Leaf newl;
  unsigned int yesnumel=0, nonumel=0, actualnumelements=0;

  // Count number of elements in the present node
  for (it_be=l.first.begin(); it_be != l.first.end(); it_be++)
    if (*it_be)
      actualnumelements++;

  // optimization value to be improved.
  minex = optimizationFunction(l.first)*actualnumelements;

  //To mark whether no questions has been chosen
  it_bchosen = l.second.end();

  // Do we have to stop here?
  if (stop(l)){
     Question q(LEAF,calculateLeafValue(l),predictee,actualnumelements,entropy(l.first));
     tree.add(q,nleaf);
     progressbar_add(actualnumelements);
     return true;
  }
  
  // For each question in the Qlist list.
  for (it_bq = l.second.begin(), it_q=Qlist.begin();
       it_bq != l.second.end();
       it_q++, it_bq++){

    // If it is left to be used
    if(*it_bq){
      // temporal count of new leafs for each question tested
      tmpnewleafyes.clear();
      tmpnewleafno.clear();
      yesnumel=nonumel=0;   
      
      // Each element in the data set.
      for (it_be = l.first.begin(), it_data=data.begin();
	   it_be != l.first.end();
	   it_data++, it_be++){
	
	// If it is placed in this node.
	if (*it_be){
	  // is assigned to yes or no nodes by asking the question
	  if (it_q->ask(it_data->at(it_q->getField()))){
	    tmpnewleafyes.push_back(true);
	    tmpnewleafno.push_back(false);
	    yesnumel++;
	  }else{
	    tmpnewleafyes.push_back(false);
	    tmpnewleafno.push_back(true);
	    nonumel++;
	  }
	}else{
	  tmpnewleafno.push_back(false);
	  tmpnewleafyes.push_back(false);
	}
      }
      
      
      // Calculate new entropies
      if (yesnumel >= minelements && nonumel >= minelements){
	if ((yesnumel+nonumel)*balance <= nonumel && yesnumel >= (yesnumel+nonumel)*balance){
	    ey=optimizationFunction(tmpnewleafyes)*yesnumel;
	    en=optimizationFunction(tmpnewleafno)*nonumel;

	// Calculate entropy improvement 
	// and choose the best question
	// and newleafs.
            Question q(*it_q);
	    /*if ((ey+en)/(yesnumel+nonumel) < minex){
            minex = (ey+en)/(yesnumel+nonumel);*/
            if (ey+en < minex){
               minex = (ey+en);
               it_bchosen = it_bq;
	       it_chosen = it_q;
	       newleafyes = tmpnewleafyes;
	       newleafno = tmpnewleafno;
	    }
           //}
	}
      }
	    
      tmpnewleafyes.clear();
      tmpnewleafno.clear();
    }
  }
  
  // If no question has been chosen,
  // then this will be a Leaf

  if (it_bchosen == l.second.end() ){
     Question q(LEAF,calculateLeafValue(l),predictee,actualnumelements,entropy(l.first));
     tree.add(q,nleaf);
     progressbar_add(actualnumelements);
     return true;
  }else{
    tree.add(Question(*it_chosen),nleaf);
  }

  Question q(*it_chosen);
  //  cerr << ">";
  
  it_bchosen = l.second.erase(it_bchosen);
  l.second.insert(it_bchosen,false);

  newl.second.swap(l.second);
  
  newl.first = newleafyes;
  newleafyes.clear();
  
  // Clearing temporal variable, to save memory
  tmpnewleafyes.clear();
  tmpnewleafno.clear();
  newq.clear();
  // ------------------------------------------
  
  if (!split (newl,tree,2*nleaf+1))
    return false;

  newl.first = newleafno;
  newleafno.clear();

  if (!split (newl,tree,2*nleaf+2))      
    return false;
  
  
  if (tree.at(2*nleaf+1).getType() == LEAF && tree.at(2*nleaf+2).getType() == LEAF)
    if (tree.at(2*nleaf+1).getValue() == tree.at(2*nleaf+2).getValue()){
      int old = progressbar(INF);
      split(l,tree,nleaf);
      progressbar(old);
    }

  //  cerr << "<";
  return true;
}


/// This returns the mean value of a leaf
/// it will then be used as predicteed value of the tree
/// at such leaf
map<float,float> DTree::calculateLeafValue(Leaf l){
  int size=0;
  map<float,int> counts;
  map<float,int>::iterator it_counts;
  float cumsum=0, cumsum2=0;
  int flcounts=0;
  map<float,float> predictions;
  vector<bool>::iterator it_elem;
  vector<dataElement>::iterator it_data;

  if (fieldsType[predictee].first == ENU){
    for (it_elem = l.first.begin(), it_data=data.begin(); 
	 it_elem != l.first.end(); 
	 it_elem++, it_data++){
      if (*it_elem){
	size++;
	if(counts.find(it_data->at(predictee)) == counts.end())
	  counts[it_data->at(predictee)] = 0;
	counts[it_data->at(predictee)]++;
      }
    }
    for (it_counts=counts.begin(); it_counts != counts.end(); it_counts++){
      predictions[it_counts->first] = it_counts->second*1.0F / size;
      //      cout << "LEAF: value:("<< predictee << ") " << it_counts->first << "=" << code.decodify((int)it_counts->first) << " prob: " << it_counts->second << "/" << size << "::" << predictions[it_counts->first] << endl;
    }
      
  }else if(fieldsType[predictee].first == FLT){
    for (it_elem = l.first.begin(), it_data=data.begin(); 
	 it_elem != l.first.end() && it_data != data.end();
	 it_elem++, it_data++)
      if (*it_elem){
	flcounts++;
	cumsum += it_data->at(predictee);
	cumsum2 += it_data->at(predictee) * it_data->at(predictee);
      }
    if (flcounts == 1) predictions[cumsum/flcounts] = 0;
    else predictions[cumsum/flcounts] = cumsum2/flcounts - cumsum*cumsum/(flcounts*flcounts);
  }
  return(predictions);
}



/// This is the function that performes the training of the tree 
/// given the data loaded in the class.
/// It buils all the possible questions,
/// Generates the top node,
/// Calculates its entropy, 
/// Initilize the tree to a single Leaf and
/// starts the splitting prcess calling the split function
/// at node 0.
bool DTree::train(){
  int nqst, ndata;
  ndata = data.size();
  nqst = buildQuestionList(section, Qlist);

  if (!errorcheck())
    return false;
    
  Leaf l(vector<bool> (ndata,true), vector<bool>(nqst,true));
  return (train(l));
}

/// Runs fold n out of 'folds'
bool DTree::trainNfold(unsigned int folds, unsigned int n){
  size_t nqst, ndata, step;
  ndata = data.size();
  nqst = buildQuestionList(section, Qlist);

  if (folds > ndata) folds = ndata;
  if (folds <= 0) folds = 1;

  if (n>=folds) n=folds-1;

  if (!errorcheck())
    return false;

  step = ndata/folds;

  cerr << "Nfold trainig " << n << " out of " << folds << endl;

  vector<bool> booleandata(ndata,true);
  vector<bool> booleanquestions(nqst,true);

  for (size_t i=step*n; i<step*(n+1) - 1; i++)
    booleandata[i]=false;
  
  if (n == folds-1)
    for(size_t i=(step+1)*n; i<ndata; i++){
      booleandata[i]=true;
    }
  
  Leaf l(booleandata,booleanquestions);
  return(train(l));
}


bool DTree::train(Leaf l){

  size_t ndata = l.first.size();
  size_t nqst = l.second.size();
  bool flag;
  
  progressbar_init(ndata);

  tree = SparseTree(0,Question(LEAF,calculateLeafValue(l),predictee));

  cerr << endl << "Predicting " << fieldsName.at(predictee) << endl;
  cerr << nqst << " questions to try" << endl;
  cerr << ndata << " elements in the data set" << endl;
  cerr << "Entropy of the data set " << entropy(l.first) << endl;
  cerr << "" << endl;

  if (nqst < 0){
    warn("No questions available");
    return false;
  }
  flag=split(l,tree,0);
  cerr << endl;
  return flag;
}




/// It returns the node of the tree 
/// corresponding to the element given, 
/// node means the starting node to ask questions, so
/// different subtrees could be eventually evaluated. Here
/// it is used for recursive puroses
double DTree::predictLeaf (const vector<float> &element, double node) const {
   if (tree.at(node).getType() == LEAF){
    return node;
   }else if(tree.at(node).getType() == NONE){
     ostringstream ostr;
     ostr << "predictLeaf: inexistent node " << node << endl;
     warn(ostr.str());
     return (node);
   }else{
     if (tree.at(node).ask(element))
       return predictLeaf(element,2*node+1);
     else
       return predictLeaf(element,2*node+2);
   }
}

bool DTree::setStatistics(){
  vector<vector<float> >::const_iterator it_data;
  multimap<double, unsigned long> nodes;  // nodes[node]=data_index;
  pair<multimap<double, unsigned long>::iterator, multimap<double, unsigned long>::iterator> range;
  multimap<double, unsigned long>::iterator it_nodes;
  vector<bool> node;
  set<double> leaflist;
  set<double>::iterator it_llist;

  unsigned long int i;

  for (i=0, it_data = data.begin(); it_data != data.end(); it_data++ , i++){
    double l=predictLeaf(*it_data);
    nodes.insert(pair<double, unsigned long> (l,i));
    leaflist.insert(l);
  }

  for (it_llist = leaflist.begin(); it_llist!=leaflist.end(); it_llist++){
    double n = *it_llist;
    range = nodes.equal_range(n);
    node.resize(data.size(),false);
    for (it_nodes = range.first; it_nodes != range.second && it_nodes != nodes.end(); it_nodes++)
      node.at(it_nodes->second) = true;
    tree.at(n).setEntropy(entropy(node));
    tree.at(n).setSize(nodes.count(n));
  }


  return true;
}


float DTree::optimizationFunction(vector<bool> l){
  switch (errortype){
  case ENTROPY:
    return entropy(l);
  case CORRELATION:
    return (1-correlation(l));
  case CORRECT:
    return (1-correct(l));
  case RMSE:
    return rmse(l);
  default:
    cerr << "ERROR: Wrong minimization function specified " << endl;
    return (-1);
  }
}


/// This returns the man value of a node
/// it will then be used as predicteed value of the tree
/// at such node
float DTree::nodeValue(vector<bool> &l){
  vector<vector<float> >::iterator it_data;
  vector<bool>::iterator it_node;
  int size=0;
  float value=0, selec=-1;
  vector<unsigned int> counts;

  switch (fieldsType.at(predictee).first){
  case FLT:
    for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
      if (*it_node){
	value += it_data->at(predictee);
	size++;
      }
    return (value/size);
    break;
  case ENU:
    for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
      if (*it_node){
	if (it_data->at(predictee) >= counts.size())
	  counts.resize((int)it_data->at(predictee)+1,0);
	counts[(int)it_data->at(predictee)]++;
      }
    for (unsigned int max=counts[0], i=1; i<counts.size(); i++)
      if (counts[i]>=max){
	max=counts[i];
	selec=i;
      }
    return (selec);
    break;
  default:
    cerr << "ERROR: Wrong data format in nodeValue() " << endl;
  }
  return (selec);
}


int DTree::setPredictee (int p){
  int old = predictee;
  predictee = p;
  return old;
}


int DTree::setPredictee (string p){
  int old = predictee;
  predictee = numFieldsName[p];
  return old;
}


string DTree::getPredictee(){
  return (getFieldName(predictee));
}


string DTree::predictValue(vector<float> &element){
  double leaf = predictLeaf(element);
  string value;
  ostringstream ostr ;
  
  if (fieldsType[predictee].first == ENU){
    return(code.decodify((int) tree.at(leaf).getValue()));
  }else{
    ostr << tree.at(leaf).getValue() ;
    return (ostr.str());
  }  
}

string DTree::predictValue(vector<string> &element){

  vector<string>::iterator it;
  vector<float> felement;
  int i;
  
  if (element.size() != fieldsType.size()){
    warn("Element has not a correct numebr of fields ");
    return (string("UNK"));
  }
  
  for (it=element.begin(), i=0; it!=element.end(); it++, i++){
    if (fieldsType.at(i).first == ENU){
      felement.push_back(code.codify(*it));
    }else if(fieldsType.at(i).first == FLT){
      felement.push_back(atof(it->c_str()));
    }
  }
  
  if (element.size() != felement.size()){
    warn("Element has a wrong field format");
    return ("UNK");
  }
  return (predictValue(felement));
}

bool DTree::isLoaded(){
  if (tree.size() > 1)
    return true;
  else
    return false;
}
