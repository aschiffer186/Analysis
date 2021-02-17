#ifndef SIMULATOR_HH
#define SIMULATOR_HH

#include <vector>
#include <array>
#include "wrappers.hh"
#include <random>
#include <chrono>

struct ecdf_t
{
    std::vector<double> _M_xpts;
    std::vector<double> _M_cdf_pts;
};

class simulator
{
    private:
        struct func_params
        {
            double _M_u;
            spline_ptr _M_spline;
        };
    public:
        //Creates a new simulator that will provide a predictive model for the given set 
        //data points. 
        //
        //@param data the data to use for the simulation
        simulator(const std::vector<double>& data);

        std::vector<double> simulate(size_t num_sims, size_t num_people);

        template<typename _FuncTp, typename... _ArgsTp>
        std::vector<std::vector<double>> simulate(size_t num_sims, size_t num_people, _FuncTp coupled, _ArgsTp&&... args)
        {
            std::vector<double> results(num_sims);
            std::vector<double> matching(num_sims);

            for(size_t i = 0; i < num_sims; ++i)
            {
                double realization = 0;
                double matching_realization = 0;
                for(size_t j = 0; j < num_people; ++j)
                {
                    double u = unif(rng);
                    double sample = perform_sample(u);
                    realization += sample;
                    u = unif(rng);
                    matching_realization += coupled(sample, u, std::forward<_ArgsTp>(args)...);
                }
                results[i] = realization;
                matching[i] = matching_realization;
            }
            return {results, matching};
        }
    private:
        void make_ecdf(std::vector<double> data);
        double perform_sample(double u);
        static double cdf_inv(double x, void* p);
        static double cdf_func(double x, gsl_spline* s);
    private:
        std::uniform_real_distribution<double> unif;
        std::mt19937_64 rng;
        ecdf_t _M_ecdf;
};

#endif