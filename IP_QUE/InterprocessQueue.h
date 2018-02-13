#pragma once

#include <condition_variable>
#include <mutex>

#include "list.h"

class RawInterprocessQueue
{
private:
  using SLL = abstract_container::p_list<true>;
  using Lock = std::unique_lock<std::mutex>;

public:
  using Elem = SLL::elem;
 
  void 
  enqueue(Elem &e)
  {
    bool empty;
    {
      Lock lock(mtx);
      empty = sll.empty();
      sll.push(&e, false);
    }
    if (empty) {
      // Notify all rather than one is necessary here, because a sequence like the following is possible:
      //
      // 1.  Queue is empty, consumer thread 1 blocks on condition variable.
      // 2.  Consumer 2 blocks on condition variable.
      // 3.  Producer adds an element to queue, and notifies.
      // 4.  Producer wins race condition and adds a second element to queue.
      // 5.  Consumer 1, unblocked by notify, takes an element from the queue.
      //
      // If only one is notified, consumer 2 is now blocked even though there is an element available for it in the queue.
      //
      // An alternative correct approach is for the producer to notify one every time it adds an element to the queue.  In a
      // case where production is faster than consumption, this might lead to fewer spurious wake-ups of consumers, and thus
      // potentially better performance.
      //
      cv.notify_all();
    }
  }

  Elem &
  dequeue()
  {
    Elem *e;
    {
      Lock lock(mtx);

      while (sll.empty()) {
        cv.wait(lock);
      }

      e = sll.pop();
    }
    return *e;
  }

  private:
    std::mutex mtx;
    std::condition_variable cv;
    SLL sll;
};

using InterprocessQueueElem = RawInterprocessQueue::Elem;

// IntermediateBase must be derived from InterprocessQueueElem. ElemType must be derived from IntermediateBase, or be
// the same class as IntermediateBase.
//
template <class ElemType, class IntermediateBase = ElemType>
class InterprocessQueue
{
public:
  void enqueue(ElemType &e) { q.enqueue(static_cast<IntermediateBase &>(e)); }

  ElemType & dequeue() { return static_cast<ElemType &>(static_cast<IntermediateBase &>(q.dequeue())); }

private:
  RawInterprocessQueue q;
};

/*
State Transition Analysis:

First element in state tuple:
-  : Nothing holds mutex.
P  : Producer thread holds mutex.
C  : A consumer thread holds mutex.

Second element in state tuple:
-  : Nothing blocked on condition variable.
V  : At least one consumer blocked on condition variable.

Third element in state tuple:
-  : Nothing in queue.
Q  : At least on element in queue.

Initial state: (-, -, -)

State transitions:

OLD STATE -> NEW STATE
EVENT TRIGGERING STATE TRANSITION

(-, -, -) -> (P, -, -)
Producer takes mutex.

(-, -, -) -> (C, -, -)
A consumer takes mutex.

(-, -, -) -> (-, -, -)
Producer notifies consumers waiting on condition variable (but there are none).

(-, -, Q) -> (P, -, Q)
Producer takes mutex.

(-, -, Q) -> (C, -, Q)
A consumer takes mutex.

(-, -, Q) -> (-, -, Q)
Producer notifies consumers waiting on condition variable (but there are none).

(-, V, -) -> (P, V, -)
Producer takes the mutex.

(-, V, -) -> (-, -, -)
Producer notifies all consumers waiting on condition variable.

(-, V, -) -> (C, V, -)
A consumer takes the mutex.

(-, V, Q) -> (P, V, Q)
Producer takes the mutex.

(-, V, Q) -> (-, -, Q)
Producer notifies all consumers waiting on condition variable.

(-, V, Q) -> (C, V, Q)
A consumer takes the mutex.

(P, -, -) -> (P, -, Q)
Producer puts an element in queue.

(P, -, Q) -> (P, -, Q)
Producer puts an element in queue.

(P, -, Q) -> (-, -, Q)
Producer releases mutex.

(P, V, -) -> (P, V, Q)
Producer puts an element in queue.

(P, V, Q) -> (P, V, Q)
Producer puts an element in queue.

(P, V, Q) -> (-, V, Q)
Producer releases mutex.

(C, -, -) -> (-, V, -)
A consumer releases mutex.

(C, -, Q) -> (C, -, -) or (C, -, Q)
A consumer takes an element from the queue.

(C, V, -) -> (-, V, -)
A consumer releases mutex.

(C, V, Q) -> (C, V, -) or (C, V, Q)
A consumer takes an element from the queue.

(C, V, Q) -> (-, V, Q)
A consumer releases mutex.

*/
