/*
Will Spurgin
10/19/2013
Data Structures - Flight Planner
Linked List DS header file
*/
#ifndef LLIST_H
#define LLIST_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;



template <typename E> class LList
{
public:
  template <typename T> class Node
  {
  public:
    Node<T>* next_;
    Node<T>* prev_;
    T data_;
    //Constructors
    Node(const T& data, Node* n = NULL, Node* p = NULL)
    {
      this->data_ = data;
      this->next_ = n;
      this->prev_ = p;
    }
  };

  class iterator
  {

  public:

    static iterator& wrap(Node<E>* ptr) { return iterator(ptr); }

    Node<E>* curr_;

    iterator(Node<E>* ptr) { this->curr_ =  ptr; }

    iterator(const iterator& rhs) { this->curr_ = rhs.curr_; }

    Node<E>* node() { return this->curr_; }

    //operators

    E& operator*() { return curr_->data_; }

    E* operator->() { return &(this->curr_->data_); }

    void operator++() { curr_ = curr_->next_; }

    void operator--() { curr_ = curr_->prev_; }

    iterator operator+(long unsigned int i) {
      iterator t = *this;
      long unsigned int j = 0;
      while(t.curr_ != NULL && j != i) {
        ++t;
        j++;
      }
      if (j != i) throw out_of_range("Requested Iterator Out of Range");
      return t;
    }

    bool operator==(const iterator& rhs) const
    {
      if(this->curr_ == rhs.curr_)
        return true;
      return false;
    }

    bool operator!=(const iterator& rhs) const
    {
      return !operator==(rhs);
    }

  };

  //Default Constructor
  LList()
  {
    this->head_ = NULL;
    this->tail_ = NULL;
  }

  //Destructor
  ~LList()
  {
    empty();
  }

  LList(const LList<E>& rhs)
  {
    this->head_ = NULL;
    this->tail_ = NULL;
    copy(rhs);
  }

  void empty()
  {
    if(!isEmpty())
      destroy();
  }

  iterator begin() const
  {
    return iterator(this->head_);
  }

  iterator end() const
  {
    if(isEmpty())
      return iterator(this->tail_);
    return iterator(this->tail_->next_);
  }

  iterator last() const
  {
    return iterator(this->tail_);
  }

  E& operator[](long unsigned int i) { return *(begin()+i); }

  // TODO instead of returning Node pointers (asking for trouble). Wrap them
  // in an iterator.
  Node<E>* insert(const E& item)
  {
    Node<E>* n = new Node<E>(item);
    n->next_ = this->head_;
    if(isEmpty())
      this->tail_ = n;
    else
      this->head_->prev_ = n;
    this->head_ = n;
    return n;
  }

  Node<E>* insert(Node<E>* n)
  {
    n->next_ = this->head_;
    n->prev_ = nullptr;
    if(isEmpty())
      this->tail_ = n;
    else
      this->head_->prev_ = n;
    this->head_ = n;
    return n;
  }

  // TODO instead of returning Node pointers (asking for trouble). Wrap them
  // in an iterator.
  Node<E>* append(const E& item)
  {
    Node<E>* n = new Node<E>(item);
    if(isEmpty())
      this->head_ = n;
    else
      this->tail_->next_ = n;
    n->prev_ = this->tail_;
    this->tail_ = n;
    return n;
  }

  Node<E>* append(Node<E>* n)
  {
    if(isEmpty())
      this->head_ = n;
    else
      this->tail_->next_ = n;
    n->prev_ = this->tail_;
    n->next_ = nullptr;
    this->tail_ = n;
    return n;
  }

  //find 'item' in LList
  bool find(const E& item) const
  {
    for(iterator i = begin(); i != end(); ++i)
    {
      if(*i == item)
        return true;
    }
    return false;
  }

  /*
  * Removes the node and returns its data,
  * if the value of pos is out of bounds, then the function fails.
  */
  E remove(E item)
  {
    iterator pos = begin();
    for (iterator i = begin(); i != end(); ++i) {
      pos = i;
      if ((*i) == item)
        break;
    }
    if (pos == end()) // item wasn't found
      throw out_of_range("Out of Range error in LList");
    return remove(pos);
  }

  E remove(iterator it) {
    Node<E>* curr = it.node();

    if(curr == NULL)
      throw out_of_range("Out of Range error in LList");
    if (curr->prev_ != NULL)
      curr->prev_->next_ = curr->next_;
    if (curr->next_ != NULL)
      curr->next_->prev_ = curr->prev_;

    E temp = curr->data_;
    if(curr == this->tail_ && curr != this->head_)
      this->tail_ = curr->prev_;
    if(curr == this->head_ && curr != this->tail_)
      this->head_ = curr->next_;
    if(curr == this->head_ && curr == this->tail_) {
      this->head_ = NULL;
      this->tail_ = NULL;
    }

    delete curr;
    return temp;
  }

  //Checks if the list is empty
  bool isEmpty() const
  {
    if(this->head_ == NULL)
      return true;
    else
      return false;
  }

  //Number of nodes in List
  long unsigned int length() const
  {
    long unsigned int count = 0;
    for(iterator i = begin(); i != end(); ++i)
      count++;
    return count;
  }

  //Writes contents of List to ostream, requires stream insertion operator
  void write(ostream& out) const
  {
    for(iterator i = begin(); i != end(); ++i)
      out << *i << endl;
  }

  //Operators

  //overloaded assignement operator
  LList<E>& operator=(const LList<E>& rhs)
  {
    if(!isEmpty())
      destroy();
    return copy(rhs);
  }

  //binary equals operator
  bool operator==(const LList<E>& rhs)
  {
    if(length() != rhs.length())
      return false;
    Node<E>* left = this->head_;
    Node<E>* right = rhs.begin();

    while(left != NULL)
    {
      if(left->data_ != right->data_)
        return false;
      left = left->next_;
      right = right->next_;
    }
    return true;
  }

  //stream extraction operator
  friend ostream& operator<<(ostream& out, const LList<E>& rhs)
  {
    rhs.write(out);
    return out;
  }

private:

  Node<E>* head_;
  Node<E>* tail_;

  // Iterative destructor function
  void destroy()
  {
    while(this->head_!= NULL)
    {
      Node<E>* curr = this->head_;
      this->head_ = this->head_->next_;
      delete curr;
    }
    this->head_ = NULL;
    this->tail_ = NULL;
  }

  LList<E>& copy(const LList<E>& rhs)
  {
    if(!rhs.isEmpty())
    {
      iterator copy = rhs.begin();
      // copy values from first node
      this->head_ = new Node<E>(*copy);
      this->tail_ = this->head_;
      Node<E>* curr = this->head_->next_;
      ++copy;

      //Copy the remaining records
      while(copy != rhs.end())
      {
        curr = new Node<E>(*copy);
        // set previous node to point to the current node
        this->tail_->next_ = curr;
        /* set tail_ to the current node as well
        *(will leave tail_ pointing to the last node)
        */
        this->tail_ = curr;
        curr = curr->next_;
        ++copy;
      }
    }
    return *this;
  }

};

#endif
