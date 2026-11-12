/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file Lpf.h
 * @brief First-order Low Pass Filter (LPF) class with elapsed time-based dynamic weighting.
 */

#ifndef YSS_MODULE_LPF__H_
#define YSS_MODULE_LPF__H_

#include <util/ElapsedTime.h>
#include <yss/Mutex.h>

/**
 * @class Lpf
 * @brief A continuous-time first-order Low Pass Filter (LPF) utilizing microsecond elapsed time.
 *
 * @details
 * `Lpf` provides first-order low-pass filtering suitable for smoothing noisy sensor data
 * (e.g. ADC readings, temperature, gyro/accelerometer, or speed).
 * Unlike simple discrete filters with a fixed sampling period, `Lpf` dynamically measures
 * the actual elapsed time between consecutive calls via `ElapsedTime`. This allows accurate
 * and stable filtering even when the sampling interval is non-uniform or jittered.
 *
 * ### Filter Equation
 * \f[
 * y(t) = y(t - \Delta t) - (y(t - \Delta t) - x(t)) \cdot \text{ratio} \cdot \Delta t
 * \f]
 * where:
 * - \f$ x(t) \f$ is the new raw input value.
 * - \f$ y(t) \f$ is the filtered output.
 * - \f$ \Delta t \f$ is the elapsed time in seconds since the previous update.
 * - \f$ \text{ratio} \f$ is the filter convergence rate coefficient.
 *
 * ### Example Usage
 * @code
 * #include <Lpf.h>
 *
 * Lpf filter(5.0f); // Ratio = 5.0
 *
 * void loop(void)
 * {
 *     float rawAdc = readAdc();
 *     float filteredValue = filter.calculate(rawAdc);
 *     // Use filteredValue...
 * }
 * @endcode
 */
class Lpf
{
public :
	/**
	 * @brief Constructs a new Lpf object with a specified convergence ratio.
	 * @param[in] ratio Filter coefficient representing the convergence speed (higher value converges faster).
	 */
	Lpf(float ratio);

	/**
	 * @brief Constructs a new Lpf object with default ratio (1.0) and zero initial state.
	 */
	Lpf(void);

	/**
	 * @brief Sets the filter convergence ratio coefficient.
	 * @param[in] value Filter coefficient (convergence speed).
	 */
	void setRatio(float value);

	/**
	 * @brief Updates the filter state with a new raw input value and returns the filtered output.
	 * @details Measures the elapsed time since the previous call, calculates the new filtered
	 *          value using first-order low-pass filter equation, and resets the internal timer.
	 * @param[in] value Raw input sample to be filtered.
	 * @return The updated filtered value.
	 */
	float calculate(float value);

	/**
	 * @brief Retrieves the most recently calculated filtered value without updating filter state.
	 * @return The current filtered value stored in the filter.
	 */
	float getCurrentData(void);

	/**
	 * @brief Manually sets the current filtered value.
	 * @details Useful for initializing the filter state to the first measured sample
	 *          to prevent an initial ramp-up delay or transient offset.
	 * @param[in] data The initial or forced value to set.
	 */
	void setCurrentData(float data);

private :
	float mData;          ///< Internal filtered output value (current state)
	float mRatio;         ///< Filter convergence rate coefficient
	ElapsedTime mTime;    ///< Timer measuring elapsed time between successive calculations
};

#endif



