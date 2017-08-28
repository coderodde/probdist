#ifndef NET_CODERODDE_UTIL_PROBABILITY_DISTRIBUTION_HPP
#define NET_CODERODDE_UTIL_PROBABILITY_DISTRIBUTION_HPP

#include <cmath>
#include <random>
#include <sstream>
#include <stdexcept>

namespace net {
namespace coderodde {
namespace util {
    
    template<typename T>
    class ProbabilityDistribution {
    public:
        ProbabilityDistribution(std::random_device::result_type seed)
        :
        m_size{0},
        m_total_weight{0.0},
        m_generator{seed},
        m_real_distribution{0.0, 1.0}
        {}
        
        ProbabilityDistribution()
        :
        m_size{0},
        m_total_weight{0.0},
        m_generator{},
        m_real_distribution{0.0, 1.0}
        {}
        
        virtual bool   is_empty       ()                          const = 0;
        virtual size_t size           ()                          const = 0;
        virtual bool   add_element    (T const& element,
                                        double weight)                  = 0;
        virtual T&     sample_element ()                                = 0;
        virtual bool   contains       (T const& element)          const = 0;
        virtual bool   remove         (T const& element)                = 0;
        virtual void   clear          ()                                = 0;

    protected:
        
        size_t                                 m_size;
        double                                 m_total_weight;
        std::uniform_real_distribution<double> m_real_distribution;
        std::mt19937                           m_generator;
        
        void check_weight(double weight) {
            if (std::isnan(weight)) {
                throw std::invalid_argument("The input weight is NaN.");
            }
                
            if (weight <= 0.0) {
                std::stringstream ss;
                ss << "The input weight is non-positive: " << weight << ".";
                throw std::invalid_argument(ss.str());
            }
                
            if (std::isinf(weight)) {
                throw std::invalid_argument(
                                    "The input weight is positive infinity.");
            }
        }
        
        void check_not_empty() const {
            if (is_empty()) {
                throw std::length_error{
                    "This probability distribution is empty."
                };
            }
        }
    };
    
} // End of namespace net::coderodde::util.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_UTIL_PROBABILITY_DISTRIBUTION_HPP
