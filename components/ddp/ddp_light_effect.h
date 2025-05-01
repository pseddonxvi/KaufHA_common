
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ddp {

class DDPLightEffectBase {
 public:
  virtual ~DDPLightEffectBase() = default;
  virtual void on_ddp_data(const std::vector<uint8_t> &data) = 0;
};

}  // namespace ddp
}  // namespace esphome
