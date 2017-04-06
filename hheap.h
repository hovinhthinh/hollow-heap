// author: [Vinh Thinh Ho]

// hollow heap
// amortized complexity:
// - Push(): O(1)
// - Top(): O(1)
// - Pop(): O(log N)
// - Update(): O(1)

// using hollow heap could reduce the complexity of dijktra to O(M + N log N)
// very efficient for dense graph

#ifndef _HOLLOW_HEAP
#define _HOLLOW_HEAP

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

// I: type of id associated with the value, should be int
// V: type of value used for comparation
// C: comparation class, default is min-heap; for max-heap, use std::greater<V>

namespace {
template<typename I, typename V>
struct Node {
  I id;
  V value;

  uint32_t rank;
  bool is_hollow;
  bool is_root;

  Node<I, V> *next_sibling; // siblings are stored in decreasing order of rank
  Node<I, V> *head_childs; // point to the first child of childs vector

  Node(I _id, V _value);
};
}

template<typename I, typename V, typename C = std::less<V>>
class HollowHeap {
private:
  uint32_t size_;
  C comparator_;
  Node<I, V> *min_id_;

  std::vector<std::vector<Node<I, V>*>*> roots_of_rank_;
  std::unordered_map<I, Node<I, V>*> node_map_;

  uint32_t n_ids_;
  std::unique_ptr<Node<I, V>*[]> node_map_arr_;

  std::vector<Node<I, V>*>& RootsAtRank(uint32_t rank);

  Node<I, V>* link(Node<I, V> *r1, Node<I, V> *r2);

  void DeleteNodeRecursively(Node<I, V> *node);

public:
  HollowHeap();
  HollowHeap(uint32_t n_ids); // indexed from 0 -> n_ids - 1

  bool Push(I id, V value);

  std::pair<I, V> Top() const;

  void Pop();

  bool Update(I id, V value);

  uint32_t size() const;

  bool empty() const;

  ~HollowHeap();
};

#include "hheap.cc"
#endif
