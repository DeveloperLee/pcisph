#pragma once

#include "core/Common.h"
#include "core/Vector.h"

namespace pbs {

// Implementation is based on paper:
// https://nccastaff.bournemouth.ac.uk/jmacey/MastersProjects/MSc11/Rajiv/MasterThesis.pdf
// SPH Kernel class
// The Kernel includes three different symmetric kernels:
// 1. Poly6 : General interpolation
// 2. Spiky : For interpolating pressures
// 3. Viscosity : For interpolating viscosity computations
struct Kernel {
    
    // Declear the constant part of the kernel.
    // The constant part of the kernel is independent from
    // the particle geometry and physical status.
    float h;
    float h2;
    float halfh, pow6h, pow9h;  
    float poly6C, poly6Grad1, poly6Grad2; // Poly6 params
    float spikyC, spikyGrad1, spikyGrad2; // Spiky params
    float viscosityGrad2;                // Viscosity params
    float surfaceTensionConstant,surfaceTensionOffset; // Surface tension params
    
    // Initialize kernel
    void init(float h_) {
        h = h_;
        h2 = sqr(h);
        halfh = 0.5f * h;
        pow6h = std::pow(h, 6.f);
        pow9h = std::pow(h, 9.f);
        poly6C = 315.f / (64.f * M_PI * pow9h);
        poly6Grad1 = -945.f / (32.f * M_PI * pow9h);
        poly6Grad2 = -945.f / (32.f * M_PI * pow9h);
        
        spikyC = 15.f / (M_PI * pow6h);
        spikyGrad1 = -45.f / (M_PI * pow6h);
        spikyGrad2 = -90.f / (M_PI * pow6h);
        viscosityGrad2 = 45.f / (M_PI * pow6h);
        
        surfaceTensionConstant = 32.f / (M_PI * pow9h);
        surfaceTensionOffset = -pow6h / 64.f;
    }
    
    // Below this part is the variant part of SPH kernel.
    // The variant part of the kernel is based on the 
    // squared distance between the current particle and one
    // of its selected neighbour particle as well as the vector
    // that connects them.
    
    
    // Poly6 kernel 
    inline float poly6(float r2) const {
        return cube(h2 - r2);
    }
    inline Vector3f poly6Grad(const Vector3f &r, float r2) const {
        return sqr(h2 - r2) * r;
    }
    inline float poly6Laplace(float r2) {
        return (h2 - r2) * (3.f * h2 - 7.f * r2);
    }
    
    // Spiky kernel 
    inline float spiky(float rn) const {
        return cube(h - rn);
    }
    inline Vector3f spikyGrad(const Vector3f &r, float rn) const {
        return sqr(h - rn) * r * (1.f / rn);
    }
    inline float spikyLaplace(float rn) const {
        return (h - rn) * (h - 2.f * rn) / rn;
    }
    
    // Viscosity kernel
    inline float viscosityLaplace(float rn) const {
        return (h - rn);
    }
    
    // Surface tension kernel
    inline float surfaceTension(float rn) const {
        if (rn < halfh) {
            return 2.f * cube(h - rn) * cube(rn) + surfaceTensionOffset;
        } else {
            return cube(h - rn) * cube(rn);
        }
    }
};

} // namespace pbs
