/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file module_Lpf.cpp
 * @brief Implementation of the Lpf (Low Pass Filter) class methods.
 * @author Yoon-Ki Hong
 * @date 2026-11-18
 */

#include "Lpf.h"
#include <util/runtime.h>

/**
 * @details Initializes a new `Lpf` instance with a user-specified filter convergence ratio.
 *          Resets the internal microsecond elapsed timer (`mTime`) and initializes the filtered
 *          output state (`mData`) to `0.0f`.
 * @see Lpf::setRatio, ElapsedTime::reset
 * @todo none
 * @bug none
 */
Lpf::Lpf(float ratio)
{
	// Store user-defined filter convergence rate coefficient
	mRatio = ratio;
	// Reset internal elapsed timer to current hardware timestamp
	mTime.reset();
	// Initialize internal filtered state to zero
	mData = 0;
}

/**
 * @details Initializes a new `Lpf` instance with a default filter ratio of `1.0f`.
 *          Resets the internal microsecond elapsed timer (`mTime`) and sets the filtered
 *          output state (`mData`) to `0.0f`.
 * @see Lpf::Lpf(float), ElapsedTime::reset
 * @todo none
 * @bug none
 */
Lpf::Lpf(void)
{
	// Assign default unity convergence rate coefficient
	mRatio = 1;
	// Reset internal elapsed timer to current hardware timestamp
	mTime.reset();
	// Initialize internal filtered state to zero
	mData = 0;
}

/**
 * @details Sets the convergence ratio coefficient (`mRatio`) used in subsequent low-pass filter calculations.
 * @see Lpf::calculate
 * @todo none
 * @bug none
 */
void Lpf::setRatio(float value)
{
	// Update convergence speed coefficient
	mRatio = value;
}

/**
 * @details Calculates the next filtered output sample given the raw input value.
 *          The method obtains the microsecond elapsed time since the previous calculation via `mTime.getUsec()`,
 *          resets the timer for the next interval, and computes the 1st-order discrete approximation:
 *          \f$ mData = mData - (mData - value) \times mRatio \times \Delta t \f$.
 * @see ElapsedTime::getUsec, ElapsedTime::reset, Lpf::getCurrentData
 * @todo none
 * @bug none
 */
float Lpf::calculate(float value)
{
	float buf, elapsedTime;

	// Compute difference (error) between previous filtered state and new raw input sample
	buf = mData - value;
	// Measure elapsed time in microseconds since the previous update
	elapsedTime = mTime.getUsec();
	// Restart elapsed timer for the next calculation cycle
	mTime.reset();
	// Apply time-weighted first-order low-pass filter update (converting microseconds to seconds)
	mData -= buf * mRatio * (float)elapsedTime / (float)1000000;

	return mData;
}

/**
 * @details Returns the current internal filtered state value (`mData`) without performing any update.
 * @see Lpf::setCurrentData, Lpf::calculate
 * @todo none
 * @bug none
 */
float Lpf::getCurrentData(void)
{
	// Return the latest filtered output state
	return mData;
}

/**
 * @details Overwrites the current internal filtered state (`mData`) with the specified value.
 *          This is commonly used to seed an initial baseline (e.g., first sensor or touch coordinate reading)
 *          to avoid startup transient lag.
 * @see Lpf::getCurrentData, Lpf::calculate
 * @todo none
 * @bug none
 */
void Lpf::setCurrentData(float data)
{
	// Force internal filtered state to the specified baseline value
	mData = data;
}

