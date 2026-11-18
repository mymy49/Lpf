# Lpf

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Doxygen Build](https://github.com/mymy49/Lpf/actions/workflows/doxygen.yml/badge.svg)](https://github.com/mymy49/Lpf/actions)
[![Doxygen API Docs](https://img.shields.io/badge/docs-API%20Reference-brightgreen.svg)](https://mymy49.github.io/Lpf/)

Time-aware, continuous-time first-order Low Pass Filter (LPF) class with dynamic sampling interval compensation, designed for the **yss OS** embedded framework.

---

## 1. Overview & Key Features

The **Lpf** module provides an efficient first-order low-pass filter tailored for embedded sensor processing, analog signal conditioning, and touch coordinate debouncing/smoothing. Unlike standard discrete digital filters that assume an idealized, fixed sampling period ($T_s$), `Lpf` dynamically measures the microsecond elapsed time between successive calculation invocations.

### Key Highlights
- **Dynamic Elapsed-Time Compensation**:
  - Incorporates `ElapsedTime` from yss OS to calculate the exact \f$ \Delta t \f$ between consecutive calls.
  - Maintains consistent filter dynamics and cutoff characteristics even in the presence of task scheduling jitter, variable CPU load, or non-uniform interrupt arrival rates.
- **Mathematical Formulation**:
  - Discretized continuous-time first-order low-pass filter:
    \f[
    y(t) = y(t - \Delta t) - (y(t - \Delta t) - x(t)) \cdot \text{ratio} \cdot \Delta t
    \f]
    where:
    - \f$ x(t) \f$: Raw input sample (ADC voltage, sensor reading, raw coordinate).
    - \f$ y(t) \f$: Filtered output sample stored in `mData`.
    - \f$ \Delta t \f$: Measured microsecond elapsed time converted to seconds \f$ (\frac{\Delta t_{\mu s}}{1,000,000}) \f$.
    - \f$ \text{ratio} \f$: Convergence rate coefficient determining filter response speed.
- **Transient Jump Suppression**:
  - `setCurrentData(initialValue)` allows immediate pre-loading of the filter's internal state with the initial sample, preventing initial ramp-up delay or false startup transients.
- **Lightweight Embedded Design**:
  - Minimal footprint: only two 32-bit floats and a hardware elapsed timer.
  - Zero dynamic heap allocation during execution, making it suitable for high-rate interrupt handlers and tight real-time control loops.

---

## 2. System Specifications & Hardware Environment

### Supported Targets & Tools
- **Target Microcontrollers & Development Boards**:
  - Nuvoton NuMicro® M251 series (e.g., **NuMaker-M251KG** / Cortex®-M23).
  - Any microcontroller platform supported by the **yss OS** framework.
- **Typical Use Cases**:
  - Resistive touch screen coordinate jitter elimination (e.g. smoothing XPT2046 touch input).
  - Smoothing noisy Analog-to-Digital Converter (ADC) sensor readings (temperature, battery voltage, current sense).
  - IMU sensor data smoothing (accelerometers, gyroscopes).
- **Required Software & Toolchains**:
  - Toolchain: ARM GNU Toolchain or CrossWorks for ARM (v5.x or higher).
    - **Note on CrossWorks for ARM**: CrossWorks for ARM does not natively support the Nuvoton M251 series. You must install the custom [CrossworksForARM_Nuvoton_M251 Support Package](https://github.com/mymy49/CrossworksForARM_Nuvoton_M251) before building M251 projects.
  - Operating System: [yss OS](https://github.com/mymy49/yss) (embedded RTOS & HAL, providing `util/ElapsedTime.h`).
  - Language: C++11 or higher.

---

## 3. Installation & Project Setup

### Cloning with Submodules
To clone a project that includes this repository as a submodule:
```bash
git clone --recursive https://github.com/mymy49/NuMaker-M251KG_example.git
```

To add this repository as a submodule into your existing yss OS application:
```bash
git submodule add https://github.com/mymy49/Lpf.git module/Lpf
```

### Prerequisites: Nuvoton M251 Support Package for CrossWorks for ARM
CrossWorks for ARM does not natively provide device support for Nuvoton NuMicro® M251 microcontrollers in its default package manager. A custom device support package must be installed beforehand:

1. Clone or download the [CrossworksForARM_Nuvoton_M251](https://github.com/mymy49/CrossworksForARM_Nuvoton_M251) repository.
2. Open the package project file (`.hzp`) in CrossWorks for ARM and build it (`F7` or `Build -> Build Project`) to generate the package file (`.hzq`).
3. In CrossWorks for ARM, navigate to **Tools -> Packages -> Manually Install Packages...** and select the generated `.hzq` package file.
4. Once installed, M251 target device definitions, CMSIS headers, and project templates will be fully available in CrossWorks for ARM.

### Build and Firmware Flashing
- **CrossWorks for ARM**:
  1. Ensure the [CrossworksForARM_Nuvoton_M251](https://github.com/mymy49/CrossworksForARM_Nuvoton_M251) package has been installed.
  2. Open the project solution file (e.g., `Example.hzp`).
  3. Select target configuration (`Example THUMB Release` or `Example THUMB Debug`).
  4. Build solution via `F7` or `Build -> Build Solution`.
  5. Connect the board via Nu-Link ICE and flash with `Ctrl+D` (`Target -> Download`).
- **Command Line (pyOCD / OpenOCD)**:
  ```bash
  # Flash prebuilt binary using pyocd
  pyocd flash -t m251kg firmware.bin
  ```

---

## 4. Quick Start & Example Code

Below are real-world usage patterns demonstrating how to apply `Lpf` for touch coordinate smoothing and analog sensor filtering under yss OS:

### 1. Touch Coordinate Smoothing (from `page_drawing.cpp` & `page_touchCalibration.cpp`)
```cpp
#include <bsp.h>
#include <Lpf/Lpf.h>

// Filters for X and Y touch axes with convergence ratio of 20.0
static Lpf gLpfX(20.0f);
static Lpf gLpfY(20.0f);

void touchEventCallback(PointerDevice::event_t event)
{
    switch (event.event)
    {
    case PointerDevice::EVENT_TOUCH_DOWN:
        // Pre-seed filter state to initial contact point to eliminate ramp-up lag
        gLpfX.setCurrentData(event.x);
        gLpfY.setCurrentData(event.y);
        break;

    case PointerDevice::EVENT_TOUCH_DRAG:
        // Update filter with new touch coordinates and retrieve smoothed output
        uint16_t smoothX = (uint16_t)gLpfX.calculate(event.x);
        uint16_t smoothY = (uint16_t)gLpfY.calculate(event.y);

        // Perform drawing with smoothed coordinates
        lcd.lock();
        lcd.drawLine({lastX, lastY}, {smoothX, smoothY});
        lcd.unlock();

        lastX = smoothX;
        lastY = smoothY;
        break;

    case PointerDevice::EVENT_TOUCH_UP:
        // Touch released
        break;
    }
}
```

### 2. General Periodic ADC Sensor Filtering
```cpp
#include <Lpf/Lpf.h>
#include <yss/instance.h>

// Initialize filter with convergence speed 5.0
Lpf batteryFilter(5.0f);

void batteryMonitorThread(void)
{
    // Initialize filter state with the first ADC sample
    batteryFilter.setCurrentData(readBatteryAdc());

    while (1)
    {
        float rawAdc = readBatteryAdc();
        float filteredVoltage = batteryFilter.calculate(rawAdc);

        // Process smoothed value...
        thread::delay(20); // 50 Hz loop with dynamic jitter compensation
    }
}
```

---

## 5. Documentation & References

- **Doxygen API Reference**: [https://mymy49.github.io/Lpf/](https://mymy49.github.io/Lpf/)
- **Doxygen Main Page**: This `README.md` file serves as the main page of the generated Doxygen documentation (`USE_MDFILE_AS_MAINPAGE = README.md`).
- **Underlying Framework**: [yss OS](https://github.com/mymy49/yss)

---

## 6. License & Author

- **Author**: Yoon-Ki Hong
- **License**: MIT License. See [LICENSE](LICENSE) for full terms and conditions.
