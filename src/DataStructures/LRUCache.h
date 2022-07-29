#include <unordered_map>

template <class KEY, class VAL>
class LRUCache {
private:
	class Node {
		Node* prev;
		Node* next;
		VAL data;

		Node(VAL data_in) {
			data = data_in;
		}
	};

	std::unordered_map<KEY, Node*> m_map;

	Node* m_head;
	Node* m_tail;
	unsigned int m_cache_size;
	unsigned int m_list_size;

	//helper functions
	void deleteNode(Node* node);
	void insertAtHead(Node* node);

public:
	LRUCache(unsigned int cache_size);
	~LRUCache();

	void put(KEY key, VAL val);
	VAL get(KEY key);
	bool exists(KEY key);
};