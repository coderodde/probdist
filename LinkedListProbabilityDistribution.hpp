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

        class LinkedListNode {
        private:
            
            T               m_element;
            double          m_weight;
            LinkedListNode* m_prev_node;
            LinkedListNode* m_next_node;
            
        public:
            
            LinkedListNode(T element, double weight) {
                m_element = element;
                m_weight  = weight;
            }
            
            T get_element() const {
                return m_element;
            }
            
            double get_weight() const {
                return m_weight;
            }
            
            LinkedListNode* get_prev_linked_list_node() const {
                return m_prev_node;
            }
            
            LinkedListNode* get_next_linked_list_node() const {
                return m_next_node;
            }
            
            void set_prev_linked_list_node(LinkedListNode* node) {
                m_prev_node = node;
            }
            
            void set_next_linked_list_node(LinkedListNode* node) {
                m_next_node = node;
            }
        };
        
    public:
        LinkedListProbabilityDistribution()
        :
        ProbabilityDistribution<T>{},
        m_head{nullptr},
        m_tail{nullptr}
        {}
        
        LinkedListProbabilityDistribution(std::random_device::result_type seed)
        :
        ProbabilityDistribution<T>{seed},
        m_head{nullptr},
        m_tail{nullptr}
        {}
        
        LinkedListProbabilityDistribution(
            const LinkedListProbabilityDistribution<T>& other) {
            this->m_size             = other.m_size;
            this->m_total_weight     = other.m_total_weight;
            
            // Copy the internal linked list:
            copy_linked_list(other.m_head);
        }
        
        LinkedListProbabilityDistribution(
            LinkedListProbabilityDistribution<T>&& other) {
            this->m_size             = other.m_size;
            this->m_total_weight     = other.m_total_weight;
            m_map                    = std::move(other.m_map);
            m_head                   = other.m_head;
            m_tail                   = other.m_tail;
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
            other.m_head         = nullptr;
            other.m_tail         = nullptr;
        }
        
        LinkedListProbabilityDistribution& operator=(
            const LinkedListProbabilityDistribution<T>& other) {
            delete_linked_list();
            copy_linked_list(other.m_head);
            
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            return *this;
        }
        
        LinkedListProbabilityDistribution& operator=(
            LinkedListProbabilityDistribution<T>&& other) {
            if (this == &other) {
                return *this;
            }
            
            delete_linked_list();
            
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            this->m_head         = other.m_head;
            this->m_tail         = other.m_tail;
            this->m_map          = std::move(other.m_map);
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
            other.m_head         = nullptr;
            other.m_tail         = nullptr;
            
            return *this;
        }

        ~LinkedListProbabilityDistribution() {
            delete_linked_list();
        }
        
        virtual bool add_element(T const& element, double weight) {
            if (m_map.find(element) != m_map.end()) {
                return false;
            }
            
            this->check_weight(weight);
            LinkedListNode* new_node = new LinkedListNode{element, weight};
            
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
            
            for (LinkedListNode* node = m_head;
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
            
            LinkedListNode* node = m_map[element];
            
            m_map.erase(element);
            this->m_size--;
            this->m_total_weight -= node->get_weight();
            unlink(node);
            delete node;
            return true;
        }
                
        virtual void clear() {
            this->m_size = 0;
            this->m_total_weight = 0.0;
            m_map.clear();
            delete_linked_list();
            m_head = nullptr;
            m_tail = nullptr;
        }
                
    private:
        std::unordered_map<T, LinkedListNode*> m_map;
        LinkedListNode* m_head;
        LinkedListNode* m_tail;
        
        void unlink(LinkedListNode* node) {
            LinkedListNode* prev_node = node->get_prev_linked_list_node();
            LinkedListNode* next_node = node->get_next_linked_list_node();
            
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
        }
        
        void delete_linked_list() {
            for (LinkedListNode* node = m_head, *next; node != nullptr;) {
                next = node->get_next_linked_list_node();
                delete node;
                node = next;
            }
        }
        
        void copy_linked_list(LinkedListNode* source_head) {
            if (source_head == nullptr) {
                m_head = nullptr;
                m_tail = nullptr;
                return;
            }
            
            m_head = m_tail = new LinkedListNode{source_head->get_element(),
                                                 source_head->get_weight()};
            
            m_head->set_prev_linked_list_node(nullptr);
            
            for (LinkedListNode* node =
                    source_head->get_next_linked_list_node();
                 node != nullptr;
                 node = node->get_next_linked_list_node()) {
                LinkedListNode* new_node =
                    new LinkedListNode(node->get_element(),
                                          node->get_weight());
                
                m_tail->set_next_linked_list_node(new_node);
                new_node->set_prev_linked_list_node(m_tail);
                m_tail = new_node;
                
                m_map[new_node->get_element()] = new_node;
            }
            
            m_tail->set_next_linked_list_node(nullptr);
        }
    };
            
} // End of namespace net::coderodde::util.
} // End of namespace net::coderodde.
} // End of namespace net.


#endif // NET_CODERODDE_UTIL_LINKED_LIST_PROBABILITY_DISTRIBUTION_HPP
