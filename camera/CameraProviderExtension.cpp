/*
 * SPDX-FileCopyrightText: (C) 2025 Paranoid Android
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "CameraProviderExtension"

#include "CameraProviderExtension.h"
#include <android-base/logging.h>
#include <fstream>
#include <string>

#define TOGGLE_SWITCH "/sys/class/leds/led:switch_2/brightness"

static const std::string kTorchBrightnessNodes[] = {
        "/sys/class/leds/led:torch_0/brightness",
        "/sys/class/leds/led:torch_1/brightness",
        "/sys/class/leds/led:torch_2/brightness",
        "/sys/class/leds/led:torch_3/brightness",
};

/**
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    if (file.is_open()) {
        file << value;
    } else {
        LOG(ERROR) << "Failed to open node for writing: " << path;
    }
}

/**
 * Read value from the path and close file.
 */
template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;
    if (!file.is_open()) {
        LOG(ERROR) << "Failed to open node for reading: " << path;
        return def;
    }
    file >> result;
    return file.fail() ? def : result;
}

bool supportsTorchStrengthControlExt() {
    return true;
}

bool supportsSetTorchModeExt() {
    return false;
}

int32_t getTorchDefaultStrengthLevelExt() {
    return 65;
}

int32_t getTorchMaxStrengthLevelExt() {
    // Hardware limit is 500, however we limit to 300 for safety reasons.
    return 300;
}

int32_t getTorchStrengthLevelExt() {
    // We write the same value in all the LEDs, so get from the first one.
    return get(kTorchBrightnessNodes[0], 0);
}

void setTorchStrengthLevelExt(int32_t torchStrength, bool enabled) {
    LOG(DEBUG) << "setTorchStrengthLevelExt(" << torchStrength << ", " << enabled << ")";
    set(TOGGLE_SWITCH, 0);
    for (const auto& node : kTorchBrightnessNodes) {
        set(node, torchStrength);
    }
    if (enabled) {
        set(TOGGLE_SWITCH, 255);
    }
}

void setTorchModeExt(bool enabled) {
    LOG(DEBUG) << "setTorchModeExt(" << enabled << ")";
    setTorchStrengthLevelExt(enabled ? getTorchDefaultStrengthLevelExt() : 0, enabled);
}