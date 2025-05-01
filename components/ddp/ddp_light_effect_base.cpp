#include "ddp_light_effect_base.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ddp {

DDPLightEffectBase::DDPLightEffectBase() {}

void DDPLightEffectBase::start() {
  if (this->ddp_) {
    this->ddp_->add_effect(this);
  }
}

void DDPLightEffectBase::stop() {
  if (this->ddp_) {
    this->ddp_->remove_effect(this);
  }
}

bool DDPLightEffectBase::timeout_check() {
  if (this->timeout_ == 0) {
    return false;
  }

  if (this->next_packet_will_be_first_) {
    return false;
  }

  if ((millis() - this->last_ddp_time_ms_) <= this->timeout_) {
    return false;
  }

  return true;
}

}  // namespace ddp
}  // namespace esphome
