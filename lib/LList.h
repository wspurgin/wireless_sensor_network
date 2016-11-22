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
    T data_;
    //Constructors
    Node(const T& data, Node* n = NULL)
    {
      this->data_ = data;
      this->next_ = n;
    }
  };

  class iterator
  {
  public:
    Node<E>* curr_;

    iterator(Node<E>* ptr) { this->curr_ =  ptr; }

    iterator(const iterator& rhs) { this->curr_ = rhs.curr_; }

    Node<E>* node() { return curr_; }

    //operators

    E& operator*() { return curr_->data_; }


    void operator++() { curr_ = curr_->next_; }

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

  //unordered insert
  Node<E>* insert(const E& item)
  {
    Node<E>* n = new Node<E>(item);
    n->next_ = this->head_;
    if(isEmpty())
      this->tail_ = n;
    this->head_ = n;
    return n;
  }

  Node<E>* append(const E& item)
  {
    Node<E>* n = new Node<E>(item);
    if(isEmpty())
      this->head_ = n;
    else
      this->tail_->next_ = n;
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
  E remove(int pos)
  {
    stringstream err_string_io;
    err_string_io << "Out of bounds exception, recieved " <<  pos
      << " on List of size " << length();
    string err_string = err_string_io.str();

    if(pos < 0)
      throw out_of_range(err_string);
    Node<E>* curr = this->head_;
    Node<E>* prev = this->head_;

    for(int i = 0; i < pos; i++)
    {
      if(curr == NULL)
        throw out_of_range(err_string);
      prev = curr;
      curr = curr->next_;
    }

    if(curr == NULL)
      throw out_of_range(err_string);
    prev->next_ = curr->next_;

    E temp = curr->data_;
    if(curr == this->tail_ && curr != this->head_)
      this->tail_ = prev;
    else if(curr == this->head_ && curr != this->tail_)
      this->head_ = curr->next_;
    else if(curr == this->head_ && curr == this->tail_)
      this->head_ = this->tail_ = NULL;

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

  //recursive destructor function
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
