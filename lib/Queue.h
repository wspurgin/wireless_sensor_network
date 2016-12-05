/*
Will Spurgin
12/02/2016
CSE 7350 - Wireless Sensory Network
Queue DS header file
*/

#ifndef QUEUE_H
#define QUEUE_H

#include "LList.h"

using namespace std;

template <typename T> class Queue
{
private:
  void copy(const Queue<T> rhs) { this->list_ = rhs.list(); }

  LList<T> list_;


public:
  //default Constructor
  Queue() {}

  //Destructor
  ~Queue() {}

  //Copy Constructor
  Queue(const Queue<T>& rhs) { copy(rhs); }

  //push elements onto the stack
  typename LList<T>::template Node<T>* push(const T& item) { return this->list_.append(item); }

  typename LList<T>::template Node<T>* push(typename LList<T>::template Node<T>* n) { return this->list_.append(n); }

  //remove the last pushed element
  T pop() { return this->list_.remove(this->list_.begin()); }

  T remove(typename LList<T>::iterator i) { return this->list_.remove(i); }

  //returns the number of elements in the stack
  long unsigned int size() const { return this->list_.length(); }

  void empty() { this->list_.empty(); }

  //returns the list of the stack
  LList<T>* list() { return &(this->list_); }

  //overloaded assignment operator
  Queue<T>& operator=(const Queue<T>& rhs) { copy(rhs); return *this; }
};
#endif
