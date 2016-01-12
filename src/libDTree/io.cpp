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


/// \brief Load all the Data in memory

/// Function to load de description of each field and the data Float
/// data is stored as is but enum date is codfided into short int and
/// then it is converted into float.
int DTree::descriptionLoad(string descfile){

  if (descfile.empty()) return -1;

  ifstream fdesc(descfile.c_str());
  string text="";
  int nf=0;

  
  if (fdesc.fail()){
    error (string("[IO] Unable to open Description file"));
    return -1;
  }
  
  while (!fdesc.eof()){
     fdesc >> text;
     if (!fdesc) continue;
     if (text.empty()){
        break;
     }
     
     numFieldsName[text]=fieldsName.size();
     fieldsName.push_back(text);

     if (fdesc.eof()){
        cerr << "ERROR: Wrong description format " << text << "-" << endl;
        return -1;
     }
     fdesc >> text;
     if (text == ENUM)
        fieldsType.push_back(pair<short int, int>((int)ENU, nf));
     else if(text == FLOAT)
        fieldsType.push_back(pair<short int, int>((int)FLT, nf));
     else
        cerr << "ERROR: Wrong field type" << endl;

     nf++;
  }
  
  fdesc.close();
  return 0;
}

bool DTree::addElement(dataElement element){

  if (!data.empty())
    if (element.size() !=  fieldsType.size()){
      stringstream ws;
      ws << "Element has not a correct number of fields (" << element.size() << "/" << fieldsType.size() << ")";
      warn(ws.str());
      return false;
    }
   data.push_back(element);
   return true;
}

bool DTree::addElement(vector<string> element){

  vector<string>::iterator it;
  vector<float> felement;
  int i;
  
  if (element.size() != fieldsType.size()){
    stringstream ws;
      ws << "Element has not a correct number of fields (" << element.size() << "/" << fieldsType.size() << ")";
      warn(ws.str());
      return false;
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
    return false;
  }
  return (addElement(felement));
}


int inline DTree::databaseLoad(string datafile){

  ifstream fdata(datafile.c_str());
  string text;
  vector<FieldDef>::iterator type_it;
  vector<string>::iterator name_it;
  vector <dataElement>::iterator it_data; 
  int nf=0;

  nf = fieldsType.size();

  if (!fdata.good()){
    error (string("[IO] Unable to read the data file "));
  }
  
  int indiv_it=0;
  while (!fdata.eof()){		
     dataElement el;
     for (type_it=fieldsType.begin(); type_it != fieldsType.end(); type_it++){
        fdata >> text;
	if (!fdata) continue;
      
        if (text.empty())
           if (type_it==fieldsType.begin())
              break;
	   else{
	     string msg("Wrong data format ");
	     msg += text;
	     msg += " -";
	     error(msg);
	   }

        if (type_it->first==ENU){
           el.push_back(code.codify(text));
        }else if (type_it->first == FLT || type_it->first == INT){
           el.push_back(atof(text.c_str()));
        }
        text.clear();
     }

     if (el.empty())
        continue;
     if (el.size() != fieldsType.size()){
       ostringstream msg;
       msg << "Wrong data format: " << el.size() << " fields found while " << fieldsType.size() <<  " were expected.";
       error(msg.str());
     }
     data.push_back(el);
     indiv_it++;
  }
  fdata.close();  
  return data.size();
}

int DTree::dataLoad(string datafile, string descfile){
  // -- This is to read de description file and fill fieldsName and fieldsType --
  if (descriptionLoad(descfile))
    return(-1);

  // -- This is to read the data --
  return databaseLoad(datafile);
}

bool DTree::saveTree(string treefile, double node){
   ofstream fout(treefile.c_str());
   if (fout.fail()){
     string msg="(I/O) Problems opening "+treefile;
     warn(msg);
     return false;
   }else{
      if (!saveTree(fout,node)){
        string msg = "(I/O) Problems wrinting on "+treefile;
	warn(msg);
	fout.close();
	return false;
      }else{         
         fout.close();
         return true;
      }
   }
   fout.close();
   return true;
}

