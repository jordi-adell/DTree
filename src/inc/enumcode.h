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

#ifndef _ENUMCODE_H_
#define _ENUMCODE_H_

#include <map>
#include <string>
#include <iostream>

using namespace std;

/// This class does the codification of enum fields
class EnumCode {

 private:
  map <string, unsigned short int> codif;
  map <unsigned short int, string> decod;

 public:
  EnumCode(){
    codif.empty();
    decod.empty();
  }

  unsigned short int codify(string element){
    unsigned short int size=codif.size();
    if (codif.find(element) == codif.end()){
      codif[element]=size;
      decod[codif[element]]=element;
    }
    return codif[element];
  }

  string decodify(unsigned short int element){
    return decod[element];
  }

  void dumpCodif();

};


#endif
