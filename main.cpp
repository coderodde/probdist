#include "ArrayProbabilityDistribution.hpp"
#include <iostream>

int main() {
    using net::coderodde::util::ArrayProbabilityDistribution;
    
    std::random_device rd{};
    std::random_device::result_type seed = rd();
    ArrayProbabilityDistribution<int> prob_dist{seed};
    
    prob_dist.add_element(1, 1.0);
    prob_dist.add_element(2, 1.0);
    prob_dist.add_element(3, 3.0);
    
    int arr[4];
    
    for (int i = 0; i < 1000; ++i) {
        arr[prob_dist.sample_element()]++;
    }
    
    for (int i = 1; i < 4; ++i) {
        std::cout << arr[i] << "\n";
    }
}
