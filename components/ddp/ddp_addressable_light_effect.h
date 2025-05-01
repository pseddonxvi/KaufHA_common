#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/light/addressable_light_effect.h"
#include "ddp_light_effect.h"
#include "ddp.h"

namespace esphome {
namespace ddp {

// Enumeration for brightness scaling mode
enum BrightnessScaling {
  // Scale both RGB and W values by alpha
  BRIGHTNESS_SCALING_ALPHA = 0,
  // Don't scale RGB or W values
  BRIGHTNESS_SCALING_NONE = 1,
  // Scale based on calculated luminance
  BRIGHTNESS_SCALING_LUMINANCE = 2,
};

class DDPAddressableLightEffect : public light::AddressableLightEffect, public DDPLightEffectBase {
 public:
  DDPAddressableLightEffect(const std::string &name) : light::AddressableLightEffect(name) {}
  void start() override;
  void stop() override;
  void apply(light::AddressableLight &it, const Color &current_color) override;

  // Handle receiving DDP data
  void on_ddp_data(const std::vector<uint8_t> &data) override;

  // Set timeout for DDP data
  void set_timeout(uint32_t timeout) { timeout_ = timeout; }
  
  // Set whether to disable gamma correction
  void set_disable_gamma(bool disable_gamma) { disable_gamma_ = disable_gamma; }
  
  // Set brightness scaling mode
  void set_brightness_scaling(BrightnessScaling brightness_scaling) { brightness_scaling_ = brightness_scaling; }
  
  // Set offset in LED strip
  void set_offset(uint16_t offset) { offset_ = offset; }

 protected:
  uint32_t timeout_{1000};
  bool disable_gamma_{false};
  BrightnessScaling brightness_scaling_{BRIGHTNESS_SCALING_ALPHA};
  uint16_t offset_{0};
  uint32_t last_data_{0};
  std::vector<Color> colors_{};
  bool data_received_{false};
};

}  // namespace ddp
}  // namespace esphome
