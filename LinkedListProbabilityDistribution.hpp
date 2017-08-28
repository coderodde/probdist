#ifndef NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP
#define NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP

#include "ProbabilityDistribution.hpp"
#include <iterator>
#include <random>
#include <unordered_map>
#include <vector>

namespace net {
namespace coderodde {
namespace util {
    
    template<typename T>
    class LinkedListProbabilityDistribution :
    public ProbabilityDistribution<T> {
        
    private:
        template<typename E>
        class LinkedListNode {
        private:
            
            E                  m_element;
            double             m_weight;
            LinkedListNode<E>* m_prev_node;
            LinkedListNode<E>* m_next_node;
            
        public:
            LinkedListNode(E element, double weight) {
                m_element = element;
                m_weight  = weight;
            }
            
            E get_element() const {
                return m_element;
            }
            
            double get_weight() const {
                return m_weight;
            }
            
            LinkedListNode<E>* get_prev_linked_list_node() const {
                return m_prev_node;
            }
            
            LinkedListNode<E>* get_next_linked_list_node() const {
                return m_next_node;
            }
            
            void set_prev_linked_list_node(LinkedListNode<E>* node) {
                m_prev_node = node;
            }
            
            void set_next_linked_list_node(LinkedListNode<E>* node) {
                m_next_node = node;
            }
        };
        
    public:
        LinkedListProbabilityDistribution() : ProbabilityDistribution<T>() {}
        LinkedListProbabilityDistribution(std::random_device::result_type seed)
        :
        ProbabilityDistribution<T>(seed) {}
        
        virtual bool add_element(T const& element, double weight) {
            if (m_map.find(element) != m_map.end()) {
                return false;
            }
            
            this->check_weight(weight);
            LinkedListNode<T>* new_node = new LinkedListNode<T>{element,
                                                                weight};
            
            if (m_head == nullptr) {
                m_head = new_node;
                m_tail = new_node;
                new_node->set_prev_linked_list_node(nullptr);
                new_node->set_next_linked_list_node(nullptr);
            } else {
                new_node->set_prev_linked_list_node(m_tail);
                new_node->set_next_linked_list_node(nullptr);
                m_tail->set_next_linked_list_node(new_node);
                m_tail = new_node;
            }
            
            m_map[element] = new_node;
            this->m_size++;
            this->m_total_weight += weight;
            return true;
        }
                
        virtual T sample_element() {
            this->check_not_empty();
            double value = this->m_real_distribution(this->m_generator) *
                           this->m_total_weight;
            
            for (LinkedListNode<T>* node = m_head;
                 ;
                 node = node->get_next_linked_list_node()) {
                if (value < node->get_weight()) {
                    return node->get_element();
                }
                
                value -= node->get_weight();
            }
            
            throw std::logic_error{"Should not get here."};
        }
                
        virtual bool contains(T const& element) const {
            return m_map.find(element) != m_map.cend();
        }
                
        virtual bool remove(T const& element) {
            if (!contains(element)) {
                return false;
            }
            
            LinkedListNode<T>* node = m_map[element];
            
            m_map.erase(element);
            this->m_size--;
            this->m_total_weight -= node->get_weight();
            unlink(node);
            return true;
        }
                
        virtual void clear() {
            this->m_size = 0;
            this->m_total_weight = 0.0;
            m_map.clear();
            
            for (LinkedListNode<T>* node = m_head, *next; node != nullptr;) {
                next = node->get_next_linked_list_node();
                delete node;
                node = next;
            }
            
            m_head = nullptr;
            m_tail = nullptr;
        }
                
    private:
        std::unordered_map<T, LinkedListNode<T>*> m_map;
        LinkedListNode<T>* m_head;
        LinkedListNode<T>* m_tail;
        
        void unlink(LinkedListNode<T>* node) {
            LinkedListNode<T>* prev_node = node->get_prev_linked_list_node();
            LinkedListNode<T>* next_node = node->get_next_linked_list_node();
            
            if (prev_node != nullptr) {
                prev_node->set_next_linked_list_node(
                     node->get_next_linked_list_node());
            } else {
                m_head = node->get_next_linked_list_node();
                
                if (m_head != nullptr) {
                    m_head->set_prev_linked_list_node(nullptr);
                }
            }
            
            if (next_node != nullptr) {
                next_node->set_prev_linked_list_node(
                     node->get_prev_linked_list_node());
            } else {
                m_tail = node->get_prev_linked_list_node();
                
                if (m_tail != nullptr) {
                    m_tail->set_next_linked_list_node(nullptr);
                }
            }
            
            delete node;
        }
    };
            
} // End of namespace net::coderodde::util.
} // End of namespace net::coderodde.
} // End of namespace net.


#endif // NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP
