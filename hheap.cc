// template impl (included from header)
// #include "hheap.h"

template<typename V>
Node<V>::Node(int32_t _id, V _value) :
    id(_id), value(_value), rank(0), is_hollow(false), is_root(true), next_sibling(
        nullptr), head_childs(nullptr) {
}

template<typename V, typename C>
inline std::vector<Node<V>*>& HollowHeap<V, C>::RootsAtRank(uint32_t rank) {
  if (roots_of_rank_.size() <= rank) {
    uint32_t old_size = roots_of_rank_.size();
    roots_of_rank_.resize(rank + 1, nullptr);
    for (uint32_t i = old_size; i <= rank; ++i) {
      roots_of_rank_[i] = new std::vector<Node<V>*>();
    }
  }

  return *roots_of_rank_[rank];
}

template<typename V, typename C>
HollowHeap<V, C>::HollowHeap() :
    size_(0), comparator_(), min_node_(nullptr), roots_of_rank_(), node_map_(), n_ids_(
        0), node_map_arr_(nullptr) {
}

template<typename V, typename C>
HollowHeap<V, C>::HollowHeap(uint32_t n_ids) :
    size_(0), comparator_(), min_node_(nullptr), roots_of_rank_(), node_map_(), n_ids_(
        n_ids), node_map_arr_(new Node<V>*[n_ids_]) {
  Node<V> **arr = node_map_arr_.get();
  std::fill(arr, arr + n_ids_, nullptr);
}

// return false if id is already inserted
template<typename V, typename C>
bool HollowHeap<V, C>::Push(uint32_t id, V value) {
  if (n_ids_) {
    if (node_map_arr_[id] != nullptr) {
      return false;
    }
  } else {
    if (node_map_.find(id) != node_map_.end()) {
      return false;
    }
  }

  Node<V> *new_node = new Node<V>(id, value);

  if (n_ids_) {
    node_map_arr_[id] = new_node;
  } else {
    node_map_[id] = new_node;
  }

  RootsAtRank(0).emplace_back(new_node);

  ++size_;
  if (min_node_ == nullptr || comparator_(value, min_node_->value)) {
    min_node_ = new_node;
  }
  return true;
}

template<typename V, typename C>
void HollowHeap<V, C>::Push(V value) {
  Node<V> *new_node = new Node<V>(-1, value);

  RootsAtRank(0).emplace_back(new_node);

  ++size_;
  if (min_node_ == nullptr || comparator_(value, min_node_->value)) {
    min_node_ = new_node;
  }
}

template<typename V, typename C>
std::pair<int32_t, V> HollowHeap<V, C>::Top() const {
  return std::make_pair(min_node_->id, min_node_->value);
}

template<typename V, typename C>
inline Node<V>* HollowHeap<V, C>::link(Node<V> *r1, Node<V> *r2) {
  Node<V> *temp;
  if (comparator_(r1->value, r2->value)) {
    temp = r1->head_childs;
    r1->head_childs = r2;
    r2->next_sibling = temp;
    r2->is_root = false;
    ++r1->rank;
    return r1;
  } else {
    temp = r2->head_childs;
    r2->head_childs = r1;
    r1->next_sibling = temp;
    r1->is_root = false;
    ++r2->rank;
    return r2;
  }
}

template<typename V, typename C>
void HollowHeap<V, C>::Pop() {
  Node<V> *temp, *temp_2;
  // remove id & make top hollow
  if (min_node_->id >= 0) {
    if (n_ids_) {
      node_map_arr_[min_node_->id] = nullptr;
    } else {
      node_map_.erase(min_node_->id);
    }
  }
  min_node_->is_hollow = true;

  // recursively remove hollow roots
  for (auto it = roots_of_rank_.rbegin(); it != roots_of_rank_.rend(); ++it) {
    std::vector<Node<V>*> &l = **it;
    uint32_t non_hollow = 0;
    for (uint32_t i = 0; i < l.size(); ++i) {
      if (l[i]->is_hollow) { // hollow node
        Node<V> *current = l[i]->head_childs;
        while (current) {
          roots_of_rank_[current->rank]->emplace_back(current);
          current->is_root = true;
          temp = current;
          current = current->next_sibling;
          temp->next_sibling = nullptr;
        }
        delete l[i];
      } else {
        l[non_hollow++] = l[i];
      }
    }
    l.resize(non_hollow);
  }

  // re-link roots and shrink roots vector
  int32_t max_rank = -1;
  for (uint32_t i = 0; i < roots_of_rank_.size(); ++i) {
    std::vector<Node<V>*> &l = *roots_of_rank_[i];
    if (l.size()) {
      max_rank = l.front()->rank;
    }
    uint32_t num_links = l.size() >> 1;
    while (num_links--) {
      temp = l.back();
      l.pop_back();
      temp_2 = l.back();
      l.pop_back();
      temp = link(temp, temp_2);
      RootsAtRank(temp->rank).emplace_back(temp);
    }
  }
  // shrink roots vector
  for (uint32_t i = max_rank + 1; i < roots_of_rank_.size(); ++i) {
    delete roots_of_rank_[i];
  }
  roots_of_rank_.resize(max_rank + 1);

  // update min-id
  min_node_ = nullptr;
  for (auto it = roots_of_rank_.begin(); it != roots_of_rank_.end(); ++it) {
    std::vector<Node<V>*> &l = **it;
    if (l.size()) {
      if (min_node_ == nullptr || comparator_(l[0]->value, min_node_->value)) {
        min_node_ = l[0];
      }
    }
  }

  // reduce size;
  --size_;
}

