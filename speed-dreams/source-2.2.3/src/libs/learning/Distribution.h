/* -*- Mode: C++; -*- */
/* VER: $Id: Distribution.h 2917 2010-10-17 19:03:40Z pouillot $*/
// copyright (c) 2004 by Christos Dimitrakakis <dimitrak@idiap.ch>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <cmath>
#include <iostream>

#include "learning.h"
#include "real.h"


LEARNING_API void setRandomSeed(unsigned int seed);
LEARNING_API real urandom();
LEARNING_API real urandom(real min, real max);

/// Probability distribution
class LEARNING_API Distribution {
 public:
	Distribution() {}
	virtual ~Distribution() {}
	virtual real generate() = 0; ///< generate a value from this distribution
	virtual real pdf(real x) = 0; ///< return the density at point x
};

/// \brief Parametric distribution.
/// For distribution simply defined via moments-related parameters.
class LEARNING_API ParametricDistribution : public Distribution {
public:
	ParametricDistribution() {}
	virtual ~ParametricDistribution() {}
	virtual void setVariance (real var) = 0; ///< set the variance
 	virtual void setMean (real mean) = 0; ///< set the mean
};


/// Discrete probability distribution
class LEARNING_API DiscreteDistribution : public Distribution {
public:
	int n_outcomes; ///< number of possible outcomes
	real* p; ///< probabilities of outcomes
	DiscreteDistribution();
	/// Make a discrete distribution with N outcomes
	DiscreteDistribution(int N); 
	virtual ~DiscreteDistribution();
	virtual real generate();
	virtual real pdf(real x);
};

/// Gaussian probability distribution
class LEARNING_API NormalDistribution : public ParametricDistribution {
private:
	bool cache;
	real normal_x, normal_y, normal_rho;
public:
	real m; ///< mean
	real s; ///< standard deviation
	NormalDistribution() {m=0.0; s=1.0; cache = false;}
	/// Normal dist. with given mean and std
	NormalDistribution(real mean, real std)
	{
		setMean (mean);
		setVariance (std*std);
	}
	virtual ~NormalDistribution() {std::cout << "Normal dist destroyed\n";}
	virtual real generate();
	virtual real pdf(real x);
	virtual void setVariance (real var) 
	{s = sqrt(var);} 
	virtual void setMean (real mean)
	{m = mean;}
};

/// Uniform probability distribution
class LEARNING_API UniformDistribution : public ParametricDistribution {
public:
	real m; ///< mean
	real s; ///< standard deviation
	UniformDistribution() {m=0.0; s=1.0;}
	/// Create a uniform distribution with mean \c mean and standard deviation \c std
	UniformDistribution(real mean, real std)
	{
		setMean (mean);
		setVariance (std*std);
	}
	virtual ~UniformDistribution() {}
	virtual real generate();
	virtual real pdf(real x);
	virtual void setVariance (real var) 
	{s = sqrt(12.0f * var);} 
	virtual void setMean (real mean)
	{m = mean;}
};


/// Laplacian probability distribution
class LEARNING_API LaplacianDistribution : public ParametricDistribution {
public:
	real l; ///< lambda
	real m; ///< mean
	LaplacianDistribution() {m=0.0;l=1.0;}
	/// Create a Laplacian distribution with parameter \c lambda
	LaplacianDistribution(real lambda)
	{
		m = 0.0;
		l = lambda;
	}
	LaplacianDistribution(real mean, real var)
	{
		setMean (mean);
		setVariance (var);
	}
	virtual ~LaplacianDistribution() {}
	virtual real generate();
	virtual real pdf(real x);
	virtual void setVariance (real var)
	{l = sqrt(0.5f / var);}
	virtual void setMean (real mean)
	{m = mean;}
};

/// Exponential probability distribution
class LEARNING_API ExponentialDistribution : public ParametricDistribution {
public:
	real l; ///< lambda
	real m; ///< mean
	ExponentialDistribution() {m=0.0; l=1.0;}
	/// Create an exponential distribution with parameter \c lambda
	ExponentialDistribution(real lambda)
	{
		l = lambda;
	}
	ExponentialDistribution(real mean, real var)
	{
		setMean(mean);
		setVariance(var);
	}
	virtual ~ExponentialDistribution() {}
	virtual real generate();
	virtual real pdf(real x);
	virtual void setVariance (real var)
	{l = sqrt(1.0f / var);}
	virtual void setMean (real mean)
	{m = mean;}
};

/// Multinomial gaussian probability distribution
// class LEARNING_API MultinomialGaussian : public Distribution {
// public:
// 	int d; ///< number of dimensions
// 	MultinomialGaussian() {d=1;}
// 	/// create a multinomial with \c d dimensions
// 	MultinomialGaussian(int d)
// 	{
// 		this->d = d;
// 	}
// 	virtual ~MultinomialGaussian() {}
// 	virtual real generate();
// 	virtual real pdf(real x);
// };


#endif
