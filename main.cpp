#include "ArrayProbabilityDistribution.hpp"
#include "LinkedListProbabilityDistribution.hpp"
#include <iostream>

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
}
