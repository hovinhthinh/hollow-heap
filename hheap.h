// hollow heap
// author: [Vinh Thinh Ho]

// amortized running time complexity:
// - Push(): O(1)
// - Top(): O(1)
// - Pop(): O(log N)
// - Update(): O(1)

// V: type of value used for comparison
// C: comparison class, default is min-heap; for max-heap, use std::greater<V>

#ifndef _HOLLOW_HEAP
#define _HOLLOW_HEAP

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
template<typename V>
struct Node {
  int32_t id; // -1 means anonymous id
  V value;

  uint32_t rank;
  bool is_hollow;
  bool is_root;

  Node<V> *next_sibling; // siblings are stored in decreasing order of rank
  Node<V> *head_childs; // point to the first child of childs list

  Node(int32_t _id, V _value);
};
}

template<typename V, typename C = std::less<V>>
class HollowHeap {
private:
  uint32_t size_;
  C comparator_;
  Node<V> *min_node_;

  std::vector<std::vector<Node<V>*>*> roots_of_rank_;
  std::unordered_map<uint32_t, Node<V>*> node_map_;

  uint32_t n_ids_;
  std::unique_ptr<Node<V>*[]> node_map_arr_;

  std::vector<Node<V>*>& RootsAtRank(uint32_t rank);

  Node<V>* link(Node<V> *r1, Node<V> *r2);

  void DeleteNodeRecursively(Node<V> *node);

public:
  HollowHeap();
  HollowHeap(uint32_t n_ids); // indexed from 0 -> n_ids - 1

  bool Push(uint32_t id, V value);
  void Push(V value);

  std::pair<int32_t, V> Top() const;

  void Pop();

  bool Update(uint32_t id, V value);

  uint32_t size() const;

  bool empty() const;

  bool ContainsId(uint32_t id) const;

  V Get(uint32_t id) const;

  void PopId(uint32_t id);

  ~HollowHeap();
};

#include "hheap.cc"
#endif