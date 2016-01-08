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

#include "enumcode.h"

void EnumCode::dumpCodif(){
  map <string, unsigned short int>::iterator it_cod;
  map <unsigned short int, string>::iterator it_dec;
  
  cerr << endl << "Codification" << endl;
  for (it_cod = codif.begin(); it_cod != codif.end(); it_cod++){
    cerr << it_cod->first << " ## " << it_cod->second << endl;
  }
  
  cerr << endl << "DECodification" << endl;
  for (it_dec = decod.begin(); it_dec != decod.end(); it_dec++){
    cerr << it_dec->first << " ## " << it_dec->second << endl;
  }
}
