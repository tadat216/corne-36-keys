# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Repo Is

ZMK firmware configuration for a Corne (CRKBD) split keyboard running on Nice!Nano v2 microcontrollers. The build is fully managed by ZMK's CI pipeline — there is no local build toolchain to install.

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
| `zmk-helpers` | Macros (`ZMK_HOLD_TAP`, `ZMK_COMBO`, etc.) and key position labels (`42.h`) |
| `zmk-adaptive-key` | Adaptive repeat behavior (reserved for future use) |
| `zmk-auto-layer` | Num-word auto-layer (reserved for future use) |
| `zmk-leader-key` | Leader key sequences (reserved for future use) |
| `zmk-tri-state` | Tri-state behaviors like smart-mouse (reserved for future use) |
| `zmk-unicode` | Unicode input (reserved for future use) |

## Keymap Architecture

### Layer Index

| # | Name | Access |
|---|------|--------|
| 0 | Base (DEF) | Default QWERTY |
| 1 | Navigation (NAV) | `&lt NAV SPACE` (left thumb hold) |
| 2 | Functions (FN) | `&lt FN RET` (left thumb hold) |
| 3 | Numbers (NUM) | `&lt_bspc_del` (right thumb hold, tap=backspace, shift+tap=delete) |
| 4 | System (SYS) | Conditional: FN + NUM held simultaneously |
| 5 | Mouse (MOUSE) | Combo toggle: E + R keys |

### Key Position Map (named labels from `zmk-helpers/key-labels/42.h`)

```
LT5 LT4 LT3 LT2 LT1 LT0 | RT0 RT1 RT2 RT3 RT4 RT5
LM5 LM4 LM3 LM2 LM1 LM0 | RM0 RM1 RM2 RM3 RM4 RM5
LB5 LB4 LB3 LB2 LB1 LB0 | RB0 RB1 RB2 RB3 RB4 RB5
            LH2 LH1 LH0  | RH0 RH1 RH2

Numeric mapping:
 0   1   2   3   4   5  |  6   7   8   9  10  11
12  13  14  15  16  17  | 18  19  20  21  22  23
24  25  26  27  28  29  | 30  31  32  33  34  35
               36  37  38  | 39  40  41
```

### Thumb Layout

```
Left:  LGUI | Space/NAV | Enter/FN
Right: Bspc/NUM (shift=Del) | RShift | RAlt
```

### Home Row Mods (base layer, positional `hml`/`hmr`)

| Position | Key | Modifier | Behavior |
|----------|-----|----------|----------|
| LM4 (13) | A | Left Ctrl | `hml` (triggers on right-hand keys only) |
| LM3 (14) | S | Left Alt | `hml` |
| LM2 (15) | D | Left Cmd | `hml` |
| LM1 (16) | F | Left Shift | `hml` |
| RM1 (19) | J | Right Shift | `hmr` (triggers on left-hand keys only) |
| RM2 (20) | K | Right Cmd | `hmr` |
| RM3 (21) | L | Right Alt | `hmr` |
| RM4 (22) | ; | Right Ctrl | `hmr` |

HRM timing: 280ms tapping-term, 175ms quick-tap, 150ms require-prior-idle.

### Combos (27 total)

**Horizontal combos (18ms timeout, 150ms idle):**

| Keys | Result | Layers |
|------|--------|--------|
| W+E (LT3+LT2) | ESC | DEF NAV NUM |
| E+R (LT2+LT1) | Mouse toggle | DEF NAV NUM |
| S+D (LM3+LM2) | TAB | DEF NAV NUM |
| X+V (LB3+LB1) | Cut (Cmd+X) | DEF NAV NUM |
| X+C (LB3+LB2) | Copy (Cmd+C) | DEF NAV NUM |
| C+V (LB2+LB1) | Paste (Cmd+V) | DEF NAV NUM |
| U+I (RT1+RT2) | Backspace | DEF NAV NUM |
| I+O (RT2+RT3) | Delete | DEF NAV NUM |
| J+K (RM1+RM2) | ( | DEF NAV NUM |
| K+L (RM2+RM3) | ) | DEF NAV NUM |
| M+, (RB1+RB2) | [ | DEF NAV NUM |
| ,+. (RB2+RB3) | ] | DEF NAV NUM |
| J+K (RM1+RM2) | < | NAV only |
| K+L (RM2+RM3) | > | NAV only |
| M+, (RB1+RB2) | { | NAV only |
| ,+. (RB2+RB3) | } | NAV only |
| G+H (LM0+RM0) | Caps Word | DEF only |

**Vertical combos (30ms timeout, 50ms idle):**

| Keys | Result | Keys | Result |
|------|--------|------|--------|
| W+S (LT3+LM3) | @ | Y+H (RT0+RM0) | ^ |
| E+D (LT2+LM2) | # | U+J (RT1+RM1) | + |
| R+F (LT1+LM1) | $ | I+K (RT2+RM2) | * |
| T+G (LT0+LM0) | % | O+L (RT3+RM3) | & |
| S+X (LM3+LB3) | ` | H+N (RM0+RB0) | _ |
| D+C (LM2+LB2) | \ | J+M (RM1+RB1) | - |
| F+V (LM1+LB1) | = | K+, (RM2+RB2) | / |
| G+B (LM0+LB0) | ~ | L+. (RM3+RB3) | \| |

### Mod-Morph Behaviors

| Behavior | Tap | Shift+Tap |
|----------|-----|-----------|
| `comma_morph` | , | ; |
| `dot_morph` | . | : |
| `qmark_excl` | ? | ! |
| `lt_bspc_del` | Backspace (hold=NUM layer) | Delete |
| `bspc_del` | Backspace | Delete |

### Outer Pinky Column Keys (extra keys from 42-key layout)

| Position | Key |
|----------|-----|
| LT5 (0) | TAB |
| LM5 (12) | GRAVE |
| LB5 (24) | GLOBE |
| RT5 (11) | MINUS |
| RM5 (23) | Single Quote |
| RB5 (35) | EQUAL |

## Hardware Notes

- **RGB:** WS2812 underglow via SPI3, 27 LEDs per half (54 total), GRB color order, max brightness capped at 30 in `.conf`
- **Display:** SSD1306 OLED 128×32 via I2C at address `0x3c`
- **BLE TX power:** +8 dBm
- **Power timeouts:** 60s → idle, 20min → deep sleep

## ZMK-Specific Patterns

- Combos go in a `combos { }` node inside the root `/ { }` block, **before** the `keymap { }` node.
- Custom behaviors (mod-morph, hold-tap, etc.) go in a `behaviors { }` node at the root level.
- Multiple `/ { }` blocks are valid — they get merged by the DTS compiler.
- When adding combos on HRM keys (mod-taps), prefer plain `&kp` keys for combo positions to avoid hold-tap timing conflicts.
- The `ZMK_HOLD_TAP` helper macro from zmk-helpers has expansion issues with position label macros (`KEYS_R`, `THUMBS`). Write hold-tap behaviors as explicit DTS nodes instead.
- Named key positions from `42.h` (e.g., `LT3`, `RM2`) can be used directly in DTS `key-positions` and `hold-trigger-key-positions` properties.
- ZMK Studio is enabled (`CONFIG_ZMK_STUDIO=y`) with locking disabled — live keymap editing works over USB without reflashing.
- The online Keymap Editor does not support preprocessor macros (`#define`, `___`, etc.). Use ZMK Studio for visual editing.
