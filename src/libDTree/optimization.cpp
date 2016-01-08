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
/// \brief Calculates the entropy of a node.

/// It calculates the entropy of a leaf. A leaf is a vector of bool elements that indicates which 
/// data elements are conteind in the leaf.
float DTree::entropy(vector<bool> l){

  vector<bool>::iterator it_leaf;
  vector<dataElement>::iterator it_data;
  float sigma2;
  double cum=0.0, cum2=0.0, e=0.0;
  vector<int> counts;
  vector <int>::iterator it_count;
  int leafsize=0;
  

  // if is a float field
  if (fieldsType[predictee].first == FLT){

    // para cada elemento
     for (it_leaf=l.begin(), it_data=data.begin(); 
          it_leaf!=l.end(); 
          it_leaf++, it_data++){
      
      // si esta en el nodo
             if (*it_leaf){
                leafsize++;
                cum += it_data->at(predictee);
                cum2 += it_data->at(predictee)*(it_data->at(predictee));
             }
          }
          if (leafsize<=1)
             return 0;
    //    leafsize--; // For an unbiased estimator.(But then you can have negative sigma2
          sigma2 = ((cum2/leafsize) - (cum/leafsize)*(cum/leafsize));
          e=sqrt(sigma2);
    // e = log(sqrt(2*PI*exp(1)*sigma2));

  }else if(fieldsType[predictee].first==ENU){
     // For each element of the dataset
     for (it_data = data.begin(), it_leaf=l.begin(); it_data != data.end(); it_leaf++, it_data ++){
        // if it is in the present leaf        
        if (*it_leaf){
           leafsize ++;
           if (it_data->at(predictee) >= counts.size())
              counts.resize((int)(it_data->at(predictee))+1,0);
           // counts how many of each class
           counts[(int)it_data->at(predictee)]++;
        }
     }
     if (leafsize==0)
        return INF;
     // For each possible value of the class
     for (it_count = counts.begin(); it_count != counts.end(); it_count++)
        if (*it_count!=0)
           e -= ((float) *(it_count)/leafsize)*log((float) *(it_count)/leafsize);
     e/=counts.size();

  }else{
     cerr << "Wrong predictee field type in entropy calculation" << endl;
     return -1;
  }

  //cerr << "Entropy--> " << e << endl;
  return e;
}



float DTree::correct(vector<bool> l){
  vector<bool>::iterator it_node;
  vector<vector<float> >::iterator it_data;
  int correct=0, size=0;
  float value = nodeValue(l);
  
  for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
    if (*it_node){
      if (value == it_data->at(predictee))
	correct++;
      size++;
    }  
  return correct*1.0F/size;
}


float DTree::rmse(vector<bool> l){
   vector<bool>::iterator it_node;
   vector<vector<float> >::iterator it_data;
   int size=0;
   float value = nodeValue(l);
   float error2=0;
  
   for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++){
      if (*it_node){
	size++;
	error2 += (value - it_data->at(predictee))*(value - it_data->at(predictee));
      }
   }
   return sqrt(error2*1.0F/size);
}

float DTree::correlation(vector<bool> l){
   vector<bool>::iterator it_node;
   vector<vector<float> >::iterator it_data;

/*   for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
      if (*it_node){
         size++;
         leaf = predictLeaf(*it_data);
         predmean += it_data->at(predictee);
         nodemean += it_node->
         error2 += (tree.at(leaf).getValue() - it_data->at(predictee))*(tree.at(leaf).getValue() - it_data->at(predictee));
}*/
      
   
   error("Optimization function unavailable");
   exit(-1);
}

float DTree::probability(vector<bool> l){
   vector<bool>::iterator it_node;
   vector<vector<float> >::iterator it_data;
   int correct=0, size=0;
   float value = nodeValue(l);
   float sum=0.0F, sum2=0.0F, mean, sigma2;
        
   
   if (fieldsType.at(predictee).first == ENU){
      for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
         if (*it_node){
            if (value == it_data->at(predictee))
               correct++;
            size++;
         }  
      return correct*1.0F/size; 
      cerr << correct << " " << size << endl;   
   }else if(fieldsType.at(predictee).first == FLT){
      for (it_node=l.begin(), it_data=data.begin(); it_data!=data.end(); it_data++, it_node++)
         if (*it_node){
            sum += it_data->at(predictee);
            sum2 += it_data->at(predictee)*it_data->at(predictee);
            size++;
         }
         mean = sum/size;         
         sigma2=sum2/size - mean*mean;
         if (sigma2==0) return 1;
         return (1/sqrt(2*PI*sigma2));//*exp(-1*(value-mean)*(value-mean)/(2*sigma2)));         // Second part is one
   }
   return -1;
}
