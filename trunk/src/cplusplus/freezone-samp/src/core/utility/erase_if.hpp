#ifndef ERASE_IF_HPP
#define ERASE_IF_HPP
#include <map>
#include <set>
#include <algorithm>

template <typename container_t, typename predicate_t>
inline void erase_if(container_t& container, predicate_t predicate) {
    container.erase(remove_if(container.begin(), container.end(), predicate), container.end());
}

template <typename item_t, typename item_pred_t, typename predicate_t>
inline void erase_if(std::set<item_t, item_pred_t> & container, predicate_t predicate) {
    for (typename std::set<item_t, item_pred_t>::iterator it = container.begin(); container.end() != it;) {
        if (predicate(*it)) {
            container.erase(it++);
        }
        else {
            ++it;
        }
    }
}

/*
template<class K, class V, class Predicate> 
void eraseIf( map<K,V>& container, Predicate predicate  ) { 
    for(typename map<K,V>::iterator iter=container.begin() ; iter!=container.end() ; ++iter )   {
        if(predicate(iter))
            container.erase(iter);
    }
}
*/
#endif // ERASE_IF_HPP
