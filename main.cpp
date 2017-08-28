#include "ArrayProbabilityDistribution.hpp"
#include "BinaryTreeProbabilityDistribution.hpp"
#include "LinkedListProbabilityDistribution.hpp"
#include "ProbabilityDistribution.hpp"
#include "assert.hpp"
#include <iostream>

using net::coderodde::util::ProbabilityDistribution;
using net::coderodde::util::ArrayProbabilityDistribution;
using net::coderodde::util::BinaryTreeProbabilityDistribution;
using net::coderodde::util::LinkedListProbabilityDistribution;

static void test_all();

int main() {
    using net::coderodde::util::ArrayProbabilityDistribution;
    using net::coderodde::util::LinkedListProbabilityDistribution;
    
    std::random_device rd{};
    std::random_device::result_type seed = rd();
    ArrayProbabilityDistribution<int> prob_dist1{seed};
    LinkedListProbabilityDistribution<int> prob_dist2{};
    
    prob_dist1.add_element(1, 1.0);
    prob_dist1.add_element(2, 1.0);
    prob_dist1.add_element(3, 3.0);
    
    prob_dist2.add_element(1, 1.0);
    prob_dist2.add_element(2, 1.0);
    prob_dist2.add_element(3, 3.0);
    
    int arr1[4];
    int arr2[4];
    
    for (int i = 0; i < 1000; ++i) {
        arr1[prob_dist1.sample_element()]++;
        arr2[prob_dist2.sample_element()]++;
    }
    
    for (int i = 1; i < 4; ++i) {
        std::cout << arr1[i] << "\n";
    }
    
    for (int i = 1; i < 4; ++i) {
        std::cout << arr2[i] << "\n";
    }
    
    test_all();
    REPORT
}

static void test_array();
static void test_linked_list();
static void test_tree();

static void test_all() {
    test_array();
    test_linked_list();
    test_tree();
}

static void test_impl(ProbabilityDistribution<int>* dist) {
    ASSERT(dist->is_empty());
    
    for (int i = 0; i < 4; ++i) {
        ASSERT(dist->size() == i);
        dist->add_element(i, 1.0);
        ASSERT(dist->size() == i + 1);
    }
    
    ASSERT(dist->is_empty() == false);
    
    for (int i = 0; i < 4; ++i) {
        ASSERT(dist->contains(i));
    }
    
    ASSERT(dist->contains(-1) == false);
    
    for (int i = 4; i < 10; ++i) {
        ASSERT(dist->contains(i) == false);
    }
    
    for (int i = 0; i < 4; ++i) {
        ASSERT(dist->add_element(i, 2.0) == false);
    }
    
    for (int i = 0; i < 4; ++i) {
        ASSERT(dist->remove(i));
    }
    
    for (int i = 0; i < 4; ++i) {
        ASSERT(dist->remove(i) == false);
    }
    
    try {
        dist->sample_element();
        FAIL("std::length_error expected.");
    } catch (std::length_error err) {}
    
    for (int i = 0; i < 4; ++i) {
        dist->add_element(i, 2.0);
    }
    
    ASSERT(dist->size() == 4);
    dist->clear();
    ASSERT(dist->size() == 0);
}

static void test_array() {
    test_impl(new ArrayProbabilityDistribution<int>);
    
    ArrayProbabilityDistribution<int> dist1;
    ArrayProbabilityDistribution<int> dist2;
    
    for (int i = 0; i < 3; ++i) {
        dist2.add_element(i, 1.0);
    }
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist2.size() == 3);
    
    dist1 = dist2;
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    
    ArrayProbabilityDistribution<int> dist3(dist1);
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    ASSERT(dist3.size() == 3);
    
    ArrayProbabilityDistribution<int> dist4;
    dist4 = std::move(dist1);
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist4.size() == 3);
    
    ArrayProbabilityDistribution<int> dist5(std::move(dist2));
    
    ASSERT(dist5.size() == 3);
    ASSERT(dist2.size() == 0);
    
    dist1.clear();
    dist2.clear();
    
    ASSERT(dist1.is_empty());
    ASSERT(dist2.is_empty());
    
    for (int i = 10; i < 15; ++i) {
        dist1.add_element(i, 1.5);
    }
    
    // Test move assignment:
    dist2 = std::move(dist1);
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist2.contains(i));
        ASSERT(dist1.contains(i) == false);
    }
    
    // Test move constructor:
    ArrayProbabilityDistribution<int> dist6(std::move(dist2));
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist2.contains(i) == false);
    }
    
    // Test copy constructor:
    ArrayProbabilityDistribution<int> dist7(dist6);
    dist7.remove(14);
    
    for (int i = 10; i < 14; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist7.contains(i));
    }
    
    ASSERT(dist6.contains(14));
    ASSERT(dist7.contains(14) == false);
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist7.size() == 4);
    
    // Test copy assignment:
    dist1.clear();
    dist1 = dist6;
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 5);
    
    ASSERT(dist1.remove(11));
    ASSERT(dist1.remove(13));
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 3);
}

