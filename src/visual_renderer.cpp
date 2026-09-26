#include "visual_renderer.h"

namespace metronome {

void VisualRenderer::startPulse(uint8_t led, Accent accent, uint32_t nowUs,
                                uint32_t beatIntervalUs) {
  led_ = led < kLedCapacity ? led : 0;
  durationUs_ = effectiveDurationUs(accent, beatIntervalUs);
  offDeadlineUs_ = nowUs + durationUs_;
  active_ = true;
}

bool VisualRenderer::update(uint32_t nowUs) {
  if (!active_ || !reached(nowUs, offDeadlineUs_)) {
    return false;
  }
  active_ = false;
  return true;
}

void VisualRenderer::cancel() {
  active_ = false;
}

bool VisualRenderer::active() const {
  return active_;
}

uint8_t VisualRenderer::led() const {
  return led_;
}

uint32_t VisualRenderer::durationUs() const {
  return durationUs_;
}

uint32_t VisualRenderer::configuredDurationUs(Accent accent) {
  switch (accent) {
    case Accent::PRIMARY:
      return kPrimaryDurationUs;
    case Accent::SECONDARY:
      return kSecondaryDurationUs;
    case Accent::NONE:
      return kNoneDurationUs;
  }
  return kNoneDurationUs;
}

uint32_t VisualRenderer::effectiveDurationUs(Accent accent,
                                             uint32_t beatIntervalUs) {
  const uint32_t safeDuration =
      (beatIntervalUs * kSafeIntervalNumerator) / kSafeIntervalDenominator;
  const uint32_t configuredDuration = configuredDurationUs(accent);
  return configuredDuration < safeDuration ? configuredDuration : safeDuration;
}

bool VisualRenderer::reached(uint32_t nowUs, uint32_t deadlineUs) {
  return static_cast<int32_t>(nowUs - deadlineUs) >= 0;
}

}  // namespace metronome
