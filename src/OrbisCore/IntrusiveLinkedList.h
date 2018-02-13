// Copyright (c) 2018 The Orbis developers
// ======================================================================
//              ===== Spell of Creation =====
// script inspired by the CNC code, on February 11, 2018
// 
// ======================================================================
/*                ,     
			     d 
                 :8;        oo 
                ,888     od88 Y 
                " b     "`88888  ,. 
                  :    88888888o. 
                   Y.   `8888888bo 
                    "bo  d888888888oooooo,. 
                      "Yo8:8888888888888Yd8. 
                        `8b;Y88888888888888' 
                        ,888d"Y888888888888.  , 
                        P88Y8b.888888888888b.         `b 
                        :;  "8888888888888888P8b       d; 
                         .   Y88888888888888bo   ",o ,d8 
                              `"8888888888888888888888",oo. 
                               :888888888888888888888P""   ` 
                               `88888888888888888888oooooo. : 
                            ;  ,888888Y888888888888888888888" 
                            `'`P""8.`8;`888888888888888888888o. 
                              `    ` :;  `888888888888888"""8P"o. 
                                 ,  '`8    Y88888888888888;  :b Yb 
                                  8.  :.    `Y8888888888888; dP  `8 
                                ,8d8    "     `888888888888d      Y; 
                                :888d8;        88888888888Pb       ; 
                                :888' `   o   d888888888888;      :' 
                               oo888bo  ,."8888888888888888;    ' ' 
                               8888888888888888888888888888; 
                       ,.`88booo`Y888888888888888888888888P' 
                           :888888888888888888888888888888' 
                   ,ooooood888888888888888888888888888888' 
                  ""888888888888888888888888888888888888; 
             ,oooooo888888888888888888888888888888888888' 
               "88888888888888888888888888888888888888P 
       ,oo bo ooo88888888888888888888888888888888888Y8 
     ."8P88d888888888888888888888888888888888888888"`"o. 
      oo888888888888888888888888888888888888888888"    8 
     d88Y8888888888888888888888888888888888888888' ooo8bYooooo. 
    ,""o888888888888888888888888888888888P":888888888888888888b 
    `   ,d88888888888888888888888888888"'  :888888888888888bod8 
      ,88888888888888888888888888888"      `d8888888888888o`88"b 
    ,88888888888888888888888888""            ,88888' 88  Y8b 
    " ,8888888888888888888""        ,;       88' ;   `8'  P 
     d8888888888888888888boo8888888P"         :.     ` 
    d888888888888888888888888888888boooo 
   :"Y888888888888P':88888"""8P"88888P' 
   ` :88888888888'   88""  ,dP' :888888. 
    ,88888888888'          '`  ,88,8b d" 
    d88888888888               dP"`888' 
    Y :888888888;                   8' 
      :8888888888.                 ` 
      :888888888888oo                        ,ooooo 
      :8888888888888o              ,o   oo d88888oooo. 
       ' :888888888888888booooood888888888888888888888bo.  -hrr- 
          Y88888888888888888888888888888888888"""888888o. 
           "`"Y8888888888888888888888888""""'     `"88888b. 
               "888"Y888888888888888"                Y888.' 
                `"'  `""' `"""""'  "          ,       8888 
                                              :.      8888 
                                           d888d8o    88;` 
                                           Y888888;  d88; 
                                         o88888888,o88P:' 
                                    "ood888888888888"' 
                                ,oo88888""888888888. 
                              ,o8P"b8YP  '`"888888;"b. 
                           ' d8"`o8",P    """""""8P 
                                    `;          d8; 
                                                 8;
// =============================================================
//              ===== it's not an illusion =====
//                  ===== it's real =====
//
// =============================================================
*/                          
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

namespace Orbis {

//Value must have public method IntrusiveLinkedList<Value>::hook& getHook()
template<class Value> class IntrusiveLinkedList {
public:
  class hook {
  public:
    friend class IntrusiveLinkedList<Value>;

    hook();
  private:
    Value* prev;
    Value* next;
    bool used;
  };

  class iterator {
  public:
    iterator(Value* value);

    bool operator!=(const iterator& other) const;
    bool operator==(const iterator& other) const;
    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    Value& operator*() const;
    Value* operator->() const;

  private:
    Value* currentElement;
  };

  IntrusiveLinkedList();

  bool insert(Value& value);
  bool remove(Value& value);

  bool empty() const;

  iterator begin();
  iterator end();
private:
  Value* head;
  Value* tail;
};

template<class Value>
IntrusiveLinkedList<Value>::IntrusiveLinkedList() : head(nullptr), tail(nullptr) {}

template<class Value>
bool IntrusiveLinkedList<Value>::insert(Value& value) {
  if (!value.getHook().used) {
    if (head == nullptr) {
      head = &value;
      tail = head;
      value.getHook().prev = nullptr;
    } else {
      tail->getHook().next = &value;
      value.getHook().prev = tail;
      tail = &value;
    }

    value.getHook().next = nullptr;
    value.getHook().used = true;
    return true;
  } else {
    return false;
  }
}

template<class Value>
bool IntrusiveLinkedList<Value>::remove(Value& value) {
  if (value.getHook().used && head != nullptr) {
    Value* toRemove = &value;
    Value* current = head;
    while (current->getHook().next != nullptr) {
      if (toRemove == current) {
        break;
      }

      current = current->getHook().next;
    }

    if (toRemove == current) {
      if (current->getHook().prev == nullptr) {
        assert(current == head);
        head = current->getHook().next;

        if (head != nullptr) {
          head->getHook().prev = nullptr;
        } else {
          tail = nullptr;
        }
      } else {
        current->getHook().prev->getHook().next = current->getHook().next;
        if (current->getHook().next != nullptr) {
          current->getHook().next->getHook().prev = current->getHook().prev;
        } else {
          tail = current->getHook().prev;
        }
      }

      current->getHook().prev = nullptr;
      current->getHook().next = nullptr;
      current->getHook().used = false;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

template<class Value>
bool IntrusiveLinkedList<Value>::empty() const {
  return head == nullptr;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::begin() {
  return iterator(head);
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::end() {
  return iterator(nullptr);
}

template<class Value>
IntrusiveLinkedList<Value>::hook::hook() : prev(nullptr), next(nullptr), used(false) {}

template<class Value>
IntrusiveLinkedList<Value>::iterator::iterator(Value* value) : currentElement(value) {}

template<class Value>
bool IntrusiveLinkedList<Value>::iterator::operator!=(const typename IntrusiveLinkedList<Value>::iterator& other) const {
  return currentElement != other.currentElement;
}

template<class Value>
bool IntrusiveLinkedList<Value>::iterator::operator==(const typename IntrusiveLinkedList<Value>::iterator& other) const {
  return currentElement == other.currentElement;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator& IntrusiveLinkedList<Value>::iterator::operator++() {
  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().next;
  return *this;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::iterator::operator++(int) {
  IntrusiveLinkedList<Value>::iterator copy = *this;

  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().next;
  return copy;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator& IntrusiveLinkedList<Value>::iterator::operator--() {
  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().prev;
  return *this;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::iterator::operator--(int) {
  IntrusiveLinkedList<Value>::iterator copy = *this;

  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().prev;
  return copy;
}

template<class Value>
Value& IntrusiveLinkedList<Value>::iterator::operator*() const {
  assert(currentElement != nullptr);

  return *currentElement; 
}

template<class Value>
Value* IntrusiveLinkedList<Value>::iterator::operator->() const {
  return currentElement; 
}

}
