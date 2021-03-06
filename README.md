# hollow-heap
[![Build Status](https://travis-ci.org/hovinhthinh/hollow-heap.svg?branch=master)](https://travis-ci.org/hovinhthinh/hollow-heap)

Implementation of Hollow Heap (see document here [https://arxiv.org/abs/1510.06535](https://arxiv.org/abs/1510.06535))
## amortized complexity
```
Push: O(1)
Top: O(1)
Pop: O(log N)
Update: O(1)
```
## usage
### template:
```
template<typename V, typename C = std::less<V>> class HollowHeap; 
```
* V: value type
* C: comparing class with declaration:
```
class C {
  bool operator(const V& v1, const V& v2) const;
};
```
* default is min-heap; for max-heap, could use `C = std::greater<V>` for built-in type V
### constructor
```
HollowHeap();
```
* default constructor
```
HollowHeap(uint32_t n_ids);
```
* for convenienve if we already know the number of ids would be used (this could increase the performance of the data structure)
* the ids are indexed from 0 to (n_ids - 1)
### push
```
bool Push(uint32_t id, V value);
```
* push value with corresponding id in to the heap
* return true if pushing successfully, false if id is already pushed
* if the heap is constructed with explicit (n_ids), id should be in between [0..n_ids-1], the data structure won't check for this.
```
void Push(V value);
```
* push value with anonymous id in to the heap
### top
```
std::pair<int32_t, V> Top() const;
```
* return pair<id, value> at the top of heap. if id == -1, this is anonymous pushed value
### pop
```
void Pop();
```
### update
```
bool Update(uint32_t id, V value);
```
* update the value associated with id
* return true if updating successfully, false if id is not pushed yet, or the value to be updated is invalid (not strictly less than current value for min-heap, or not strictly greater than current value for max-heap)
* if the heap is constructed with explicit n_ids, id should be in between [0..n_ids-1], the data structure won't check for this.
### other operations
```
uint32_t size() const;
bool empty() const;
```
* these function should be used to control the heap; Top(), Pop() do not check if the heap is empty, so please call these function before calling Top() or Pop()
```
bool ContainsId(uint32_t id) const;
```
* check if the id is pushed; if the heap is constructed with explicit n_ids, id should be in between [0..n_ids-1]
* complexity: O(1)
```
V Get(uint32_t id) const;
```
* get the value associated with id; the data structure assume that id is already pushed, so before calling this function, please call `ContainsId(uint32_t id)` to check for the availability of id.
* id should be in between [0..n_ids-1] for explicit n_ids
* complexity: O(1)
```
void PopId(uint32_t id);
```
* pop the value associated with id; the data structure assume that id is already pushed, so please ensure this by calling `ContainsId(uint32_t id)` first.
* id should be in between [0..n_ids-1] for explicit n_ids
* complexity: amortized O(logN)
## example
```
#include "hheap.h"
...
HollowHeap<int> hheap; 
// default is min-heap, use HollowHeap<int, greater<int>> for max-heap

hheap.Push(3, 0); // push id 3 with value 0
hheap.Push(1, 5); // push id 1 with value 5
hheap.Push(2, 4); // push id 2 with value 4
hheap.Push(-11); // push value -11
hheap.Update(1, -2); // update value of id 1 to -2
hheap.Update(2, -1); // update value of id 2 to -1
hheap.Pop(); // pop pair<id, value> = <-1, -11> (anonymous)
hheap.Pop(); // pop pair<id, value> = <1, -2>
hheap.Top(); // return pair<id, value> = <2, -1>
```
## application
* One application is that using this data structure could reduce the complexity of Dijkstra algorithm to O(M + NlogN) instead of O(MlogN) when using binary heap, which is very efficient for dense graph

## License

MIT licensed.
