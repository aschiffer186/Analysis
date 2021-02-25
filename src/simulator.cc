#include "simulator.hh"

#include <algorithm>
#include <random>
#include <chrono>
#include "gsl/gsl_errno.h"
#include <iostream>
#include <stdexcept>


simulator::simulator(const std::vector<double>& data)
: unif(0,1)
{
    make_ecdf(data);
    //Initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
}

void simulator::make_ecdf(std::vector<double> data)
{
    std::sort(data.begin(), data.end());
    std::vector<double> xpts = {data.front() - 0.01};
    std::vector<double> ypts = {0};
    double count = 1;
    size_t num_points = data.size();
    for(size_t i = 0; i < num_points; ++i)
    {
        if (i == num_points - 1 || data[i + 1] != data[i])
        {
            xpts.push_back(data[i]);
            ypts.push_back(count/num_points);
        }
        count += 1;
    }
    _M_ecdf = {xpts, ypts};
}

std::vector<double> simulator::simulate(size_t num_sims, size_t num_people)
{
    std::vector<double> results(num_sims);

    for(size_t i = 0; i < num_sims; ++i)
    {
        double realization = 0;
        for(size_t j = 0; j < num_people; ++j)
        {
            double u = unif(rng);
            double sample = perform_sample(u);
            realization += sample;
        }
        results[i] = realization;
    }
    return results;
}

double simulator::perform_sample(double u)
{
    int status;
    fsolver_ptr ptr = make_fsolver_ptr(gsl_root_fsolver_brent);

    double r = 0, x_lo = _M_ecdf._M_xpts.front();
    double x_hi = _M_ecdf._M_xpts.back();

    spline_ptr spline = make_spline_ptr(gsl_interp_linear, _M_ecdf._M_xpts.size());
    gsl_spline_init(spline.get(), _M_ecdf._M_xpts.data(), _M_ecdf._M_cdf_pts.data(), _M_ecdf._M_xpts.size());
    func_params params = {u, spline};
    
    gsl_function F;
    F.function = &cdf_inv;
    F.params = &params;

    gsl_root_fsolver_set(ptr.get(), &F, x_lo, x_hi);
    int max_iter = 1000;
    int curr_iter = 0;
    do
    {
        ++curr_iter;
        status = gsl_root_fsolver_iterate(ptr.get());
        r = gsl_root_fsolver_root(ptr.get());
        x_lo = gsl_root_fsolver_x_lower (ptr.get());
        x_hi = gsl_root_fsolver_x_upper (ptr.get());
        status = gsl_root_test_interval (x_lo, x_hi,
                                       0, 0.001);
        if (status == GSL_SUCCESS)
            return r;
    } while (status == GSL_CONTINUE && curr_iter < max_iter);
    throw std::runtime_error("Error: root finder did not converge");
}   

double simulator::cdf_inv(double x, void* p)
{
    func_params* params = static_cast<func_params*>(p);
    double val = cdf_func(x, params->_M_spline.get()) - params->_M_u;
    return val;
    
}

double simulator::cdf_func(double x, gsl_spline* spline)
{
    gsl_interp_accel* accel = gsl_interp_accel_alloc();
    double result = gsl_spline_eval(spline, x, accel);
    gsl_interp_accel_free(accel);
    return result;
}