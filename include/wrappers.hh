#ifndef WRAPPERS_HH
#define WRAPPERS_HH

#include <gsl/gsl_spline.h>
#include <gsl/gsl_roots.h>
#include <memory>
#include <vector>

struct spline_deleter
{
    void operator()(gsl_spline* ptr) const 
    {
        gsl_spline_free(ptr);
    }
};

struct root_fsolver_deleter
{
    void operator()(gsl_root_fsolver* ptr) const 
    {
        gsl_root_fsolver_free(ptr);
    }
};

typedef std::shared_ptr<gsl_spline> spline_ptr;
typedef std::unique_ptr<gsl_root_fsolver, root_fsolver_deleter> fsolver_ptr;

template<typename... _ArgsTp>
spline_ptr make_spline_ptr(_ArgsTp&&... args)
{
    gsl_spline* ptr = gsl_spline_alloc(std::forward<_ArgsTp>(args)...);
    return spline_ptr(ptr, spline_deleter());
}

template<typename... _ArgsTp>
fsolver_ptr make_fsolver_ptr(_ArgsTp&&... args)
{
    gsl_root_fsolver* ptr = gsl_root_fsolver_alloc(std::forward<_ArgsTp>(args)...);
    return fsolver_ptr(ptr);
}

#endif