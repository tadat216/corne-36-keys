# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Repo Is

ZMK firmware configuration for a 36-key Corne (CRKBD) split keyboard (3x5+3 layout) running on Nice!Nano v2 microcontrollers. The 5-column PCB uses the standard ZMK `corne` shield (42-key matrix) with outer pinky positions set to `&none`. Build is fully managed by ZMK's CI pipeline — no local build toolchain needed.

## Building Firmware

**Trigger a build:** push or open a PR — GitHub Actions runs automatically via `.github/workflows/build.yml`, which delegates to ZMK's official reusable workflow at `zmkfirmware/zmk/.github/workflows/build-user-config.yml@v0.3`.

**Build matrix** is defined in `build.yaml`. It builds two shields: `corne_left` and `corne_right`, both with ZMK Studio support.

**Download firmware:** go to the GitHub Actions run → download the artifact zip → flash each `.uf2` to the corresponding half by putting it in bootloader mode (double-tap reset).

## Repository Structure

All user-editable files live in `config/`:

| File | Purpose |
|------|---------|
| `config/corne.keymap` | All key bindings, layers, combos, and custom behaviors |
| `config/corne.conf` | Feature flags (BLE power, RGB, display, power management) |
| `config/corne.overlay` | Device tree: OLED (I2C), RGB (SPI3), external power |
| `config/corne.dtsi` | Hardware matrix transform and GPIO scan config |
| `config/west.yml` | ZMK version pin (`v0.3`) + urob module dependencies |

`zephyr/module.yml` tells west to treat this repo as a ZMK module. The `boards/` and `zephyr/` dirs are structural stubs required by the module system — not edited directly.

### ZMK Modules (via `west.yml`)

