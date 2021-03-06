#ifndef NET_CODERODDE_UTIL_ARRAY_PROBABILITY_DISTRIBUTION_HPP
#define NET_CODERODDE_UTIL_ARRAY_PROBABILITY_DISTRIBUTION_HPP

#include "ProbabilityDistribution.hpp"
#include <iterator>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

namespace net {
namespace coderodde {
namespace util {
    
    template<typename T>
    class ArrayProbabilityDistribution : public ProbabilityDistribution<T> {
    
    public:
        ArrayProbabilityDistribution() : ProbabilityDistribution<T>() {}
        ArrayProbabilityDistribution(std::random_device::result_type seed) :
        ProbabilityDistribution<T>(seed) {}
        
        ArrayProbabilityDistribution(
            const ArrayProbabilityDistribution<T>& other) {
            this->m_size             = other.m_size;
            this->m_total_weight     = other.m_total_weight;
            m_element_storage_vector = other.m_element_storage_vector;
            m_weight_storage_vector  = other.m_weight_storage_vector;
            m_filter_set             = other.m_filter_set;
        }
        
        ArrayProbabilityDistribution(
            ArrayProbabilityDistribution<T>&& other) {
            this->m_size             = other.m_size;
            this->m_total_weight     = other.m_total_weight;
            m_element_storage_vector =
                std::move(other.m_element_storage_vector);
            
            m_weight_storage_vector  = std::move(other.m_weight_storage_vector);
            m_filter_set             = std::move(other.m_filter_set);
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
        }
        
        ArrayProbabilityDistribution& operator=(
            const ArrayProbabilityDistribution<T>& other) {
            this->m_size             = other.m_size;
            this->m_total_weight     = other.m_total_weight;
            m_element_storage_vector = other.m_element_storage_vector;
            m_weight_storage_vector  = other.m_weight_storage_vector;
            m_filter_set             = other.m_filter_set;
            return *this;
        }
        
        ArrayProbabilityDistribution& operator=(
            ArrayProbabilityDistribution<T>&& other) {
            if (this == &other) {
                return *this;
            }
            
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            
            m_element_storage_vector =
                std::move(other.m_element_storage_vector);
            
            m_weight_storage_vector = std::move(other.m_weight_storage_vector);
            m_filter_set            = std::move(other.m_filter_set);
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
            
            return *this;
        }
        
        bool is_empty() const {
            return this->m_size == 0;
        }
        
        virtual size_t size() const {
            return this->m_size;
        }
        
        virtual bool add_element(T const& element, double weight) {
            if (m_filter_set.find(element) != m_filter_set.cend()) {
                return false;
            }
            
            this->check_weight(weight);
            m_element_storage_vector.push_back(element);
            m_weight_storage_vector.push_back(weight);
            m_filter_set.insert(element);
            this->m_total_weight += weight;
            this->m_size++;
            return true;
        }
        
        virtual T sample_element() {
            this->check_not_empty();
            double value = this->m_real_distribution(this->m_generator) *
                           this->m_total_weight;
            
            for (size_t i = 0; i < this->m_size; ++i) {
                if (value < m_weight_storage_vector[i]) {
                    return m_element_storage_vector[i];
                }
                
                value -= m_weight_storage_vector[i];
            }
            
            throw std::logic_error{"Should not get here."};
        }
        
        virtual bool contains_element(T const& element) const {
            return m_filter_set.find(element) != m_filter_set.cend();
        }
        
        virtual bool remove_element(T const& element) {
            if (!contains_element(element)) {
                return false;
            }
            
            auto target_element_iterator =
            std::find(m_element_storage_vector.begin(),
                      m_element_storage_vector.end(),
                      element);
            
            size_t target_index =
            std::distance(m_element_storage_vector.begin(),
                          target_element_iterator);
            
            m_element_storage_vector.erase(target_element_iterator);
            
            auto target_weight_iterator = m_weight_storage_vector.begin();
            std::advance(target_weight_iterator, target_index);
            
            double weight = m_weight_storage_vector[target_index];
            m_weight_storage_vector.erase(target_weight_iterator);
            m_filter_set.erase(element);
            
            this->m_size--;
            this->m_total_weight -= weight;
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


#endif // NET_CODERODDE_UTIL_ARRAY_PROBABILITY_DISTRIBUTION_HPP
