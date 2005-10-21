/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.1 $  $Author: trey $  $Date: 2005-10-21 20:11:52 $
 *  
 * @file    pomdpCommonTypes.h
 * @brief   No brief
 ***************************************************************************/

#ifndef INCpomdpCommonTypes_h
#define INCpomdpCommonTypes_h

// if VEC_OPTIM is set (check the makefile), set NDEBUG just for the vector/matrix headers
#if VEC_OPTIM
#  ifdef NDEBUG
#     define VEC_NDEBUG_WAS_DEFINED 1
#  else
#     define NDEBUG 1
#  endif
#endif

#if USE_UBLAS
# include "ublasMatrixTypes.h"
# define MATRIX_NAMESPACE boost::numeric::ublas
#else
# include "sla.h"
# define MATRIX_NAMESPACE sla
#endif

// undefine NDEBUG if it was previously undefined
#if VEC_OPTIM && !VEC_NDEBUG_WAS_DEFINED
#   undef NDEBUG
#endif

namespace pomdp {

struct ValueInterval {
  double l, u;

  ValueInterval(void) {}
  ValueInterval(double _l, double _u) : l(_l), u(_u) {}
  bool overlapsWith(const ValueInterval& rhs) const {
    return (l <= rhs.u) && (rhs.l <= u);
  }
  double width(void) const {
    return u - l;
  }
};
std::ostream& operator<<(std::ostream& out, const ValueInterval& v);

}; // namespace pomdp

#endif // INCpomdpCommonTypes_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/01/28 03:20:11  trey
 * VEC_OPTIM now implies NDEBUG for sla as well as ublas
 *
 * Revision 1.4  2005/01/26 04:14:15  trey
 * added MATRIX_NAMESPACE
 *
 * Revision 1.3  2005/01/21 18:07:02  trey
 * preparing for transition to sla matrix types
 *
 * Revision 1.2  2005/01/21 15:22:02  trey
 * added include of ublas/operation.hpp, allowing use of axpy_prod()
 *
 * Revision 1.1  2004/11/13 23:29:44  trey
 * moved many files from hsvi to common
 *
 * Revision 1.1.1.1  2004/11/09 16:18:56  trey
 * imported hsvi into new repository
 *
 * Revision 1.3  2003/09/22 18:48:14  trey
 * made several algorithm configurations depend on makefile settings, added extra timing output
 *
 * Revision 1.2  2003/09/16 00:57:02  trey
 * lots of performance tuning, mostly fixed rising upper bound problem
 *
 * Revision 1.1  2003/09/11 01:45:50  trey
 * initial check-in
 *
 *
 ***************************************************************************/