All from [urob](https://github.com/urob), pinned to `v0.3`:

| Module | Purpose |
|--------|---------|
| `zmk-helpers` | Macros (`ZMK_HOLD_TAP`, `ZMK_COMBO`, etc.) and key position labels |
| `zmk-adaptive-key` | Adaptive repeat behavior (reserved for future use) |
| `zmk-auto-layer` | Num-word auto-layer (reserved for future use) |
| `zmk-leader-key` | Leader key sequences (reserved for future use) |
| `zmk-tri-state` | Tri-state behaviors like smart-mouse (reserved for future use) |
| `zmk-unicode` | Unicode input (reserved for future use) |

## Keymap Architecture

### Physical Layout

36-key (3x5+3 per side) on 42-key matrix. Outer pinky positions (0, 11, 12, 23, 24, 35) are `&none`.

```
 _   1   2   3   4   5  |  6   7   8   9  10   _
 _  13  14  15  16  17  | 18  19  20  21  22   _
 _  25  26  27  28  29  | 30  31  32  33  34   _
               36  37  38  | 39  40  41
```

### Layer Index

| # | Name | Access |
|---|------|--------|
| 0 | Base (DEF) | Default QWERTY |
| 1 | Navigation (NAV) | `&lt NAV SPACE` (left thumb hold) |
| 2 | Functions (FN) | `&lt FN RET` (left thumb hold) |
| 3 | Numbers (NUM) | `&lt_bspc_del` (right thumb hold, tap=backspace, shift+tap=delete) |
| 4 | System (SYS) | Conditional: FN + NUM held simultaneously |

### Thumb Layout

```
Left:  GUI | Space/NAV | Enter/FN
Right: Bspc/NUM (shift=Del) | RShift | RAlt
```

### Home Row Mods (base layer, positional `hml`/`hmr`)

| Position | Key | Modifier | Behavior |
|----------|-----|----------|----------|
| 13 | A | Left Ctrl | `hml` (triggers on right-hand keys only) |
| 14 | S | Left Alt | `hml` |
| 15 | D | Left Cmd | `hml` |
| 16 | F | Left Shift | `hml` |
| 19 | J | Right Shift | `hmr` (triggers on left-hand keys only) |
| 20 | K | Right Cmd | `hmr` |
| 21 | L | Right Alt | `hmr` |
| 22 | ; | Right Ctrl | `hmr` |

HRM timing: 280ms tapping-term, 175ms quick-tap, 150ms require-prior-idle.

### Combos (27 total)

**Horizontal combos (18ms timeout, 150ms idle):**

| Keys | Result | Layers |
|------|--------|--------|
| W+E (2+3) | ESC | DEF NAV NUM |
| E+R (3+4) | Caps Word | DEF |
| S+D (14+15) | TAB / Hold: Shift+Alt | DEF NAV NUM |
| X+V (26+28) | Cut (Cmd+X) | DEF NAV NUM |
| X+C (26+27) | Copy (Cmd+C) | DEF NAV NUM |
| C+V (27+28) | Paste (Cmd+V) | DEF NAV NUM |
| U+I (7+8) | Backspace | DEF NAV NUM |
| I+O (8+9) | Delete | DEF NAV NUM |
| O+P (9+10) | Single Quote (') | DEF NAV NUM |
| J+K (19+20) | ( / Shift: < / Hold: Shift+Cmd | DEF NAV NUM |
| K+L (20+21) | ) / Shift: > / Hold: Cmd+Alt | DEF NAV NUM |
| M+, (31+32) | [ / Shift: { | DEF NAV NUM |
| ,+. (32+33) | ] / Shift: } | DEF NAV NUM |

**Vertical combos (30ms timeout, 50ms idle):**

| Keys | Result | Keys | Result |
|------|--------|------|--------|
| W+S (2+14) | @ | Y+H (6+18) | ^ |
| E+D (3+15) | # | U+J (7+19) | + |
| R+F (4+16) | $ | I+K (8+20) | * |
| T+G (5+17) | % | O+L (9+21) | & |
| S+X (14+26) | ` | H+N (18+30) | _ |
| D+C (15+27) | \ | J+M (19+31) | - |
| F+V (16+28) | = | K+, (20+32) | / |
| G+B (17+29) | ~ | L+. (21+33) | \| |

### Mod-Morph Behaviors

| Behavior | Tap | Shift+Tap |
|----------|-----|-----------|
| `comma_morph` | , | ; |
| `dot_morph` | . | : |
| `qmark_excl` | ? | ! |
| `lpar_lt` | ( | < |
| `rpar_gt` | ) | > |
| `lbkt_lbrc` | [ | { |
| `rbkt_rbrc` | ] | } |
| `lt_bspc_del` | Backspace (hold=NUM layer) | Delete |

## Hardware Notes

- **Layout:** 36-key (3x5+3) Corne on 42-key matrix shield
- **MCU:** Nice!Nano v2 (nRF52840)
- **RGB:** WS2812 underglow via SPI3, 27 LEDs per half (54 total), GRB color order, max brightness capped at 30 in `.conf`
- **Display:** SSD1306 OLED 128x32 via I2C at address `0x3c`
- **BLE TX power:** +8 dBm
- **Power timeouts:** 60s idle, 20min deep sleep

## ZMK-Specific Patterns

- The ZMK `corne` shield defines a 42-key matrix. Our 36-key PCB uses `&none` on the 6 outer pinky positions. All layers must have 42 bindings.
- Combos and HRM `hold-trigger-key-positions` use raw matrix numbers (not 36.h labels) because the 36.h positions don't match our 42-key matrix offsets.
- Combos go in a `combos { }` node inside the root `/ { }` block, **before** the `keymap { }` node.
- Custom behaviors (mod-morph, hold-tap, etc.) go in a `behaviors { }` node at the root level.
- Multiple `/ { }` blocks are valid — they get merged by the DTS compiler.
- The `ZMK_HOLD_TAP` helper macro from zmk-helpers has expansion issues with position label macros. Write hold-tap behaviors as explicit DTS nodes instead.
- **HRM combo hack:** Combos where both keys are home row mods (S+D, J+K, K+L) must use hold-tap wrapped bindings. Otherwise the combo fires instantly and you lose access to the combined modifiers. Tap = combo action, Hold = combined modifier of both HRM keys. See `hmr_lpar`, `hmr_rpar`, and the `&hml` on the TAB combo.
- ZMK Studio is enabled (`CONFIG_ZMK_STUDIO=y`) with locking disabled — live keymap editing works over USB without reflashing.
- The online Keymap Editor does not support preprocessor macros (`#define`, `___`, etc.). Use ZMK Studio for visual editing.