static void test_linked_list() {
    test_impl(new LinkedListProbabilityDistribution<int>);
    
    LinkedListProbabilityDistribution<int> dist1;
    LinkedListProbabilityDistribution<int> dist2;
    
    for (int i = 0; i < 3; ++i) {
        dist2.add_element(i, 1.0);
    }
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist2.size() == 3);
    
    dist1 = dist2;
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    
    LinkedListProbabilityDistribution<int> dist3(dist1);
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    ASSERT(dist3.size() == 3);
    
    LinkedListProbabilityDistribution<int> dist4;
    dist4 = std::move(dist1);
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist4.size() == 3);
    
    LinkedListProbabilityDistribution<int> dist5(std::move(dist2));
    
    ASSERT(dist5.size() == 3);
    ASSERT(dist2.size() == 0);
    
    dist1.clear();
    dist2.clear();
    
    ASSERT(dist1.is_empty());
    ASSERT(dist2.is_empty());
    
    for (int i = 10; i < 15; ++i) {
        dist1.add_element(i, 1.5);
    }
    
    // Test move assignment:
    dist2 = std::move(dist1);
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist2.contains(i));
        ASSERT(dist1.contains(i) == false);
    }
    
    // Test move constructor:
    LinkedListProbabilityDistribution<int> dist6(std::move(dist2));
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist2.contains(i) == false);
    }
    
    // Test copy constructor:
    LinkedListProbabilityDistribution<int> dist7(dist6);
    dist7.remove(14);
    
    for (int i = 10; i < 14; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist7.contains(i));
    }
    
    ASSERT(dist6.contains(14));
    ASSERT(dist7.contains(14) == false);
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist7.size() == 4);
    
    // Test copy assignment:
    dist1.clear();
    dist1 = dist6;
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 5);
    
    ASSERT(dist1.remove(11));
    ASSERT(dist1.remove(13));
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 3);

}

static void test_tree() {
    test_impl(new BinaryTreeProbabilityDistribution<int>);
    
    
    BinaryTreeProbabilityDistribution<int> dist1;
    BinaryTreeProbabilityDistribution<int> dist2;
    
    for (int i = 0; i < 3; ++i) {
        dist2.add_element(i, 1.0);
    }
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist2.size() == 3);
    
    dist1 = dist2;
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    
    BinaryTreeProbabilityDistribution<int> dist3(dist1);
    
    ASSERT(dist1.size() == 3);
    ASSERT(dist2.size() == 3);
    ASSERT(dist3.size() == 3);
    
    BinaryTreeProbabilityDistribution<int> dist4;
    dist4 = std::move(dist1);
    
    ASSERT(dist1.size() == 0);
    ASSERT(dist4.size() == 3);
    
    BinaryTreeProbabilityDistribution<int> dist5(std::move(dist2));
    
    ASSERT(dist5.size() == 3);
    ASSERT(dist2.size() == 0);
    
    dist1.clear();
    dist2.clear();
    
    ASSERT(dist1.is_empty());
    ASSERT(dist2.is_empty());
    
    for (int i = 10; i < 15; ++i) {
        dist1.add_element(i, 1.5);
    }
    
    // Test move assignment:
    dist2 = std::move(dist1);
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist2.contains(i));
        ASSERT(dist1.contains(i) == false);
    }
    
    // Test move constructor:
    BinaryTreeProbabilityDistribution<int> dist6(std::move(dist2));
    
    for (int i = 10; i < 15; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist2.contains(i) == false);
    }
    
    // Test copy constructor:
    BinaryTreeProbabilityDistribution<int> dist7(dist6);
    dist7.remove(14);
    
    for (int i = 10; i < 14; ++i) {
        ASSERT(dist6.contains(i));
        ASSERT(dist7.contains(i));
    }
    
    ASSERT(dist6.contains(14));
    ASSERT(dist7.contains(14) == false);
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist7.size() == 4);
    
    // Test copy assignment:
    dist1.clear();
    dist1 = dist6;
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 5);

    ASSERT(dist1.remove(11));
    ASSERT(dist1.remove(13));
    
    ASSERT(dist6.size() == 5);
    ASSERT(dist1.size() == 3);
}
