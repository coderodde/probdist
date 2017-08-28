#ifndef NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP
#define NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP

#include "ProbabilityDistribution.hpp"
#include <iterator>
#include <random>
#include <unordered_set>
#include <vector>

namespace net {
namespace coderodde {
namespace util {
    
    template<typename T>
    class LinkedListProbabilityDistribution :
    public ProbabilityDistribution<T> {
                
    public:
        LinkedListProbabilityDistribution() : ProbabilityDistribution<T>() {}
        LinkedListProbabilityDistribution(std::random_device::result_type seed)
        :
        ProbabilityDistribution<T>(seed) {}
        
        virtual bool add_element(T const& element, double weight) {
            return true;
        }
                
        virtual T& sample_element () {
            T t{};
            return t;
        }
                
        virtual bool contains(T const& element) const {
            return m_filter_set.find(element) != m_filter_set.cend();
        }
                
        virtual bool remove(T const& element) {
            return true;
        }
                
        virtual void clear() {
            this->m_size = 0;
            this->m_total_weight = 0.0;
            m_element_storage_vector.clear();
            m_weight_storage_vector.clear();
            m_filter_set.clear();
        }
                
    private:
        std::vector<T>        m_element_storage_vector;
        std::vector<double>   m_weight_storage_vector;
        std::unordered_set<T> m_filter_set;
    };
            
} // End of namespace net::coderodde::util.
} // End of namespace net::coderodde.
} // End of namespace net.


#endif // NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP
