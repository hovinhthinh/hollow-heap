// template impl (included from header)
// #include "hheap.h"

template<typename I, typename V>
Node<I, V>::Node(I _id, V _value) :
		id(_id), pvalue(new V(_value)), rank(0), next_sibling(nullptr), head_childs(nullptr), is_root(true) {
}

template<typename I, typename V, typename C>
std::list<Node<I, V>*>& HollowHeap<I, V, C>::RootsAtRank(uint32_t rank) {
	if (roots_of_rank_.size() <= rank) {
		uint32_t old_size = roots_of_rank_.size();
		roots_of_rank_.resize(rank + 1, nullptr);
		for (uint32_t i = old_size; i <= rank; ++i) {
			roots_of_rank_[i] = new std::list<Node<I, V>*>();
		}
	}

	return *roots_of_rank_[rank];
}

template<typename I, typename V, typename C>
HollowHeap<I, V, C>::HollowHeap() :
		size_(0), comparator_(), min_id_(nullptr), roots_of_rank_(), node_map_() {
}

// return false if id is already inserted
template<typename I, typename V, typename C>
bool HollowHeap<I, V, C>::Push(I id, V value) {
	if (node_map_.find(id) != node_map_.end()) {
		return false;
	}

	Node<I, V> *new_node = new Node<I, V>(id, value);
	node_map_[id] = new_node;
	RootsAtRank(0).emplace_back(new_node);

	++size_;
	if (min_id_ == nullptr || comparator_(value, *(min_id_->pvalue))) {
		min_id_ = new_node;
	}
	return true;
}

template<typename I, typename V, typename C>
std::pair<I, V> HollowHeap<I, V, C>::Top() const {
	return std::make_pair(min_id_->id, *(min_id_->pvalue));
}

template<typename I, typename V, typename C>
Node<I, V>* HollowHeap<I, V, C>::link(Node<I, V> *r1, Node<I, V> *r2) {
	Node<I, V>* temp;
	if (comparator_(*(r1->pvalue), *(r2->pvalue))) {
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

template<typename I, typename V, typename C>
void HollowHeap<I, V, C>::Pop() {
	Node<I, V> *temp, *temp_2;
	// remove id & make top hollow
	node_map_.erase(min_id_->id);
	min_id_->pvalue.reset();

	// recursively remove hollow roots
	for (auto it1 = roots_of_rank_.rbegin(); it1 != roots_of_rank_.rend(); ++it1) {
		std::list<Node<I, V>*> &l = **it1;
		for (auto it2 = l.begin(); it2 != l.end();) {
			if (!((*it2)->pvalue)) { // hollow node
				Node<I, V> *current = (*it2)->head_childs;
				while (current) {
					roots_of_rank_[current->rank]->emplace_back(current);
					current->is_root = true;
					temp = current;
					current = current->next_sibling;
					temp->next_sibling = nullptr;
				}
				delete (*it2);
				it2 = l.erase(it2);
			} else {
				++it2;
			}
		}
	}

	// re-link roots and shrink roots vector
	int32_t max_rank = -1;
	for (uint32_t i = 0; i < roots_of_rank_.size(); ++i) {
		std::list<Node<I, V>*> &l = *roots_of_rank_[i];
		if (l.size()) {
			max_rank = l.front()->rank;
		}
		uint32_t num_links = l.size() >> 1;
		while (num_links--) {
			temp = l.front();
			l.pop_front();
			temp_2 = l.front();
			l.pop_front();
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
	min_id_ = nullptr;
	for (auto it = roots_of_rank_.begin(); it != roots_of_rank_.end(); ++it) {
		std::list<Node<I, V>*> &l = **it;
		if (l.size()) {
			if (min_id_ == nullptr || comparator_(*(l.front()->pvalue), *(min_id_->pvalue))) {
				min_id_ = l.front();
			}
		}
	}

	// reduce size;
	--size_;
}

// return false if id is not inserted yet, or updated value is invalid
template<typename I, typename V, typename C>
bool HollowHeap<I, V, C>::Update(I id, V value) {
	auto i = node_map_.find(id);
	if (i == node_map_.end() || !comparator_(value, *(i->second->pvalue))) {
		return false;
	}

	Node<I, V> *current_node = i->second;
	// if id is root
	if (current_node->is_root) {
		current_node->pvalue.reset(new V(value));
		if (comparator_(*(current_node->pvalue), *(min_id_->pvalue))) {
			min_id_ = current_node;
		}
		return true;
	}

	// make current node hollow
	current_node->pvalue.reset();

	// build new node
	Node<I, V> *new_node = new Node<I, V>(id, value);

	if (current_node->rank > 2) {
		new_node->rank = current_node->rank - 2;
		Node<I, V> *second_child = current_node->head_childs->next_sibling;
		new_node->head_childs = second_child->next_sibling;
		second_child->next_sibling = nullptr;
	}

	node_map_[id] = new_node;
	RootsAtRank(new_node->rank).emplace_back(new_node);

	// update min-id
	if (comparator_(*(new_node->pvalue), *(min_id_->pvalue))) {
		min_id_ = new_node;
	}

	return true;
}

template<typename I, typename V, typename C>
uint32_t HollowHeap<I, V, C>::size() const {
	return size_;
}

template<typename I, typename V, typename C>
bool HollowHeap<I, V, C>::empty() const {
	return size_ == 0;
}

template<typename I, typename V, typename C>
void HollowHeap<I, V, C>::DeleteNodeRecursively(Node<I, V> *node) {
	Node<I, V> *temp;
	while (node->head_childs != nullptr) {
		temp = node->head_childs->next_sibling;
		DeleteNodeRecursively(node->head_childs);
		node->head_childs = temp;
	}
	delete node;
}

template<typename I, typename V, typename C>
HollowHeap<I, V, C>::~HollowHeap() {
	for (auto it = roots_of_rank_.begin(); it != roots_of_rank_.end(); ++it) {
		std::list<Node<I, V>*> &l = **it;
		for (const auto &n : l) {
			DeleteNodeRecursively(n);
		}
		delete *it;
	}
}