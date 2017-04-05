# hollow-heap
Implementation of Hollow Heap (fastest heap until now - see document here [https://arxiv.org/abs/1510.06535)](https://arxiv.org/abs/1510.06535))
## amortized complexity
```
Push: O(1)
Top: O(1)
Pop: O(log N)
Update: O(1)
```
## usage
```
// push id with corresponding value
// return true if id is not pushed yet, false otherwise
bool Push(I id, V value);

std::pair<I, V> Top() const;

void Pop();

// update the value associated with id
// return true if updating successfully
// return false if id is not pushed yet, or the value to be updated is invalid (not strictly less than current value for min-heap, or not strictly greater than current value for max-heap)
bool Update(I id, V value);

uint32_t size() const;

bool empty() const;
```
## example
```
#include "hheap.h"
...
HollowHeap<int, int> hheap; // HollowHeap<I, V, C = less<K>>; default is min-heap, use C = greater<V> for max-heap

hheap.Push(3, 0); // push key 3 with value 0
hheap.Push(1, 5); // push key 1 with value 5
hheap.Push(2, 4); // push key 2 with value 4
hheap.Update(1, -2); // update value of key 1 to -2
hheap.Update(2, -1); // update value of key 2 to -1
hheap.Pop(); // pop pair<key, value> = <1, -2>
hheap.Top(); // return pair<key, value> = <2, -1>
```
