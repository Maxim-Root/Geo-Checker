#pragma once

/**
 * Settings for the "Theme" and "Language" row in settings.
 * Edit this file — changes apply after rebuild. cmake --build cpp/build
 */
namespace lang_ui {

// ---- Language button ----
// Button width (pixels)
constexpr int BTN_WIDTH_RU = 105;
constexpr int BTN_WIDTH_EN = 105;
// Button height
constexpr int BTN_HEIGHT = 36;
// Horizontal offset: positive = right, negative = left
constexpr int BTN_MOVE_X_RU = 15;
constexpr int BTN_MOVE_X_EN = 15;

// ---- "Language" label ----
// Label width (0 = fit to text)
constexpr int LABEL_WIDTH_RU = 56;
constexpr int LABEL_WIDTH_EN = 78;
// Free label X offset (any value, e.g. for overlapping the button)
constexpr int LABEL_MOVE_X_RU = 18;
constexpr int LABEL_MOVE_X_EN = 16;

// ---- Row spacing (left to right) ----
// Space between theme combobox (System) and "Language" label
constexpr int SPACER_WIDTH_RU = 120;
constexpr int SPACER_WIDTH_EN = 160;

// Gap between "Language" label and language button
constexpr int SPACE_LABEL_TO_BTN_GAP_RU = 0;
constexpr int SPACE_LABEL_TO_BTN_GAP_EN = 0;

// Extra padding inside button widget (usually 0)
constexpr int SPACE_TO_BTN_RU = 0;
constexpr int SPACE_TO_BTN_EN = 0;
constexpr int LABEL_TO_BTN_OFFSET_RU = 0;
constexpr int LABEL_TO_BTN_OFFSET_EN = 0;

// Space to the right of the button (align with Data Files block)
constexpr int SPACE_BTN_RIGHT = 12;

// Min-width for QComboBox in styles (should be <= button width)
constexpr int COMBO_MIN_WIDTH = 70;

} // namespace lang_ui
