// Allen Cheung | CSE111 | alkhcheu@ucsc.edu
// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   
   listmap<key_t,mapped_t,less_t>::iterator list_itor = begin();
   while (list_itor != end()) {
      if (less(pair.first, list_itor->first) == false) break;
      ++list_itor;
   }
   if (list_itor != end()) {
      if (less(pair.first, list_itor->first) == true) {
         list_itor->second = pair.second;
         return list_itor;
      }
   }

   node* entry = new node(list_itor.where, list_itor.where->prev, pair);
   entry->next->prev = entry;
   entry->prev->next = entry;
   return iterator(entry);
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   
   listmap<key_t,mapped_t,less_t>::iterator list_itor = begin();
   while (list_itor != end()) {
      if (list_itor->first == that) break;
      ++list_itor;
   }

   return list_itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   
   listmap<key_t,mapped_t,less_t>::iterator list_itor = position;
   ++list_itor;
   position.erase();
   
   return list_itor;
}

//void listmap::iterator::erase()
template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t,mapped_t,less_t>::iterator::erase() {
   if (where != nullptr){
      where->prev->next = where->next;
      where->next->prev = where->prev;
      delete where;
   }
}
