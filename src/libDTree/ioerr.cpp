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
/-------------------------------------------------------------------------------*/

#include "DTree.h"


void DTree::error(string msg, int value){
  cerr << "DTree (" << DTREE_VERSION << ") [ERROR]: " << msg << " " <<  endl;
  //  exit(value);
}

void DTree::warn(string msg){
  cerr << "DTree (" << DTREE_VERSION <<  ") [WARNING!]: " << msg << " " << endl;
}


bool DTree::errorcheck(){
  if (predictee >= (signed int) fieldsType.size()){
    ostringstream msg;
    msg << "Predictee out of range (";
    msg << predictee;
    msg << " / ";
    msg << fieldsType.size();
    msg << " )";
    warn(msg.str());
    return false;
  }
  if (minelements <= 0 || minelements > data.size()){
    ostringstream msg;
    msg << "Stopping criteria must be positive and lower than data size (0 < ";
    msg << minelements;
    msg << " < ";
    msg << data.size();
    msg << ")";
    warn(msg.str());
    return false;
  }
  if (balance < 0 || balance > 1){
    warn("Balance must be between 0 and 1 ");
    return false;
  }
  return true;
}
