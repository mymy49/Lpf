/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file module_Lpf.cpp
 * @brief Implementation of the Lpf (Low Pass Filter) class methods.
 */

#include "Lpf.h"
#include <util/runtime.h>

/**
 * @details Initializes a new Lpf instance with a user-specified filter ratio.
 * It resets the internal elapsed timer and initializes the filtered output state to 0.
 */
Lpf::Lpf(float ratio)
{
	mRatio = ratio;
	mTime.reset();
	mData = 0;
}

/**
 * @details Initializes a new Lpf instance with a default filter ratio of 1.0.
 * It resets the internal elapsed timer and sets the filtered output state to 0.
 */
Lpf::Lpf(void)
{
	mRatio = 1;
	mTime.reset();
	mData = 0;
}

/**
 * @details Sets the convergence ratio coefficient used in subsequent low-pass filter calculations.
 */
void Lpf::setRatio(float value)
{
	mRatio = value;
}

/**
 * @details Calculates the next filtered output sample given the raw input value.
 * The method obtains the microsecond elapsed time since the previous calculation,
 * resets the timer, and computes the 1st-order discrete approximation:
 * \f$ mData = mData - (mData - value) \times mRatio \times \Delta t \f$.
 */
float Lpf::calculate(float value)
{
	float buf, elapsedTime;

	buf = mData - value;
	elapsedTime = mTime.getUsec();
	mTime.reset();
	mData -= buf * mRatio * (float)elapsedTime / (float)1000000;

	return mData;
}

/**
 * @details Returns the current internal filtered state value without performing any update.
 */
float Lpf::getCurrentData(void)
{
	return mData;
}

/**
 * @details Overwrites the current internal filtered state with the specified value.
 * This can be used to set an initial baseline (e.g., initial sensor reading)
 * to avoid startup transient lag.
 */
void Lpf::setCurrentData(float data)
{
	mData = data;
}

