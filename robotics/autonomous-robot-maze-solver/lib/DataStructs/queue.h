#ifndef QUEUE_H_
#define QUEUE_H_

#include <Arduino.h>
#include <stdlib.h>
#include <assert.h>
#include <new.h>

#include "utility.h"
#include "hardware.h"

#include "hardware.h"

template<typename T>
class QueueNode {
    public:
        QueueNode() : next_{NULL} {};
        template <typename... Args>
        QueueNode(Args... args) : val_{args...}
        {
        }
        QueueNode(const T& val) : 
            next_{NULL},
            val_{val}
        {
            // nop
        };

        QueueNode *next_;
        T val_;
};

template<typename T>
static QueueNode<T> *new_node(const T& val) {
    QueueNode<T> *new_n = static_cast<QueueNode<T> *>(malloc(sizeof(QueueNode<T>)));
    new_n->next_ = NULL;
    new_n->val_ = val;

    return new_n;
}

template <typename T>
static void my_delete(T* val) {
    //val->~T();
    free(val);
}

template<typename T>
class Queue {
    private:
        QueueNode<T> *head_;
        QueueNode<T> *tail_;
    public:
        Queue() : 
            head_{NULL},
            tail_{NULL}
        {
            // nop
        }

        ~Queue() {
            reset();
        }

        Queue(const Queue<T>& q);
        Queue(Queue<T>&& q);
        Queue<T>& operator=(const Queue<T>& q);
        Queue<T>& operator=(Queue<T>&& q);

        void reset();
        
        void push(const T& val);

        void push_front(const T& val);

        T pop();

        constexpr T pop_front() {
            return pop();
        }

        T& peek() {
            return head_->val_;
        }

        constexpr T& peek_front() {
            return peek();
        }

        const T& peek() const {
            return head_->val_;
        }

        void erase(const T& val);

        bool is_empty() const {
            return (head_ == NULL);
        }

        int size() const {
            int size = 0;
            for (const QueueNode<T>* curr = head_; curr != NULL; curr = curr->next_) {
                ++size;
            }
            return size;
        }

        String toString() {
            String str = "[";
            if (!is_empty()) {
                QueueNode<T>* curr = head_; 
                while (curr != NULL) {
                    str += curr->val_;
                    str += ", ";
                    curr = curr->next_;
                }
                str = str.substring(0, str.length() - 2);
            }
            str += "]";
            return str; 
        }
};

#include "queue.tpp"
#endif