// return false if id is not inserted yet, or updating value is invalid
template<typename V, typename C>
bool HollowHeap<V, C>::Update(uint32_t id, V value) {
  Node<V> *current_node;
  if (n_ids_) {
    current_node = node_map_arr_[id];
    if (current_node == nullptr || !comparator_(value, current_node->value)) {
      return false;
    }
  } else {
    auto i = node_map_.find(id);
    if (i == node_map_.end() || !comparator_(value, i->second->value)) {
      return false;
    }
    current_node = i->second;
  }

  // if id is root
  if (current_node->is_root) {
    current_node->value = value;
    if (comparator_(current_node->value, min_node_->value)) {
      min_node_ = current_node;
    }
    return true;
  }

  // make current node hollow
  current_node->is_hollow = true;

  // build new node
  Node<V> *new_node = new Node<V>(id, value);

  if (current_node->rank > 2) {
    new_node->rank = current_node->rank - 2;
    Node<V> *second_child = current_node->head_childs->next_sibling;
    new_node->head_childs = second_child->next_sibling;
    second_child->next_sibling = nullptr;
  }

  if (n_ids_) {
    node_map_arr_[id] = new_node;
  } else {
    node_map_[id] = new_node;
  }
  RootsAtRank(new_node->rank).emplace_back(new_node);

  // update min-id
  if (comparator_(new_node->value, min_node_->value)) {
    min_node_ = new_node;
  }

  return true;
}

template<typename V, typename C>
uint32_t HollowHeap<V, C>::size() const {
  return size_;
}

template<typename V, typename C>
bool HollowHeap<V, C>::empty() const {
  return size_ == 0;
}

template<typename V, typename C>
bool HollowHeap<V, C>::ContainsId(uint32_t id) const {
  if (n_ids_) {
    return node_map_arr_[id] != nullptr;
  } else {
    return node_map_.find(id) != node_map_.end();
  }
}

template<typename V, typename C>
V HollowHeap<V, C>::Get(uint32_t id) const {
  if (n_ids_) {
    return node_map_arr_[id]->value;
  } else {
    return node_map_.find(id)->second->value;
  }
}

template<typename V, typename C>
void HollowHeap<V, C>::PopId(uint32_t id) {
  Node<V> *node;
  // find node corresponding to id
  if (n_ids_) {
    node = node_map_arr_[id];
  } else {
    node = node_map_[id];
  }

  // pop
  if (node == min_node_) {
    Pop();
  } else {
    node->is_hollow = true;
    if (n_ids_) {
      node_map_arr_[id] = nullptr;
    } else {
      node_map_.erase(id);
    }
    // reduce size;
    --size_;
  }
}

template<typename V, typename C>
void HollowHeap<V, C>::DeleteNodeRecursively(Node<V> *node) {
  Node<V> *temp;
  while (node->head_childs != nullptr) {
    temp = node->head_childs->next_sibling;
    DeleteNodeRecursively(node->head_childs);
    node->head_childs = temp;
  }
  delete node;
}

template<typename V, typename C>
HollowHeap<V, C>::~HollowHeap() {
  for (auto it = roots_of_rank_.begin(); it != roots_of_rank_.end(); ++it) {
    std::vector<Node<V>*> &l = **it;
    for (const auto &n : l) {
      DeleteNodeRecursively(n);
    }
    delete *it;
  }
}
