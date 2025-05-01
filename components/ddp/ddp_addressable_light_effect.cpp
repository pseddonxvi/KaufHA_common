#include "esphome/core/log.h"
#include "ddp_addressable_light_effect.h"

namespace esphome {
namespace ddp {

static const char *const TAG = "ddp.addressable_effect";

void DDPAddressableLightEffect::start() {
  ESP_LOGD(TAG, "Starting DDP Effect");
  this->data_received_ = false;
  this->colors_.clear();
  
  // Add this effect to the global component
  if (global_ddp_component != nullptr) {
    ESP_LOGD(TAG, "Adding effect to global DDP component");
    global_ddp_component->add_effect(this);
  } else {
    ESP_LOGW(TAG, "Global DDP component not initialized!");
  }
}

void DDPAddressableLightEffect::stop() {
  ESP_LOGD(TAG, "Stopping DDP Effect");
  
  // Remove this effect from the global component
  if (global_ddp_component != nullptr) {
    global_ddp_component->remove_effect(this);
  }
}

void DDPAddressableLightEffect::apply(light::AddressableLight &it, const Color &current_color) {
  // Check if we've received DDP data
  if (!this->data_received_) {
    ESP_LOGW(TAG, "No DDP data received yet");
    return;
  }

  // Check if we've timed out
  if (millis() - this->last_data_ > this->timeout_) {
    ESP_LOGW(TAG, "DDP data timed out after %u ms", this->timeout_);
    this->data_received_ = false;
    return;
  }

  // Apply colors from DDP data
  uint32_t max_leds = std::min(it.size(), this->colors_.size());
  for (uint32_t i = 0; i < max_leds; i++) {
    it[i] = this->colors_[i];
  }

  // Show that we did something
  it.schedule_show();
}

void DDPAddressableLightEffect::on_ddp_data(const std::vector<uint8_t> &data) {
  // Check if we have enough data for DDP header (10 bytes)
  if (data.size() < 10) {
    ESP_LOGW(TAG, "DDP packet too small: %u bytes", data.size());
    return;
  }

  // Parse DDP header
  uint8_t flags = data[0];
  uint8_t type = data[1];
  uint8_t id_lo = data[2];
  uint8_t id_hi = data[3];
  uint32_t offset = (data[4] << 16) | (data[5] << 8) | data[6];
  uint16_t length = (data[7] << 8) | data[8];
  uint8_t data_type = data[9];

  // Check if this is a Push or Query data packet (type 1 or 3)
  if (type != 1 && type != 3) {
    ESP_LOGW(TAG, "Unsupported DDP packet type: %u", type);
    return;
  }

  // Check if this is RGB or RGBW data
  uint8_t channels_per_pixel = 3;  // Default to RGB
  if (data_type == 1) {
    channels_per_pixel = 4;  // RGBW
  } else if (data_type != 0) {
    ESP_LOGW(TAG, "Unsupported DDP data type: %u", data_type);
    return;
  }

  // Check if we have enough data for the stated length
  if (data.size() < 10 + length) {
    ESP_LOGW(TAG, "DDP packet too small for stated length: %u < %u", data.size(), 10 + length);
    return;
  }

  // Resize colors vector if needed
  uint32_t pixel_count = length / channels_per_pixel;
  if (this->colors_.size() < offset + pixel_count) {
    this->colors_.resize(offset + pixel_count);
  }

  // Copy color data
  for (uint32_t i = 0; i < pixel_count; i++) {
    uint32_t src_offset = 10 + i * channels_per_pixel;
    uint32_t dest_offset = offset + i;

    if (dest_offset < this->offset_) {
      // Skip pixels before the offset
      continue;
    }

    uint32_t actual_offset = dest_offset - this->offset_;
    if (actual_offset >= this->colors_.size()) {
      // Resize if needed
      this->colors_.resize(actual_offset + 1);
    }

    uint8_t r = data[src_offset];
    uint8_t g = data[src_offset + 1];
    uint8_t b = data[src_offset + 2];
    uint8_t w = (channels_per_pixel == 4) ? data[src_offset + 3] : 0;

    if (!this->disable_gamma_) {
      // Apply gamma correction
      r = light::gamma_correct(r, 2.8f);
      g = light::gamma_correct(g, 2.8f);
      b = light::gamma_correct(b, 2.8f);
      if (channels_per_pixel == 4) {
        w = light::gamma_correct(w, 2.8f);
      }
    }

    // Apply brightness scaling based on the configured mode
    float brightness = 1.0f;
    switch (this->brightness_scaling_) {
      case BRIGHTNESS_SCALING_ALPHA:
        // Don't scale - DDP doesn't have an alpha channel
        break;
      case BRIGHTNESS_SCALING_NONE:
        // No scaling
        break;
      case BRIGHTNESS_SCALING_LUMINANCE:
        // Scale by luminance
        brightness = 0.299f * r + 0.587f * g + 0.114f * b;
        brightness = brightness / 255.0f;
        if (brightness > 0.0f) {
          r = static_cast<uint8_t>(r / brightness);
          g = static_cast<uint8_t>(g / brightness);
          b = static_cast<uint8_t>(b / brightness);
        }
        break;
    }

    // Create the color
    if (channels_per_pixel == 4) {
      this->colors_[actual_offset] = Color(r, g, b, w);
    } else {
      this->colors_[actual_offset] = Color(r, g, b);
    }
  }

  // Update timestamp and mark as data received
  this->last_data_ = millis();
  this->data_received_ = true;
}

}  // namespace ddp
}  // namespace esphome
