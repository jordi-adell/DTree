/*--------------------------------------------------------------------------------
    DTree Is a Decision Tree which can do Classification and also Regression.
    Copyright (C) 2005 - Jordi Adell - TALP / UPC
-----------------------------------------------------------------------------------
                    GNU GENERAL PUBLIC LICENSE
		        Version 2, June 1991

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

#ifndef _SPARSETREE_H_
#define _SPARSETREE_H_

#include "DTree.h"


class SparseTree{
private:
   vector <Question> tree;
   map <double, unsigned long int> index;   // index[node]=vector
   /// this variable it_index works together with iterator.
   // double it_index;
   double last;

public:
   typedef vector<Question>::iterator iterator;
   Question emptynode;


   Question& at(double i){ 
     if (index.find(i) != index.end())
       return tree[index[i]]; 
     else
       return emptynode; 
   }
   
   
   // note that nodes in vector arent sorted.
   void inline add(Question q, double node){
      if (node > last)
         last=node;
      index[node]=tree.size();
      tree.push_back(q);
   }
   
   void inline remove(unsigned long int node){
     index.erase(index.find(node));
   }
   
   unsigned long int inline size(){
      return tree.size();  
   }
    
   double inline lastnode(){return last;}
   bool inline full(){return (last >= (numeric_limits<double>::max() - 1)/2);}
   iterator inline end(){return tree.end();}
   void inline clear(){tree.clear(); last=0; index.clear(); tree.push_back(Question(LEAF,-1,-1));}
   iterator inline begin(){return tree.begin();}
   
   void write (FILE *f){
     
   }
   
   void read(FILE *f){
      unsigned long int node, field, type, size;
      size_t dataread;
      float value, entropy;
      clear();
      while (!feof(f)){
         dataread=fread(&node,sizeof(node),1,f);
         dataread=fread(&value,sizeof(value),1,f);
         dataread=fread(&field,sizeof(field),1,f);
         dataread=fread(&type,sizeof(type),1,f);
         dataread=fread(&size,sizeof(size),1,f);
         dataread=fread(&entropy,sizeof(entropy),1,f);
         add(Question(type,value,field,0,entropy),node);
      }
   }

   friend ostream& operator << (ostream& os, SparseTree& tr){
      vector<Question>::iterator it_tree = tr.begin();
      for (; it_tree != tr.end(); it_tree++){
	os << *it_tree << endl;
      }
      os << tr.size() << endl;
      return os;
    }
   
   SparseTree(){
     emptynode = Question();
     last=0;
     index[0]=0;
     tree.push_back(Question(LEAF,-1,-1));
   }
   
   SparseTree(unsigned long int num, Question q){
     emptynode = Question();
     last=0;
     tree =  vector<Question>(num,q);
     for (unsigned long int i=0; i<num; i++)
       index[i]=i;
     last = num;
   }
   ~SparseTree(){}

};





#endif

