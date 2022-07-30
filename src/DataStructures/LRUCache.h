#include <unordered_map>

template <class KEY, class VAL>
class LRUCache {
public:
	class Node {
	public:
		Node* prev;
		Node* next;
		VAL data;

		Node(VAL data_in) {
			data = data_in;
		}

		VAL getData() {
			return data;
		}
	};

	std::unordered_map<KEY, Node*> m_map;

	Node* m_head;
	Node* m_tail;
	unsigned int m_cache_size;
	unsigned int m_list_size;

	//helper functions
	void deleteNode(Node* node) {
		if (node != nullptr) {
			if (node->prev != nullptr && node->next != nullptr) {

				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
			}
			else if (node->prev != nullptr) {

				node->prev->next = nullptr;
				m_tail = node->prev;
				delete node;
			}
			else {

				m_head = m_head->next;
				if (m_head != nullptr)
					m_head->prev = nullptr;
				else
					m_tail = m_head;
				delete node;
			}
		}
	}
	void insertAtHead(Node* node) {

		if (m_head == nullptr) {
			m_head = node;
		}
		else {
			node->prev = nullptr;
			node->next = m_head;
			m_head = node;
		}
	}

	LRUCache(unsigned int cache_size) {
		if (cache_size < 1)
			m_cache_size = 1;
		else
			m_cache_size = cache_size;

		m_list_size = 0;
	}
	~LRUCache(){}

	void put(KEY key, VAL val) {

		if (exists(key)) {
			deleteNode(m_map.find(key)->second);
			m_map.erase(m_map.find(key));
		}

		Node* newNode = new Node(val);
		insertAtHead(newNode);
		m_map.insert(std::make_pair(key, newNode));
		m_list_size++;

		if (m_list_size > m_cache_size) {
			deleteNode(m_tail);
			m_list_size = m_cache_size;
		}
	}
	VAL* get(KEY key) {

		//key not in list
		if (!exists(key))
			return nullptr;

		//move node to front of list
			//delete node and map entry
			//then create new node and new map entry
		//then, return val

		VAL val = m_map.find(key)->second->data;
		deleteNode(m_map.find(key)->second);
		m_map.erase(m_map.find(key));

		Node* newNode = new Node(val);
		m_map.insert(std::make_pair(key, newNode));
		insertAtHead(newNode);

		return &val;
	}
	bool exists(KEY key) {
		return m_map.find(key) != m_map.end();
	}

	//helper function
	auto iterator() {
		return std::make_pair(m_map.begin(), m_map.end());
	}

};