bool DTree::saveTree(ofstream &fout, double node){
  if (fout.bad()){
    return false;
  }

  if (tree.at(node).getType() != LEAF){   
    /*    cerr << node << "\t" << tree.size() << "\t" << tree.lastnode() << "\t" 
	 << numeric_limits<unsigned long int>::min() << "\t" 
	 << numeric_limits<unsigned long int>::max() << endl;
    */
    
    fout  << fieldsName.at((tree.at(node)).getField()) << " "; // << tree.at(node).getType();
    if (tree.at(node).getType() == ENU)
      fout  << code.decodify((int) (tree.at(node)).getValue()) << endl;
    else if (tree.at(node).getType() == FLT)
      fout << (tree.at(node)).getValue() << endl;
    else if (tree.at(node).getValue() == NONE){
      ostringstream error_msg;
      error_msg.str("(I/O) Problems at saving, malformed tree. (inexistent node ");
      error_msg << node << ") ." << endl;
      error(error_msg.str());
      return false;
    }else{
      string error_msg = "(I/O) Problems at saving, malformed tree.";
      error(error_msg);
      return false;
    }
    
    if (saveTree(fout, 2*node+1))
      return (saveTree(fout, 2*node+2));
    else
      return false;
    
  }else{
    
    map<float, float>  prediction = tree.at(node).getPrediction();
    map<float, float>::iterator it;
    
    if (fieldsType[tree.at(node).getField()].first == ENU){
      
      fout << "LEAF " << fieldsName.at((tree.at(node)).getField()); // << " " << code.decodify((int) tree.at(node).getValue());
      /*       if (prediction.size() > 0)
	       fout << " ";*/
      for (it=prediction.begin(); it!=prediction.end(); it++)
	fout << " " << code.decodify(static_cast<unsigned short int>(it->first)) << " " << it->second;
      fout << endl;
      
    }else if (fieldsType[tree.at(node).getField()].first == FLT){
      fout << "LEAF " << fieldsName.at((tree.at(node)).getField()); // << " " << tree.at(node).getValue();
      /*       if (prediction.size() > 0)
	       fout << " ";*/
      for (it=prediction.begin(); it!=prediction.end(); it++)
	fout << " " << it->first  << " " << it->second;
      fout << endl;
    }
    
  }
   return true;
}


bool DTree::saveUPCt(string treefile, double node){
   ofstream fout(treefile.c_str());
   if (fout.fail()){
     string msg="(I/O) Problems opening "+treefile;
     warn(msg);
     return false;
   }else{
      if (!saveUPCt(fout,node)){
        string msg = "(I/O) Problems wrinting on "+treefile;
	warn(msg);
	fout.close();
	return false;
      }else{         
         fout.close();
         return true;
      }
   }
   fout.close();
   return true;
}


bool DTree::saveUPCt(ofstream &fout, double node){
  if (fout.bad()){
    return false;
  }
   
   if (tree.at(node).getType() != LEAF){   
     fout  << fieldsName.at((tree.at(node)).getField()) << " "; // << tree.at(node).getType();
     if (tree.at(node).getType() == ENU)
       fout  << code.decodify((int) (tree.at(node)).getValue()) << endl;
     else if (tree.at(node).getType() == FLT)
       fout << (tree.at(node)).getValue() << endl;
     
     if (saveUPCt(fout, 2*node+1))
       return (saveUPCt(fout, 2*node+2));
     else
       return false;
     
   }else{
     if (fieldsType[tree.at(node).getField()].first == ENU)
       fout << "-1\t0\t" <<  code.decodify((int) tree.at(node).getValue()) <<  "\t0" << endl;
     //       fout << "LEAF " << fieldsName.at((tree.at(node)).getField()) << " " << code.decodify((int) tree.at(node).getValue()) <<  endl;
     else if (fieldsType[tree.at(node).getField()].first == FLT)
       fout << "-1\t0\t" << tree.at(node).getValue() << "\t0" <<   endl;
     //       fout << "LEAF " << fieldsName.at((tree.at(node)).getField()) << " " << tree.at(node).getValue() <<  endl;
   }
   return true;
}

bool DTree::loadTree(string treefile, double node){
  bool flag;
  ifstream fin(treefile.c_str());
  if (fin.fail() || fin.eof() || fin.bad()){
    string msg = "ERROR: Problems opening " + treefile;
    warn(msg);
    fin.close();
    return false;
  }
  tree.clear();
  flag = loadTree(fin,node);
  fin.close();
  return(flag);
}

