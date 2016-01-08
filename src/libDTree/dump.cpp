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

/// Writes the tree on a stream
void DTree::dumpTree (double begin, string tab){
  if (begin <= tree.lastnode()){
    float value = tree.at(begin).getValue();

    cout << tab <<  "(" << begin << ") ";

    /*    string debug_msg = tree.at(begin).get_debug();
    if (!debug_msg.empty())
      cout << "[" << tree.at(begin).get_debug() << "] ";
    */

    switch (tree.at(begin).getType()){
    case ENU:
      cout  << "Is " << fieldsName[tree.at(begin).getField()] << " == " << code.decodify((int) tree.at(begin).getValue()) << endl;
      break;
    case FLT:
      cout  << "Is " << fieldsName[tree.at(begin).getField()] << " < " <<  tree.at(begin).getValue() << endl;
      break;
    case LEAF:
      if (fieldsType.at(predictee).first == FLT)
	cout  << "LEAF " 
	     << fieldsName[predictee] << "=" << value
	     << " STD: " << tree.at(begin).getPrediction(value)
	     << " Size: " << tree.at(begin).getSize() << endl;
      else if (fieldsType.at(predictee).first == ENU)
	cout  << "LEAF " 
	     << fieldsName[predictee] << "=" <<  code.decodify((int) value) 
	     << " Pr: " << tree.at(begin).getPrediction(value)
	     << " Size: " << tree.at(begin).getSize() << endl;
      return;
    default:     
      cout << "Wrong tree format" << endl;
      return;
    }
    tab += "  ";
    dumpTree(2*begin+1,tab);
    dumpTree(2*begin+2,tab);
  }
}

/// Writes on cerr all the bunch of questions
void DTree::dumpQuestions(){
   set<DTree::Question>::iterator it_q;
   cerr << endl << "   >QUESTIONS LIST: " << Qlist.size() << endl;
   for (it_q = Qlist.begin(); it_q != Qlist.end(); it_q++){
      if (it_q->getType() == ENU)
         cerr << "Is " << fieldsName[it_q->getField()] << " == " << code.decodify((int) it_q->getValue()) << endl;
      else if (it_q->getType() == FLT)
         cerr << "Is " << fieldsName[it_q->getField()] << " < " << it_q->getValue() << endl;    
      else if (it_q->getType() != LEAF)
         cerr << "ERROR: Wrong question format " << endl;
   }
}



