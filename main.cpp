#include "ArrayProbabilityDistribution.hpp"
#include "LinkedListProbabilityDistribution.hpp"
#include "ProbabilityDistribution.hpp"
#include "assert.hpp"
#include <iostream>

using net::coderodde::util::ProbabilityDistribution;
using net::coderodde::util::ArrayProbabilityDistribution;
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
}

static void test_tree() {
    
}
