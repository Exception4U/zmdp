/********** tell emacs we use -*- c++ -*- style comments *******************
 $Revision: 1.1 $  $Author: trey $  $Date: 2006-02-11 22:38:10 $
   
 @file    RTDPCore.h
 @brief   No brief

 Copyright (c) 2006, Trey Smith. All rights reserved.

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

#ifndef INCRTDPCore_h
#define INCRTDPCore_h

#include "MatrixUtils.h"
#include "Solver.h"
#include "AbstractBound.h"
#include "MDPCache.h"

namespace zmdp {

struct RTDPCore : public Solver {
  const MDP* problem;
  MDPNode* root;
  MDPHash* lookup;
  AbstractBound* initUpperBound;
  AbstractBound* initLowerBound;
  timeval boundsStartTime;
  timeval previousElapsedTime;
  int numStatesTouched;
  int numStatesExpanded;
  int numTrials;
  int numBackups;
  double lastPrintTime;
  std::ostream* boundsFile;
  bool initialized;

  RTDPCore(AbstractBound* _initUpperBound);

  void init(void);
  MDPNode* getNode(const state_vector& s);
  void expand(MDPNode& cn);
  void update(MDPNode& cn, int* maxUBActionP);

  // different derived classes (RTDP variants) will implement these
  // in varying ways
  virtual bool getUseLowerBound(void) const = 0;
  virtual void updateInternal(MDPNode& cn, int* maxUBActionP) = 0;
  virtual void doTrial(MDPNode& cn, double pTarget) = 0;

  // virtual functions from Solver that constitute the external api
  void planInit(const MDP* pomdp);
  bool planFixedTime(const state_vector& s,
		     double maxTimeSeconds,
		     double minPrecision);
  int chooseAction(const state_vector& s);
  void setBoundsFile(std::ostream* boundsFile);
  ValueInterval getValueAt(const state_vector& s) const;
};

}; // namespace zmdp

#endif /* INCRTDPCore_h */

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 *
 ***************************************************************************/
