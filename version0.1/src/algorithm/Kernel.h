// This is class implements the kernel(W) and its functions mentioned in paper.
// Kernel constants and equations are based on the following paper:
// https://nccastaff.bournemouth.ac.uk/jmacey/MastersProjects/MSc11/Rajiv/MasterThesis.pdf
//
// The Kernel includes four different symmetric kernels:
// 1. Poly6 : General interpolation (mass, density, normal, density-variance-scale and etc...)
// 2. Spiky : For interpolating pressures
// 3. Viscosity : For interpolating viscosity computations
// 4. Surface tension : For interpolating surface tensions
// Zihao Li 2016/4/16

#pragma once

#include "utils/Def.h"

namespace cs224 {

struct Kernel {
  
	// Declear the constant part of the kernel.
	// The constant part of the kernel is independent from
	// the particle geometry and physical status.
	// Since the efficiency of the kernel significantly
	// affect the efficiency of the simulation since each
	// iteration involves kernel computation, its 
	// good to pre-compute all constant values. 

	// Smoothing Length : represents "h" in the paper
	float smoothLength, squaredSmoothLength, halfSmoothLength;
	float pow6SmoothLength, pow9SmoothLength;

	// Poly6 kernel constants (3.12~3.14)
	// Poly6 Constant, Poly6 first/second order derivative constant.
	float poly6C, poly6Grad1, poly6Grad2;

	// Spiky kernel constants (3.15)
	float spikyGrad1;

	// Viscosity kernel constants (3.16)
	float viscosityGrad2; 

	// Surface tension constants 
	float surfaceTensionConstant,surfaceTensionOffset; 
    
    // Build the kernel
    void buildKernel(float h) {
    	smoothLength = h;
        squaredSmoothLength = pow2(smoothLength);
        halfSmoothLength = 0.5f * smoothLength;
        pow6SmoothLength = pow6(smoothLength);
        pow9SmoothLength = pow9(smoothLength);
        poly6C = 315.f / (64.f * PI * pow9SmoothLength);
        poly6Grad1 = -945.f / (32.f * PI * pow9SmoothLength);
        poly6Grad2 = -945.f / (32.f * PI * pow9SmoothLength);
        spikyGrad1 = -45.f / (PI * pow6SmoothLength);
        viscosityGrad2 = 45.f / (PI * pow6SmoothLength);
        
        surfaceTensionConstant = 32.f / (PI * pow9SmoothLength);
        surfaceTensionOffset = -pow6SmoothLength / 64.f;
    }


    // Below this part is the variant part of kernel.
    // The variant part of the kernel is based on the 
    // squared distance between the current particle and
    // referencing neighbour particle, and the vector <r - rn>.

    // Poly6 kernel (3.12)
    inline float poly6(float squaredR) const {
        return pow3(squaredSmoothLength - squaredR);
    }
    
    // First order gradiant variant term of poly6 kernel (3.13)
    inline Vector3f poly6Grad(const Vector3f &r, float squaredR) const {
    	return r * pow2(squaredSmoothLength - squaredR);
    }

    // Second order gradiant variant term of poly6 kernel (3.14)
    inline float poly6Laplace(float squaredR) const {
    	return (squaredSmoothLength - squaredR) * (3.f * squaredSmoothLength - 7.f * squaredR);
    }

    // Spiky kernel (3.15)
    inline Vector3f spikyGrad(const Vector3f &r, float normalizedR) const {
        return r * (1.f / normalizedR) * pow2(smoothLength - normalizedR);
    } 

    // Viscosity kernel (3.16)
    inline float viscosityLaplace(float normalizedR) const {
        return smoothLength - normalizedR;
    }

    // Surface tension
    inline float surfaceTension(float normalizedR) const {
        if (normalizedR < halfSmoothLength) {
            return 2.f * pow3(smoothLength - normalizedR) * pow3(normalizedR) + surfaceTensionOffset;
        } else {
            return pow3(normalizedR) * pow3(smoothLength - normalizedR);
        }
    }
};

}