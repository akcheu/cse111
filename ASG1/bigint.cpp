// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $
// Allen Cheung | CSE 111 | 1/17/2020 | alkhcheu@ucsc.edu

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   // Declare return variable and sign flag
   ubigint sum;
   bool neg_flag {false};
   
   // If signs are the SAME : add
   if (that.is_negative == is_negative) {
      if (is_negative == true) {
         neg_flag = true;
      } else {
         neg_flag = false;
      }
      sum = uvalue + that.uvalue;
      
   // If signs are different: LEFT is greater : subtract
   } else if (uvalue > that.uvalue) {
      if (is_negative == true) {
          neg_flag = true;
      } else {
          neg_flag = false;
      }
      sum = uvalue - that.uvalue;
      
   // If signs are different: RIGHT is greater: subtract
   } else if (uvalue < that.uvalue) {
      if (that.is_negative == true) {
         neg_flag = true;
      } else {
         neg_flag = false;
      }
      sum = that.uvalue - uvalue;
   }
   
   return {sum, neg_flag};
}

bigint bigint::operator- (const bigint& that) const {
   // Declare return variable and sign flag
   ubigint diff;
   bool neg_flag {false};
   
   // If signs are DIFFERENT: add
   if (that.is_negative != is_negative) {
      if (is_negative == true) {
         neg_flag = true;
      } else {
         neg_flag = false;
      }
      diff = uvalue + that.uvalue;
      
      // If left operand is greater: subtract
   } else if (uvalue > that.uvalue) {
      if (is_negative == true) {
         neg_flag = true;
      } else {
         neg_flag = false;
      }
      diff = uvalue - that.uvalue;
      
      // If right operand is greater: subtract
   } else if (uvalue < that.uvalue) {
      if (is_negative == false) {
         neg_flag = true;
      } else {
         neg_flag = false;
      }
      diff = that.uvalue - uvalue;
   }
   return {diff, neg_flag};
}


bigint bigint::operator* (const bigint& that) const {
   // Declare return variable and sign flag
   ubigint prod;
   bool neg_flag {false};
   
   // If either operand is negative, product is negative
   if (is_negative or that.is_negative) {
      neg_flag = true;
   }
   
   // Product computation
   prod = uvalue * that.uvalue;
   
   return {prod, neg_flag};
}

bigint bigint::operator/ (const bigint& that) const {
   // Declare return variable and sign flag
   ubigint quot;
   bool neg_flag {false};
   
   // If either operands are negative, quotient is negative
   if (is_negative or that.is_negative) {
       neg_flag = true;
   }
   
   // Quotient computation
   quot = (uvalue) / (that.uvalue);
   
   return {quot, neg_flag};
}

bigint bigint::operator% (const bigint& that) const {
   // Declare return variable and sign flag
   ubigint rem;
   bool neg_flag {false};
   
   // If either operands are negative, remainder is negative
   if (is_negative or that.is_negative) {
      neg_flag = true;
   }
   
   // Modulus computation
   rem = uvalue % that.uvalue;
   
   return {rem, neg_flag};
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
         << that.uvalue;
}
