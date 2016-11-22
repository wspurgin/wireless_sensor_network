/*
Will Spurgin
10/19/2013
Data Structures - Flight Planner
Stack DS header file
*/

#ifndef STACK_H
#define STACK_H

#include "LList.h"

using namespace std;

template <typename T> class Stack
{
private:
  void copy(const Stack<T> rhs) { this->list_ = rhs.list(); }

  LList<T> list_;


public:
  //default Constructor
  Stack() {}

  //Destructor
  ~Stack() {}

  //Copy Constructor
  Stack(const Stack<T>& rhs) { copy(rhs); }

  //push elements onto the stack
  typename LList<T>::template Node<T>* push(const T& item) { return this->list_.insert(item); }

  //remove the last pushed element
  T pop() { return this->list_.remove(0); }

  //returns the number of elements in the stack
  long unsigned int size() const { return this->list_.length(); }

  void empty() { this->list_.empty(); }

  //returns the list of the stack
  LList<T> list() const { return this->list_; }

  //overloaded assignment operator
  Stack<T>& operator=(const Stack<T>& rhs) { copy(rhs); return *this; }
};
#endif
