// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $
// Allen Cheung | CSE 111 | 1/17/2020 | alkhcheu@ucsc.edu

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
#include <string>
using namespace std;

#include "ubigint.h"
#include "debug.h"

// Difference from integer to ASCII character
#define INT_DIFF 48

// Constructors
ubigint::ubigint (const int& value) {
   ubig_value.push_back(value + INT_DIFF);
}

ubigint::ubigint (const string& that) {
   string::const_iterator itor = that.cend() - 1;
   while (itor >= that.cbegin()) {
      ubig_value.push_back(*itor);
      itor --;
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   // Declare variables
   ubigint computation;
   int first_value, second_value, sum;
   bool carry {false};
   bool end_flag1 {false};
   bool end_flag2 {false};
   
   // Define two iterators for left and right operands
   ubigvalue_t::const_iterator itor_1, itor_2;
   
   // Initialize iterators at beginning of vector
   itor_1 = ubig_value.cbegin();
   itor_2 = that.ubig_value.cbegin();
   
   // Main while loop: iterate through both vectors to end
   while (itor_1 != ubig_value.cend() or
          itor_2 != that.ubig_value.cend()) {
      
      // End condition: exit loop when end flags enabled
      if (end_flag1 == true and end_flag2 == true) {
         break;
      }
      if (end_flag1 == true) {
         first_value = 0;
      } else {
         first_value = *itor_1 - INT_DIFF;
      }
      if (end_flag2 == true) {
         second_value = 0;
      } else {
         second_value = *itor_2 - INT_DIFF;
      }
      
      // Calculate integer sum
      sum = first_value + second_value;
      
      // If carry flag, add "1" to the sum to emulate carry
      if (carry == true) {
         sum ++;
      }
      // Set carry flag for sums greater than 10
      if (sum >= 10) {
         carry = true;
         string sum_string = to_string(sum);
         computation.ubig_value.push_back(sum_string.back());
      } else {
         carry = false;
         string sum_string2 = to_string(sum);
         computation.ubig_value.push_back(sum_string2.front());
      }
      
      // Increment iterator to next operand in vector
      itor_1++;
      itor_2++;
      
      // Set end flags
      if (itor_1 == ubig_value.cend()){
         end_flag1 = true;
      }
      if (itor_2 == that.ubig_value.cend()){
         end_flag2 = true;
      }
   }
   
   // Hanging carry case
   if (carry == true) {
      computation.ubig_value.push_back('1');
   }
   
   return computation;
}

ubigint ubigint::operator-(const ubigint& that) const {
   // Declare variables
   ubigint computation;
   int first_value, second_value, diff;
   bool borrow {false};
   bool end_flag1 {false};
   bool end_flag2 {false};
   
   // Define and initialize constant iterators
   ubigvalue_t::const_iterator itor_1, itor_2;
   itor_1 = ubig_value.cbegin();
   itor_2 = that.ubig_value.cbegin();
   
   // Main while loop: go through elements of vector
   while (itor_1 != ubig_value.cend() or
          itor_2 != that.ubig_value.cend()) {
      
      // Exit condition: when end flags enabled
      if (end_flag1 == true and end_flag2 == true) {
         break;
      }
      
      // Change character value to integer
      if (end_flag1 == true) {
         first_value = 0;
      } else {
         first_value = *itor_1 - INT_DIFF;
      }
      if (end_flag2 == true) {
         second_value = 0;
      } else {
         second_value = *itor_2 - INT_DIFF;
      }
      
      // Compute integer difference
      diff = first_value - second_value;
      if (borrow == true) {
         diff --;
      }
      
      // If difference is negative, set borrow flag and add 10
      if (diff < 0) {
         borrow = true;
         string diff_string = to_string(diff + 10);
         computation.ubig_value.push_back(diff_string.front());
      } else {
         borrow = false;
         string diff_string2 = to_string(diff);
         computation.ubig_value.push_back(diff_string2.front());
      }
      
      // Increment iterators
      itor_1 ++;
      itor_2 ++;
      
      // Set end flags
      if (itor_1 == ubig_value.cend()){
         end_flag1 = true;
      }
      if (itor_2 == that.ubig_value.cend()){
         end_flag2 = true;
      }
   }
   
   // Remove extra zeroes in computation
   ubigvalue_t::const_iterator zero_itor;
   zero_itor = computation.ubig_value.cend() - 1;
   while ((*zero_itor - INT_DIFF) == 0) {
      computation.ubig_value.pop_back();
      zero_itor --;
   }
   
   return computation;
}

ubigint ubigint::operator* (const ubigint& that) const {
   // Declare return ubigint variable
   ubigint computation;
   
   // If only element in vector is 0, return computation = 0
   if ((ubig_value.size() == 1) and
       ((*ubig_value.cbegin() - INT_DIFF) == 0)) {
      computation.ubig_value.push_back('0');
      return computation;
   }
   if ((that.ubig_value.size() == 1) and
       ((*that.ubig_value.cbegin() - INT_DIFF) == 0)) {
      computation.ubig_value.push_back('0');
      return computation;
   }
   
   // Define integer variables
   int carry, prod;
   // Resize computation to be = sum of size of two operands
   computation.ubig_value.resize(ubig_value.size()
                                 + that.ubig_value.size(), '0');
   
   // Multiplication algorithm defined in lab manual
   for (int m = 0; m < static_cast<int>(ubig_value.size()); m++) {
      carry = 0;
      for (int n = 0; n < static_cast<int>
           (that.ubig_value.size()); n++) {
         prod = (computation.ubig_value.at(m + n) - INT_DIFF) +
         ((ubig_value.at(m) - INT_DIFF) * (that.ubig_value.at(n)
                                           - INT_DIFF) + carry);
         computation.ubig_value.at(m + n) = prod % 10 + INT_DIFF;
         carry = prod / 10;
      }
      computation.ubig_value.at
         (m + that.ubig_value.size()) = carry + INT_DIFF;
   }
   
   // Remove extra zeroes
   ubigvalue_t::const_iterator zero_itor;
   zero_itor = computation.ubig_value.cend() - 1;
   while ((*zero_itor - INT_DIFF) == 0) {
      computation.ubig_value.pop_back();
      zero_itor --;
   }
   
   return computation;
}

void ubigint::multiply_by_2() {
   // Declare constant iterator and variables
   ubigvalue_t::const_iterator itor;
   ubigvalue_t computation;
   int value;
   bool carry {false};
   
   // Initialize iterator at beginning of vector
   itor = ubig_value.cbegin();
   
   // Main while loop: iterate until end of vector
   while (itor != ubig_value.cend()) {
      // Convert char to int and compute multiplication
      value = *itor - INT_DIFF;
      value = value * 2;
      
      // If carry is enabled, increment value by 1 to emulate
      if (carry == true) {
         value ++;
      }
      
      // Set carry flag if value is greater than 10
      // Push individual digits into computation vector
      if (value >= 10) {
         carry = true;
         string prod_string = to_string(value);
         computation.push_back(prod_string.back());
      } else {
         carry = false;
         string prod_string2 = to_string(value);
         computation.push_back(prod_string2.front());
      }
      
      // Increment iterator
      itor ++;
   }
   
   // Hanging carry case
   if (carry == true) {
      computation.push_back('1');
   }
   
   // Clear and store computation into ubig_value vector
   ubig_value.clear();
   itor = computation.cbegin();
   while (itor != computation.cend()) {
      ubig_value.push_back(*itor);
      itor ++;
   }
}

void ubigint::divide_by_2() {
   // Declare iterator, variables, and storage vector
   ubigvalue_t::const_iterator itor, peek;
   ubigvalue_t computation;
   int value, check;
   bool next_odd {false};
   
   // Initialize iterator values
   itor = ubig_value.cbegin();
   peek = ubig_value.cbegin();
   peek ++;
   
   // Peek vector looks one value ahead in vector
   // If vector size is greater than 1, set next_odd bool
   if (peek != ubig_value.cend()) {
      check = *peek - INT_DIFF;
      if ((check % 2) != 0) {
         next_odd = true;
      }
   }
   
   // Main while loop: iterate until iterator reaches end
   while (itor != ubig_value.cend()) {
      value = *itor - INT_DIFF;
      value = value / 2;
      if (next_odd == true) {
         value = value + 5;
      }
      computation.push_back(value + INT_DIFF);
      itor ++;
      peek ++;
      if (peek != ubig_value.cend()) {
         check = *peek - INT_DIFF;
         if ((check % 2) != 0) {
            next_odd = true;
         } else {
            next_odd = false;
         }
      } else {
         next_odd = false;
      }
   }
   
   // Remove extra zeroes
   ubigvalue_t::const_iterator zero_itor;
   zero_itor = computation.cend() - 1;
   while ((*zero_itor - INT_DIFF) == 0) {
      computation.pop_back();
      zero_itor --;
   }
   
   // Clear ubig_value vector and push values from storage
   ubig_value.clear();
   itor = computation.begin();
   while (itor != computation.end()) {
      ubig_value.push_back(*itor);
      itor ++;
   }
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/(const ubigint& that) const {
   return udivide(*this, that).quotient;
}

ubigint ubigint::operator%(const ubigint& that) const {
   return udivide(*this, that).remainder;
}

bool ubigint::operator==(const ubigint& that) const {
   // Declare variables
   int first_value, second_value;
   
   // Condition 1: If different sizes =/=
   if (ubig_value.size() != that.ubig_value.size()) {
      return false;
   }
   
   // Declare and initialize constant iterator
   ubigvalue_t::const_iterator itor_1, itor_2;
   itor_1 = ubig_value.cbegin();
   itor_2 = that.ubig_value.cbegin();
   
   // Loop through each element and return false if =/=
   while (itor_1 != ubig_value.cend()) {
      first_value = *itor_1 - INT_DIFF;
      second_value = *itor_2 - INT_DIFF;
      if (first_value != second_value) {
         return false;
      }
      
      // Increment iterators
      itor_1 ++;
      itor_2 ++;
   }
   
   // If all conditions are passed, return true
   return true;
}

bool ubigint::operator<(const ubigint& that) const {
   // Condition 1: If the size of that is greater, it is greater
   if (ubig_value.size() < that.ubig_value.size()) {
      return true;
      // Condition 2: If size of that is smaller, it is smaller
   } else if (ubig_value.size() > that.ubig_value.size()) {
      return false;
   } else {
      // Declare variables and constant iterator
      int first_value, second_value;
      ubigvalue_t::const_iterator itor_1, itor_2;
      
      // Traverse vector from end (higher order)
      itor_1 = ubig_value.cend() - 1;
      itor_2 = that.ubig_value.cend() - 1;
      while (itor_1 >= ubig_value.cbegin()) {
         first_value = *itor_1 - INT_DIFF;
         second_value = *itor_2 - INT_DIFF;
         if (first_value < second_value) {
            return true;
         } else if (first_value > second_value) {
            return false;
         }
         
         // Decrement iterators
         itor_1 --;
         itor_2 --;
      }
   }
   
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   // Define constant iterator
   vector<unsigned char>::const_iterator itor;
   
   // Initialize iterator: Traverse vector from end
   itor = that.ubig_value.end() - 1;
   int char_count {0};
   
   // While loop: limits amount of characters per line by dc format
   while (itor >= that.ubig_value.begin()) {
      if (char_count == 69) {
         out << "\\" << endl;
         char_count = 0;
      }
      out << *itor;
      itor --;
      char_count ++;
   }
   
   return out;
}