bool DTree::loadTree(ifstream &fin, double node){
  int type, i_field, version_check;
  string field, s_value, flag;
  string msg = "(I/O)Problems loading tree", debug_msg;
  float f_value, f_prob;
  char buffer[MAXLINELENGTH];
  istringstream iss;
  vector<Question>::iterator it_tree;
  map<float,float> predictions;
  Question q;
  if (fin.bad()){
    warn(msg);
    return false;
  }
  
  if (!fin.eof()){
    fin.getline(buffer, MAXLINELENGTH);
    iss.str(buffer);
    
    if (iss.str().empty())
      return loadTree(fin,node);

    debug_msg = iss.str();
     
     iss >> field;
     if (field == "LEAF"){
       iss >> field;
       i_field = numFieldsName[field];
       setPredictee(i_field);
       type = fieldsType.at(i_field).first;

       version_check=0;
       
       if (type == ENU){
	 while(!iss.eof()){
	   iss >> s_value;  
	   if (iss.eof()){
	     if (version_check != 0){
	       warn(msg+"(wrong version)");
	       return false;
	     }else
	       f_prob = -1;
	   }else
	     iss >> f_prob;
	   predictions[(float)code.codify(s_value)] = f_prob;
	   version_check++;
	 }
	 q=Question(LEAF,predictions,i_field);
	 q.set_debug(debug_msg);
	 tree.add(q,node);
       }else if(type == FLT){
	 while(!iss.eof()){
	   iss >> f_value;
	   if (iss.eof()){
	     if (version_check != 0){
	       warn(msg+" (wrong version)");
	       return false;
	     }else
	       f_prob = -1;
	   }else
	     iss >> f_prob;
	   predictions[f_value]=f_prob;
	 }
	 q=Question(LEAF,predictions,i_field);
	 q.set_debug(debug_msg);
	 tree.add(q,node);
       }
     }else {
       i_field = numFieldsName[field];
       type = fieldsType.at(i_field).first;
       
       if (type == ENU){
	 iss >> s_value;  
	 q=Question(type,code.codify(s_value),i_field);
	 q.set_debug(debug_msg);
	 tree.add(q,node);
       }else if(type == FLT){
	 iss >> f_value;
	 q=Question(type,f_value,i_field);
	 q.set_debug(debug_msg);
	 tree.add(q,node);
       }
       loadTree(fin,2*node+1);
       loadTree(fin,2*node+2);
     }
  }else{
    return false;
  }
   return true;
}


/// It writes the prediction of the tree 
/// corresponding to the data loades into the 
/// the outfile
bool DTree::writeAnswers(string outfile, bool onscreen=false){
   vector<vector<float> >::iterator it_data;
   ofstream fout(outfile.c_str());
   float node;
  
   if (!fout.good())
     return false;

   if (fieldsType.at(predictee).first == FLT)
     for (it_data=data.begin(); it_data!=data.end(); it_data++){
       node = predictLeaf(*it_data);
       fout << tree.at(node).getValue() << endl;
       if (onscreen) cout << tree.at(node).getValue();
     }
   else if(fieldsType.at(predictee).first == ENU)
     for (it_data=data.begin(); it_data!=data.end(); it_data++){
       node = predictLeaf(*it_data);
       fout << code.decodify((int)tree.at(node).getValue()) << endl;
       if (onscreen) cout << code.decodify((int)tree.at(node).getValue()) << " ";
     }else{
       cerr << "ERROR: Wrong field type at 'writeAnswers'" << endl;
       fout.close();
       return false;
     }
   
   fout.close();
   return true;
}


float DTree::getAnswer(vector<float> element){
  double node = predictLeaf(element);
  return(tree.at(node).getValue());
}

string DTree::getStringAnswer(vector<float> element){
  float value = getAnswer(element);
  stringstream svalue;
  if (fieldsType.at(predictee).first == FLT){
    svalue << value;
    return (svalue.str());
  }else
    return (code.decodify((int)value));
}

float DTree::getFloatAnswer(vector<string> element){
  float value;
  vector<float> felement;
  vector<string>::iterator it;
  int i=0;
  for(it=element.begin(); it != element.end(); it++, i++){
    if (fieldsType.at(i).first == FLT)
      felement.push_back(atof(it->c_str()));
    else
      felement.push_back(code.codify(*it));
  }
  return (value = getAnswer(felement));
}

string DTree::getAnswer(vector<string> element){
  stringstream svalue;
  float value = getFloatAnswer(element);
  if (fieldsType.at(predictee).first == FLT){
    svalue << value;
    return (svalue.str());
  }else
    return (code.decodify((int)value));
}


float DTree::getAnswerProb(vector<string> element){
  vector<float> felement;
  vector<string>::iterator it;
  int i=0;
  for(it=element.begin(); it != element.end(); it++, i++){
    if (fieldsType.at(i).first == FLT)
      felement.push_back(atof(it->c_str()));
    else
      felement.push_back(code.codify(*it));
  } 

  return (getAnswerProb(felement));

}


float DTree::getAnswerProb(vector<float> element){

  double node = predictLeaf(element);
  return(tree.at(node).getValuePrediction());

}
