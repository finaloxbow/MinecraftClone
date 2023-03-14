//#pragma once
//
//#include <list>
//#include <unordered_map>
//#include <optional>
//
//template<class KEY, class VAL> class LRUCache {
//public:
//	LRUCache(unsigned int cache_size) : m_current_size(0) {
//		if (cache_size < 1)
//			m_max_size = 0;
//		else
//			m_max_size = cache_size;
//	}
//
//	//inserts key:val pair into cache
//	void put(KEY key, VAL val){
//
//		//checks whether key already in list
//		auto iter = m_map.find(key);
//		if (iter != m_map.end()) {
//			m_list.erase(iter->second);
//			m_map.erase(iter);
//		}
//
//		//adds pair to map and front of list
//		m_list.push_front(std::make_pair(key, val));
//		m_map.insert(std::make_pair(key, m_list.begin()));
//		m_current_size++;
//	}
//
//	std::optional<VAL> get(KEY key){
//		//check whether key is in map
//		if (!m_map.count(key))
//			return {};
//
//		//move node to front 
//		auto iter = m_map.find(key);
//		m_list.splice(m_list.begin(), m_list, iter->second);
//
//		return { iter->second->second };
//	}
//
//	//removes last node in list and returns its value
//	std::optional<VAL> evict(){
//		if (m_list.empty())
//			return {};
//
//		std::pair<KEY, VAL> last_node = m_list.back();
//		VAL val = last_node.second;
//		KEY key = last_node.first;
//
//		m_map.erase(key);
//		m_list.pop_back();
//		m_current_size--;
//
//		return { val };
//	}
//
//	inline bool exists(KEY key) { return m_map.count(key) > 0; }
//	inline unsigned int size() { return m_current_size; }
//	inline bool needsEviction() {	return m_current_size > m_max_size; }
//
//private:
//
//	std::list<std::pair<KEY, VAL>> m_list;
//	std::unordered_map<KEY, decltype(m_list.begin())> m_map;
//
//	unsigned int m_max_size;
//	unsigned int m_current_size;
//	
//
//
//
//};
