#ifndef QUEUE_TPP_
#define QUEUE_TPP_

#include "queue.h"

template <typename T>
Queue<T>::Queue(const Queue<T>& q) {
    for (const auto* curr = q.head_; 
            curr != NULL;
            curr = curr->next_) {
        push(curr->val_);
    }
}


template <typename T>
Queue<T>::Queue(Queue<T>&& q) {
    head_ = q.head_;
    tail_ = q.tail_;

    q.head_ = NULL;
    q.tail_ = NULL;
}

template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& q) {
    reset();
    for (const auto* curr = q.head_; 
            curr != NULL;
            curr = curr->next_) {
        push(curr->val_);
    }

    return *this;
}

template <typename T>
Queue<T>& Queue<T>::operator=(Queue<T>&& q) {
    reset();
    head_ = q.head_;
    tail_ = q.tail_;

    q.head_ = NULL;
    q.tail_ = NULL;

    return *this;
}

template <typename T>
void Queue<T>::reset() {
    while (head_ != NULL) {
        QueueNode<T> *next = head_->next_;

        my_delete(head_);

        head_ = next;
    }
    head_ = NULL;
    tail_ = NULL;
}


template <typename T>
void Queue<T>::push(const T& val) {
    QueueNode<T> *new_n = new QueueNode<T>{val};
    if (new_n == NULL) hardware::err_led::high();
    if (head_ == NULL || tail_ == NULL) {
        head_ = tail_ = new_n;
    } else {
        tail_->next_ = new_n;
        tail_ = new_n;
    }
}

template <typename T>
void Queue<T>::push_front(const T& val) {
    QueueNode<T> *new_n = new QueueNode<T>{val};
    if (new_n == NULL) hardware::err_led::high();
    if (head_ == NULL || tail_ == NULL) {
        head_ = tail_ = new_n;
    } else {
        new_n->next_ = head_; 
        head_ = new_n; 
    }
}

template <typename T>
T Queue<T>::pop() {
    assert(head_ != NULL);
    T val = head_->val_;
    QueueNode<T>* old_head = head_;
    head_ = head_->next_;

    if (head_ == NULL) tail_ = NULL;

    my_delete(old_head);

    return val;
}

template <typename T>
// Erase first instance of val
void Queue<T>::erase(const T& val) {
    QueueNode<T>* prev = NULL;
    for (QueueNode<T>* curr = head_; curr != NULL; curr = curr->next_) {
        if (curr->val_ == val) {
            // Remove this node
            if (prev == NULL) {
                head_ = curr->next_;
            } else {
                prev->next_ = curr->next_;
            }
            if (curr == tail_) {
                tail_ = prev;
            }

            my_delete(curr);
            break;
        }
        prev = curr;
    }
}

#endif
