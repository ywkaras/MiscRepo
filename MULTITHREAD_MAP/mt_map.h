#pragma once

#include <atomic>
#include <mutex>
#include <thread>

// See https://github.com/wkaras/C-plus-plus-intrusive-container-templates/blob/master/avl_tree.pdf
//
#include "avl_tree.h"

// Multi-threaded intrusive map from Key to Value, where Key type has < operator.  Optimized for frequent
// lookups and relatively rare changes.
//
template <typename Key, typename Value>
class Map
{
public:
  class Node
  {
  public:
    Key key;
    Value value;

  private:
    struct
    {
      Node *greater, *less;
      int balance_factor;
    }
    st[2];

    friend class Map;
  };

  // Returns null if no node with given key.
  //
  Node const * find(Key const &key);

  // Returns false if there is already a node in the map with the same key.
  //
  bool insert(Node &new_node);

  Map()
  {
    tree[0].set_select(0);
    tree[1].set_select(1);
  }

  // No copying.
  Map(Map const &) = delete;
  Map * operator = (Map const &) = delete;

private:
  struct Abstractor
  {
    using handle = Node *;
    using key = Key const &;
    using size = unsigned;

    int select;

    handle get_less(handle h, bool) { return(h->st[select].less); }
    void set_less(handle h, handle lh) { h->st[select].less = lh; }
    handle get_greater(handle h, bool) { return(h->st[select].greater); }
    void set_greater(handle h, handle gh) { h->st[select].greater = gh; }

    int get_balance_factor(handle h) { return(h->st[select].balance_factor); }
    void set_balance_factor(handle h, int bf) { h->st[select].balance_factor = bf; }

    int compare(key k1, key k2)
    {
      if (k1 > k2)
        return(1);
      if (k1 < k2)
        return(-1);
      return(0);
    }

    int compare_key_node(key k, handle h) { return(compare(k, h->key)); }

    int compare_node_node(handle h1, handle h2) { return(compare(h1->key, h2->key)); }

    handle null(void) { return(nullptr); }

    bool read_error(void) { return(false); } 
  };

  class Tree : public abstract_container::avl_tree<Abstractor>
  {
  public:
    void set_select(int sel) { this->abs.select = sel; }
  };

  Tree tree[2];

  // The least significant bit is the write-in-progress flag for the tree.  The other bits are a count
  // of the number of reads in progress for the the tree.
  //
  std::atomic<unsigned> status[2] = { 0, 0 };

  // Index into tree[], 1 or 0.  Reads are initiated in the tree selected by this index.
  //
  std::atomic<int> read_idx{0};

  std::mutex write_mtx;
};

template <typename Key, typename Value>
typename Map<Key, Value>::Node const * Map<Key, Value>::find(Key const &key)
{
  int sel;

  for (;;)
  {
    // Reading read_idx, an atomic variable, causes an acquire memory fence.
    //
    sel = read_idx;

    unsigned s = status[sel];

    // The write flag could be set after read_idx is read into sel.
    //
    if ((s != 1) and status[sel].compare_exchange_weak(s, s + 2))
      break;
  }

  auto result = tree[sel].search(key);

  status[sel] -= 2;

  return result;
}

template <typename Key, typename Value>
bool Map<Key, Value>::insert(Map<Key, Value>::Node &node)
{
  std::lock_guard<std::mutex> lg(write_mtx);

  int other = !read_idx;

  // No new reads will start on the other (unselected) tree.  This non-busy wait on reads currently in
  // progress will work if all threads are of equal priority.
  //
  for (;;)
  {
    // Set write flag once all current readers have finished.
    //
    unsigned expected = 0;
    if (status[other].compare_exchange_weak(expected, 1))
      break;

    std::this_thread::yield();
  }

  auto r = tree[other].insert(&node);

  status[other] = 0;

  if (r != &node)
    return(false);

  // Select the tree that now has the new node for future reads (finds).  Writing read_idx, an atomic
  // variable, causes a release memory fence.
  //
  read_idx = other;

  // Make other the index of the tree that is now not selected.
  //
  other = !other;

  for (;;)
  {
    // Set write flag once all current readers have finished.
    //
    unsigned expected = 0;
    if (status[other].compare_exchange_weak(expected, 1))
      break;

    std::this_thread::yield();
  }

  tree[other].insert(&node);

  status[other] = 0;

  return(true);
}

// Deleting a node would be similar.
