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

#ifndef _QUESTION_H_
#define _QUESTION_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <math.h>
#include <vector>

// fiel entropy and numelem are  only used when a question is actually a leaf.

class Question {
  private:
  short int type;
  float value, entropy;
  int field, numelem;
  map<float, float> prediction;

  string debug_msg;

  // \brief THis function returns the answer to the question
  // \brief for value v, for Enumarate fields.
    bool askENU (float v) const{
      if (v == value){
	return true;
      }else{
	return false;
      }
    }
    // \brief THis function returns the answer to the question
    // \brief for value v, for Float fields.
    bool askFLT (float v) const{
      if (v < value){
	return true;
      }else{
	return false;
      }
    }


  public:
    // Constructors
    // \brieg Defaults values are LEAF -1 -1
    /// type = DTree::LEAF and field=-1;
    Question(){
      type = DTree::NONE;
      value=-1.0F;
      field=-1;
      entropy=0.0F;
      numelem=0;
    }
    Question (short int t, float v,int f,int num=0, float e=0.0F){
      type=t;
      value=v;
      field=f;
      entropy=e;
      numelem=num;
    }
    Question (short int t, map<float,float> p, int f, int num=0, float e=0.0F){
      float bestp=0, bestv=-1;
      map<float,float>::iterator it;
      for (it = p.begin(); it != p.end(); it++){
	if(it->second >= bestp){
	  bestp = it->second;
	  bestv = it->first;
	}
      }
      value=bestv;
      prediction=p;
      type=t;
      field=f;
      entropy=e;
      numelem=num;
    }
    ~Question(){}


    void set_debug(string s){ debug_msg = s;}
    string get_debug(){return debug_msg;}

    // \brief Returns the answer for value v in any case
    // \brief It calls askENU and askFLT.

    bool inline ask(vector<float> element) const{
      return ask (element.at(getField()));
    }

    bool ask(float v) const{

      switch (type){
      case DTree::ENU:
	return (askENU(v));
      case DTree::FLT:
	return (askFLT(v));
      case DTree::LEAF:
	cerr << "You are asking in a LEAF, you can't do this" << endl;
	exit(1);
	break;
	case DTree::NONE:
	  cerr << "You are accessing an unitilised node" << endl;
	  exit(1);
	default:
	cerr << "Value out of format" << endl;
	exit(1);
      }
    }

    inline int getField() const {return field;}
    inline float getValue() const {return value;}
    inline short int getType() const {return type;}
    inline float getEntropy() const {return entropy;}
    inline int getSize() const {return numelem;}
    inline map<float,float> getPrediction() const {return prediction;}
    inline float getPrediction(float k) const {
      if (prediction.find(k) != prediction.end()) 
	return prediction.at(k);
      else
	return 0;
    }
    inline float getValuePrediction() const { return getPrediction(getValue()); }



    inline void addPrediction(float k, float v){prediction[k]=v;}
    inline void setEntropy(float e){ if(type == DTree::LEAF) entropy=e;}
    inline void setSize(int n){ if(type == DTree::LEAF) numelem=n;}
    inline void setValue(float v){value=v;}

    inline void clearPrediction(){prediction.clear();}

    friend ostream& operator << (ostream& os, Question& q){
      if (q.getType() == DTree::ENU){
	os << "Is Field: " << q.getField() << " == " << q.getValue();
      }else if (q.getType() == DTree::FLT){
	os << "Is Field: " << q.getField() << " < " << q.getValue();
      }else if (q.getType() == DTree::LEAF){
	os << "Field: " << q.getField() << " ==> " << q.getValue();
      }
      return os;
    }

    friend int operator == (const Question& q1, const Question& q2){
       if (q1.getField() == q2.getField() && q1.getType() == q2.getType() && q1.getValue() == q2.getValue()){
	return true;
      }else{
	return false;
      }
    }
    friend int operator != (const Question& q1, const Question& q2){
       if (q1.getField() != q2.getField() || q1.getType() != q2.getType() || q1.getValue() != q2.getValue()){
	return true;
      }else{
	 return false;
      }
    }
    
    friend int operator < (const Question& q1, const Question& q2){
      if (q1.getField() == q2.getField())
         if ( q1.getValue() < q2.getValue()){
	  return true;
         }else{
	  return false;
         }
      else{
         if (q1.getField() < q2.getField())
            return true;
         else  
            return false;
      }
    }
    
    friend int operator > (const Question& q1, const Question& q2){
       if (q1.getField() == q2.getField())
          if ( q1.getValue() > q2.getValue()){
            return true;
          }else{
            return false;
          }
          else{
             if (q1.getField() > q2.getField())
                return true;
             else  
                return false;
          }
    }
    

  };

#endif

