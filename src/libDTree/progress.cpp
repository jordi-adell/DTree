#include "DTree.h"

void DTree::progressbar_init(int n) {
  progress.first = n*1.0F/50;   // step
  progress.second = 0;
  return;
}

int DTree::progressbar(int n) {

  int old = progress.second;
  int last = (int) (progress.second*1.0F/progress.first + 0.5); // how many steps.
  int actual = (int) (n*1.0F/progress.first + 0.5);             // how many steps.

//   cout << progress.second << " to " << n << " // ";
//   cout << last << " to " << actual << endl;
  if (actual > last && actual <= 50){
    for (int i=last+1; i<=actual; i++){
      if (i%5 == 0)
	if (i%2 == 0){
	  cout << "|"; 
	  cout.flush();
	}else{
	  cout << "+";  
	  cout.flush();
	}
      else{
	cout << "-";  
	cout.flush();
      }
    }
  }
  progress.second=n;
  // cout << n << " " << progress.second << " || " << actual << " " << last << endl;
  return old;
}

void DTree::progressbar_add(int n){
  // cout << "(" << n << ") ";
  progressbar(progress.second + n);
  return;
}

// void DTree::progressbar_remove(int n){
//   progress.second += n;
//   progressbar(progress.second);
//   return;
// }


/* Example */

// progressbar_init(300);
// for (int i=0; i<300; i++){
//   progressbar(i);
// }
