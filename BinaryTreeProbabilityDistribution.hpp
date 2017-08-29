#ifndef NET_CODERODDE_UTIL_BINARY_TREE_PROBABILITY_DISTRIBUTION_HPP
#define NET_CODERODDE_UTIL_BINARY_TREE_PROBABILITY_DISTRIBUTION_HPP

#include "ProbabilityDistribution.hpp"
#include <unordered_map>
#include <utility>

namespace net {
namespace coderodde {
namespace util {
    
    template<typename T>
    class BinaryTreeProbabilityDistribution :
    public ProbabilityDistribution<T> {
    private:
        
        class TreeNode {
        private:
            
            T         m_element;
            double    m_weight;
            bool      m_is_relay_node;
            TreeNode* m_left_child;
            TreeNode* m_right_child;
            TreeNode* m_parent;
            size_t    m_leaf_node_count;
            
        public:
            
            TreeNode(T element, double weight)
            :
            m_element{element},
            m_weight{weight},
            m_is_relay_node{false},
            m_leaf_node_count{1},
            m_left_child{nullptr},
            m_right_child{nullptr},
            m_parent{nullptr}
            {}
            
            TreeNode()
            :
            m_element{},
            m_weight{},
            m_is_relay_node{true},
            m_leaf_node_count{},
            m_left_child{nullptr},
            m_right_child{nullptr},
            m_parent{nullptr}
            {}
            
            T get_element() const {
                return m_element;
            }
            
            double get_weight() const {
                return m_weight;
            }
            
            void set_weight(double weight) {
                m_weight = weight;
            }
            
            size_t get_number_of_leaves() const {
                return m_leaf_node_count;
            }
            
            void set_number_of_leaves(size_t leaf_node_count) {
                m_leaf_node_count = leaf_node_count;
            }
            
            TreeNode* get_left_child() const {
                return m_left_child;
            }
            
            void set_left_child(TreeNode* node) {
                m_left_child = node;
            }
            
            TreeNode* get_right_child() const {
                return m_right_child;
            }
            
            void set_right_child(TreeNode* node) {
                m_right_child = node;
            }
            
            TreeNode* get_parent() const {
                return m_parent;
            }
            
            void set_parent(TreeNode* node) {
                m_parent = node;
            }
            
            bool is_relay_node() const {
                return m_is_relay_node;
            }
            
            bool is_leaf_node() const {
                return !m_is_relay_node;
            }
        };
        
    public:
        
        BinaryTreeProbabilityDistribution()
        :
        BinaryTreeProbabilityDistribution(std::random_device::result_type{})
        {}
        
        BinaryTreeProbabilityDistribution(std::random_device::result_type seed)
        :
        ProbabilityDistribution<T>{seed},
        m_root{nullptr}
        {}
        
        BinaryTreeProbabilityDistribution(
            const BinaryTreeProbabilityDistribution<T>& other) {
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            
            // Copy the internal tree:
            copy_tree(other.m_root);
        }
        
        BinaryTreeProbabilityDistribution(
            BinaryTreeProbabilityDistribution<T>&& other) {
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            
            m_map  = std::move(other.m_map);
            m_root = other.m_root;
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
            other.m_root         = nullptr;
        }
        
        BinaryTreeProbabilityDistribution& operator=(
            const BinaryTreeProbabilityDistribution<T>& other) {
            if (this == &other) {
                return *this;
            }
            
            delete_tree();
            copy_tree(other.m_root);
            
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            return *this;
        }
        
        BinaryTreeProbabilityDistribution& operator=(
            BinaryTreeProbabilityDistribution<T>&& other) {
            if (this == &other) {
                return *this;
            }
            
            delete_tree();
            
            this->m_size         = other.m_size;
            this->m_total_weight = other.m_total_weight;
            this->m_root         = other.m_root;
            this->m_map          = std::move(other.m_map);
            
            other.m_size         = 0;
            other.m_total_weight = 0.0;
            other.m_root         = nullptr;
            
            return *this;
        }
        
        virtual bool add_element(T const& element, double weight) {
            if (m_map.find(element) != m_map.end()) {
                return false;
            }
            
            this->check_weight(weight);
            TreeNode* new_node = new TreeNode{element, weight};
            insert(new_node);
            this->m_size++;
            this->m_total_weight += weight;
            m_map[element] = new_node;
            return true;
        }
        
        virtual bool contains_element(T const& element) const {
            return m_map.find(element) != m_map.end();
        }
        
        virtual T sample_element() {
            this->check_not_empty();
            double value = this->m_real_distribution(this->m_generator) *
                           this->m_total_weight;
            
            TreeNode* node = m_root;
            
            while (node->is_relay_node()) {
                if (value < node->get_left_child()->get_weight()) {
                    node = node->get_left_child();
                } else {
                    value -= node->get_left_child()->get_weight();
                    node = node->get_right_child();
                }
            }
            
            return node->get_element();
        }
        
        virtual bool remove_element(T const& element) {
            if (!contains_element(element)) {
                return false;
            }
            
            TreeNode* node = m_map[element];
            delete_node(node);
            m_map.erase(element);
            update_metadata(node->get_parent(), -node->get_weight(), -1);
            this->m_size--;
            this->m_total_weight -= node->get_weight();
            return true;
        }
        
        virtual void clear() {
            delete_tree();
            m_map.clear();
            
            m_root               = nullptr;
            this->m_size         = 0;
            this->m_total_weight = 0.0;
        }
        
    private:
        
        void delete_node(TreeNode* node) {
            TreeNode* relay_node = node->get_parent();
            
            if (relay_node == nullptr) {
                m_root = nullptr;
                return;
            }
            
            TreeNode* parent_of_relay_node = relay_node->get_parent();
            TreeNode* sibling_leaf = relay_node->get_left_child() == node ?
                                     relay_node->get_right_child() :
                                     relay_node->get_left_child();
            
            if (parent_of_relay_node == nullptr) {
                m_root = sibling_leaf;
                sibling_leaf->set_parent(nullptr);
                return;
            }
            
            if (parent_of_relay_node->get_left_child() == relay_node) {
                parent_of_relay_node->set_left_child(sibling_leaf);
            } else {
                parent_of_relay_node->set_right_child(sibling_leaf);
            }
            
            sibling_leaf->set_parent(parent_of_relay_node);
        }
        
        void update_metadata(TreeNode* node,
                             double weight_delta,
                             size_t node_count_delta) {
            while (node != nullptr) {
                node->set_number_of_leaves(
                            node->get_number_of_leaves() + node_count_delta);
                node->set_weight(node->get_weight() + weight_delta);
                node = node->get_parent();
            }
        }
        
        void bypass_leaf_node(TreeNode* bypass_node, TreeNode* new_node) {
            TreeNode* relay_node = new TreeNode{};
            TreeNode* parent_of_current_node = bypass_node->get_parent();
            
            relay_node->set_number_of_leaves(1);
            relay_node->set_weight(bypass_node->get_weight());
            relay_node->set_left_child(bypass_node);
            relay_node->set_right_child(new_node);
            
            bypass_node->set_parent(relay_node);
            new_node->set_parent(relay_node);
            
            if (parent_of_current_node == nullptr) {
                m_root = relay_node;
            } else if (parent_of_current_node->get_left_child()
                       == bypass_node) {
                relay_node->set_parent(parent_of_current_node);
                parent_of_current_node->set_left_child(relay_node);
            } else {
                relay_node->set_parent(parent_of_current_node);
                parent_of_current_node->set_right_child(relay_node);
            }
            
            update_metadata(relay_node, new_node->get_weight(), 1);
        }
        
        void insert(TreeNode* new_node) {
            if (m_root == nullptr) {
                m_root = new_node;
                new_node->set_parent(nullptr);
                new_node->set_left_child(nullptr);
                new_node->set_right_child(nullptr);
                return;
            }
            
            TreeNode* current_node = m_root;
            
            while (current_node->is_relay_node()) {
                if (current_node->get_left_child()->get_number_of_leaves() <
                    current_node->get_right_child()->get_number_of_leaves()) {
                    current_node = current_node->get_left_child();
                } else {
                    current_node = current_node->get_right_child();
                }
            }
            
            bypass_leaf_node(current_node, new_node);
        }
        
        void delete_tree(TreeNode* node) {
            if (node == nullptr) {
                return;
            }
            
            delete_tree(node->get_left_child());
            delete_tree(node->get_right_child());
            delete node;
        }
        
        void delete_tree() {
            delete_tree(m_root);
            m_root = nullptr;
        }
        
        TreeNode* copy_tree_impl(TreeNode* node) {
            if (node == nullptr) {
                return nullptr;
            }
            
            TreeNode* new_node = new TreeNode{node->get_element(),
                                              node->get_weight()};
            
            m_map[new_node->get_element()] = new_node;
            
            new_node->set_left_child (copy_tree_impl(node->get_left_child()));
            new_node->set_right_child(copy_tree_impl(node->get_right_child()));
            return new_node;
        }
        
        void copy_tree(TreeNode* copy_root) {
            m_root = copy_tree_impl(copy_root);
        }
        
        std::unordered_map<T, TreeNode*> m_map;
        TreeNode* m_root;
    };
    
} // End of namespace net::coderodde::util.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_UTIL_BINARY_TREE_PROBABILITY_DISTRIBUTION_HPP
