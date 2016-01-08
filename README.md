# DTree

--------------------------------------------------------------------------------
    DTree Is a Decision Tree which can do Classification and also Regression.
    Copyright (C) 2005 - Jordi Adell - TALP / UPC
-----------------------------------------------------------------------------------
                    GNU LESSER GENERAL PUBLIC LICENSE 
                        Version 3, 29 June 2007 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------

DTree has been developed to generate decision trees which can
perform either classification and regression. It is supplied under the
GNU General Public License and is programed in C++.
	

#Download
========
  You can dowload the source code from GitHub here: https://github.com/jordi-adell/DTree


#Help
====
   It provides a binary for training trees and another one to test them,
The main code is inside a library (libDTree) that can be used by other
programs to train or use DTrees.

Code Documentation
     doc/ 

User Documentation:

     DTrain(v0.3 beta), training a decision tree

     DTrain [options]
     -d Data file
     -c Description file
     -t Output tree file
     -p Predictee fields (def: 0)
     -r Number of section for float fields (def: 10)
     -b Balance, minimum percentage of elements in a node (def: 0)
     -s Minimum number of elements in a leaf (stop criteria) (def: 50)
     -e Errortype. Options are: entropy, rmse, correlation, correct
     -v Print tree on screen
     -h Print this help

     ------------------------------------------------------------------------

     DTest(v0.3 beta) evaluating a decision tree

     DTest [options]
     -d Data file
     -c Description file
     -t Input tree file
     -o Output file
     -p Predictee fields (def: 0)
     -v Verbose predictions on stdout
     -h Print this help

     -------------------------------------------------------------------------

### Description File
   A text file containing a row for each field in the data file. Each row
must have two columns, on first one the name of the field is specified
and on second one the type of the field (float, enum).

Example:
   sepallength float
   sepalwidth float
   petallength float
   petalwidth float
   class enum


### Data file
   A text file with rows meaning elements and a column for each
field. Columns mush be separated by ' '.

* Example:
* 
   5.1 3.5 1.4 0.2 Iris-setosa

   4.9 3.0 1.4 0.2 Iris-setosa
   
   4.7 3.2 1.3 0.2 Iris-setosa
   
   4.6 3.1 1.5 0.2 Iris-setosa
   
   5.0 3.6 1.4 0.2 Iris-setosa
   
   5.4 3.9 1.7 0.4 Iris-setosa
   
   4.6 3.4 1.4 0.3 Iris-setosa
   
   5.0 3.4 1.5 0.2 Iris-setosa

### Output tree
   The name of the file where the tree will be written in.

### Predictee field
   Name of the files (as written in description file) that has to be
predictee by the tree.  Example: class

### Sections
   Float fields will be splitted in sections, in order to
   allow question such as Is petallength > 2.6. The bigger this number
   is the more questions the tree will handle, and the tree can better
   fit the training data. However it will make the training procedure
   slower and may lead to over-fitting.  

### Stopping Criteria (-s) 
   The number of elements in the leaf is used as stopping criteria. If no
more nodes with a number of elements bigger than this number can be
created the tree will stop growing.

### Error type 
   This option is used to choose the minimisation function. Default is
likelihood but also Root Mean Square Error and percentage of correctly
classified elements can be used.  If you need more help you can
contact me at jadell@gps.tsc.upc.edu
