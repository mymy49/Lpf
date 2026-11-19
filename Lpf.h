/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file Lpf.h
 * @brief First-order Low Pass Filter (LPF) class with elapsed time-based dynamic weighting.
 * @author Yoon-Ki Hong
 * @date 2026-11-18
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
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning Ensure the yss OS runtime timer is initialized before constructing or updating this filter.
	 * @note Initializes the internal filtered state `mData` to `0.0f` and resets the elapsed timer.
	 * @pre yss OS runtime timer (`YSS_RUNTIME_TIMER`) must be active.
	 * @post Filter instance is initialized with `mRatio = ratio` and `mData = 0.0f`.
	 * @deprecated none
	 */
	Lpf(float ratio);

	/**
	 * @brief Constructs a new Lpf object with default ratio (1.0) and zero initial state.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning Ensure the yss OS runtime timer is initialized before constructing or updating this filter.
	 * @note Initializes `mRatio` to `1.0f`, `mData` to `0.0f`, and resets the internal elapsed timer.
	 * @pre yss OS runtime timer (`YSS_RUNTIME_TIMER`) must be active.
	 * @post Filter instance is initialized with default parameters.
	 * @deprecated none
	 */
	Lpf(void);

	/**
	 * @brief Sets the filter convergence ratio coefficient.
	 * @param[in] value Filter coefficient (convergence speed).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning Positive values should be used for stable low-pass convergence.
	 * @note Can be adjusted dynamically at runtime without resetting the current filter output state.
	 * @pre Filter instance must be constructed.
	 * @post `mRatio` is updated to `value`.
	 * @deprecated none
	 */
	void setRatio(float value);

	/**
	 * @brief Updates the filter state with a new raw input value and returns the filtered output.
	 * @param[in] value Raw input sample to be filtered.
	 * @return The updated filtered floating-point value.
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning Very long gaps between calls can cause `(mRatio * elapsedTime)` to exceed `1.0f` and overshoot.
	 * @note Measures the microsecond elapsed time since the previous call and resets the internal timer.
	 * @pre Filter instance must be initialized; optionally call `setCurrentData()` on the first sample.
	 * @post `mData` and `mTime` are updated for the current sampling instant.
	 * @deprecated none
	 */
	float calculate(float value);

	/**
	 * @brief Retrieves the most recently calculated filtered value without updating filter state.
	 * @param[in] none
	 * @return The current filtered value stored in the filter (`mData`).
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning none
	 * @note Does not reset or modify the internal elapsed time counter.
	 * @pre Filter instance must be constructed.
	 * @post Filter state remains unchanged.
	 * @deprecated none
	 */
	float getCurrentData(void);

	/**
	 * @brief Manually sets the current filtered value.
	 * @param[in] data The initial or forced value to set as the internal filtered output state.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2026-11-18
	 * @warning Overwrites the accumulated filter history immediately.
	 * @note Useful for seeding the filter with the first raw sensor/touch sample to avoid startup ramp lag.
	 * @pre Filter instance must be constructed.
	 * @post `mData` is set directly to `data`.
	 * @deprecated none
	 */
	void setCurrentData(float data);

private :
	float mData;          ///< Internal filtered output value (current state)
	float mRatio;         ///< Filter convergence rate coefficient
	ElapsedTime mTime;    ///< Timer measuring elapsed time between successive calculations
};

#endif



