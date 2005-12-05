/********** tell emacs we use -*- c++ -*- style comments *******************
 $Revision: 1.2 $  $Author: trey $  $Date: 2005-12-05 16:06:16 $
   
 @file    testPQueue.cc
 @brief   No brief

 Copyright (c) 2005, Trey Smith. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

 * The software may not be sold or incorporated into a commercial
   product without specific prior written permission.
 * The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 ***************************************************************************/

#include <stdio.h>
#include "PQueue.h"

using namespace std;
using namespace pomdp;

void fill(PQueue<int, double>& q) {
  q.setPriority(0, 0.5);
  q.setPriority(1, 4.5);
  q.setPriority(2, 1.5);
  q.setPriority(3, 6.5);
  q.setPriority(4, 2.5);
  q.setPriority(5, 3.5);
  q.setPriority(6, 8.5);
  q.setPriority(7, 5.5);
  q.setPriority(8, 7.5);
}

int main(int argc, char** argv) {
  PQueue<int, double> q;
  fill(q);

  printf("--1\n");
  while (!q.empty()) {
    printf("v=%d prio=%f\n", q.top(), q.getTopPriority());
    q.pop();
  }
		
  fill(q);
  q.setPriority(1, 1.7);
  q.setPriority(3, 2.7);
  q.erase(5);

  printf("--2\n");
  while (!q.empty()) {
    printf("v=%d prio=%f\n", q.top(), q.getTopPriority());
    q.pop();
  }

  return 0;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/29 04:43:19  trey
 * initial check-in
 *
 *
 ***************************************************************************/