float DTree::dumpStatistics(ostream &os, bool simple){
   vector<vector<float> >::iterator it_data;
   float correct=0, cum=0, cum2=0, e=0, cuma=0, cuma2=0, error, retv=numeric_limits<float>::max();
   double leaf;
  
   for (unsigned int i=0; i<tree.size(); i++)
      if (tree.at(i).getType() == LEAF)
         e += tree.at(i).getEntropy() * tree.at(i).getSize()/data.size();

   if (fieldsType.at(predictee).first == FLT){
      
      for (it_data=data.begin(); it_data!=data.end(); it_data++){
         leaf = predictLeaf(*it_data);
         error = (tree.at(leaf).getValue() - it_data->at(predictee)); 
         cum += error; cum2 += error*error;
         error = fabs(error);
         cuma += error; cuma2 += error*error;

      }
      cum = cum / data.size();
      cum2 = cum2 / data.size();
      cuma = cuma / data.size();
      cuma2 = cuma2 / data.size();

      if (simple){ // RMSE / mean error / mean abs error
	os << sqrt(cum2) << "\t" << cum << "\t" << cuma << endl;
      }else{
	os << "RMSE: " << sqrt(cum2) <<  endl;
	os << "Mean error (abs): " << cum << " (" << cuma << ")" << endl;
	os << "Std error (abs): " << sqrt(cum2 - (cum*cum)) << " (" << sqrt(cuma2 - (cuma*cuma)) << ")" << endl;
	os << "Entropy: " << e << endl;
      }

      retv = sqrt(cum2);

   }else if (fieldsType.at(predictee).first == ENU){
      vector<vector<int> > confusion;
      vector<vector<int> >::iterator it_row;
      vector<int>::iterator it_col;
      map<int,int> recall, precision;
      set<int>::iterator it_val_col, it_val_row;
      set<int> pred_values;
      
      for (it_data=data.begin(); it_data!=data.end(); it_data++){
         
         leaf = predictLeaf(*it_data);
         if (pred_values.find((int) it_data->at(predictee)) == pred_values.end())
              pred_values.insert((int) it_data->at(predictee));
         
         if (confusion.size() <= it_data->at(predictee)){
            confusion.resize((int) (it_data->at(predictee)+1),vector<int>((int) (it_data->at(predictee)+1),0));
            for (it_row = confusion.begin(); it_row!=confusion.end(); it_row++)
               it_row->resize((int) it_data->at(predictee)+1,0);
         }
         if (confusion.size() <= tree.at(leaf).getValue()){
            confusion.resize((int) tree.at(leaf).getValue()+1,vector<int>((int) tree.at(leaf).getValue()+1,0));
            for (it_row = confusion.begin(); it_row!=confusion.end(); it_row++)
               it_row->resize((int) tree.at(leaf).getValue()+1,0);
         }
               
         confusion.at((int) it_data->at(predictee)).at((int) tree.at(leaf).getValue())++;
         if (tree.at(leaf).getValue() == it_data->at(predictee))
            correct++;

      /*      if (counts.size() <= tree.at(leaf).getValue()){
         counts.resize((int) tree.at(leaf).getValue(),vector<int>((int) tree.at(leaf).getValue(),0));
         if (counts.at((int) tree.at(leaf).getValue()).size() <= it_data->at(predcitee))
         counts.resize((int) tree.at(leaf).getValue(),vector<int>((int) tree.at(leaf).getValue(),0));*/
      }


      if (!simple)  os << "--Confusion Matrix ------------------" << endl;
      int sum=0;
      //recall.resize(pred_values.size());
      //   precision.resize(pred_values.size());
      //cerr << recall.size() << " " << precision.size() << " " << pred_values.size() << endl;
      
      for (it_val_row = pred_values.begin(); it_val_row!=pred_values.end(); it_val_row++){
	if (!simple) os << code.decodify(*it_val_row) << "\t";
	sum=0;
	for (it_val_col = pred_values.begin(); it_val_col != pred_values.end(); it_val_col++){
	  sum+=confusion.at(*it_val_row).at(*it_val_col);
	  if (!simple) os << confusion.at(*it_val_row).at(*it_val_col) << "\t"; 
	}
	if (sum!=0)
	  recall[*it_val_row] = (confusion.at(*it_val_row)).at(*it_val_row)*100/sum;
	else
	  recall[*it_val_row] = 0;
	//	 os << endl << "size: " << recall.size() << " " << *it_val_row << endl;
	if (!simple)  os << recall[*it_val_row] << "%"<< endl;      
	//  return;
	
      }
      
      if (!simple) os << "\t";
      for (it_val_col = pred_values.begin(); it_val_col != pred_values.end(); it_val_col++){
	sum = 0;
	for (it_val_row = pred_values.begin(); it_val_row!=pred_values.end(); it_val_row++){
	  sum+=confusion.at(*it_val_row).at(*it_val_col);
	}
	if (sum!=0)
	  precision[*it_val_col]=(confusion.at(*it_val_col)).at(*it_val_col)*100/sum;
	else
	  precision[*it_val_col]=0;
	if (!simple)  os << precision[*it_val_col]  << "%\t";
      }
      if (!simple) {
	os << endl;
	
	os << "-- Statistics ------------------------" << endl;
	vector<bool> tmp(data.size(),true);
	os << "Correct: (with mean value) " << correct*100/data.size() << "%" << "  (" << this->correct(tmp) << ")"<< endl;
	os << "Entropy: " << e << endl;
	os << "F-measure:\t";
      }
      
      
      float globalFmeasure=0, Fmeasure;
      for (it_val_col = pred_values.begin(); it_val_col != pred_values.end(); it_val_col++){
	if (precision[*it_val_col] == 0 || recall[*it_val_col] == 0)
	  Fmeasure = 0;
	else
	  Fmeasure = 2*precision[*it_val_col]*recall[*it_val_col]/(precision[*it_val_col]+recall[*it_val_col]) ;
	globalFmeasure += Fmeasure;

	if (!simple)
	  os << code.decodify(*it_val_col) << ": ";
	os << Fmeasure << "\t";

      }
      
      os << globalFmeasure/pred_values.size() << endl;
      
      retv = globalFmeasure/pred_values.size();
   }else{
      cerr << "ERROR: Wrong field format found while calculating statistics " << endl;
      exit(1);
   }

   return(retv);
}

/// Writes all the data on a stream
void DTree::dumpData(ofstream &ofs){
   vector<dataElement>::iterator it_data;
   vector<FieldDef>::iterator it_fields;
   vector<float>::iterator it_vals;

//    for (it_fields = fieldsType.begin(); it_fields != fieldsType.end(); it_fields++)
//       ofs << it_fields->first << "::" << fieldsName[it_fields->second] << " ";
//    ofs << endl;

   for (it_data = data.begin(); it_data != data.end(); it_data++){
     int i=0;
     for (it_vals = it_data->begin(); it_vals != it_data->end(); it_vals++, i++){
       if (it_vals != it_data->begin())	ofs << "\t";
       if (fieldsType.at(i).first == ENU)
	 ofs << code.decodify((int) *it_vals);
       else
	 ofs << *it_vals;
     }
     ofs << endl;
   }
}

void  DTree::dumpCodification(){
  code.dumpCodif();
}

