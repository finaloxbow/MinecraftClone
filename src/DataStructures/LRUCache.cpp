#include "LRUCache.h"

template<class KEY, class VAL>
LRUCache<KEY, VAL>::LRUCache(unsigned int cache_size){
	if (cache_size < 1)
		m_cache_size = 1;
	else
		m_cache_size = cache_size;

	m_list_size = 0;
}

template<class KEY, class VAL>
LRUCache<KEY, VAL>::~LRUCache() {
	//iterate over list and delete
}

template<class KEY, class VAL>
void LRUCache<KEY, VAL>::put(KEY key, VAL val) {

	if (exists(key)) {
		deleteNode(m_map.find(key));
		m_map.erase(m_map.find(key));
	}

	Node* newNode = new Node(val);
	insertAtHead(newNode);
	m_map.insert(key, newNode);
	m_list_size++;

	if (m_list_size > m_cache_size) {
		deleteNode(tail);
		m_list_size = m_cache_size;
	}
}

template<class KEY, class VAL>
VAL LRUCache<KEY, VAL>::get(KEY key) {
	
	if (!exists(key))
		return nullptr;

	VAL val = m_map.find(key)->second->data;
	deleteNode(m_map.find(key)->second);
	m_map.erase(m_map.find(key));

	Node* newNode = new Node(val);
	m_map.insert(key, newNode);
	insertAtHead(newNode);

	return val;
}

template<class KEY, class VAL>
void LRUCache<KEY, VAL>::deleteNode(Node* node){
	if (node != nullptr) {
		if (node->prev != nullptr && node->next != nullptr) {

			node->prev->next = node->next;
			node->next->prev = node->prev;
			delete node;
		}
		else if (node->prev != nullptr) {

			node->prev->next = nullptr;
			tail = node->prev;
			delete node;
		}
		else {

			head = head->next;
			if (head != nullptr)
				head->prev = nullptr;
			else
				tail = head;
			delete node;
		}
	}
}

template<class KEY, class VAL>
void LRUCache<KEY, VAL>::insertAtHead(Node* node){

	if (head == nullptr) {
		head = node;
	}
	else {
		node->prev = nullptr;
		node->next = head;
		head = node;
	}
}