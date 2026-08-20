# `mv::math` v2 cutover performance record

This is the terminal record for the temporary `mv::math` versus `move::math`
migration executables. The live benchmark suite no longer needs the legacy
headers after this evidence was accepted.

## Basis

- Move revision: `fe3b6f736a99000f6dcb344e8a7b81f52265af6f`
- Benchmark revision before removal: `e1be1f9be1a5b1252aac195873a46c15923ad723`
- Compiler/ISA: GCC 16.1, SSE4.2 with AVX disabled, `-O3`
- Working set shown below: 4,096 values
- Correctness: all 12 SSE4.2/AVX/AVX2 parity tests passed before measurement

The host used the powersave governor and frequency scaling. Nanosecond timings
were therefore diagnostic only. Retired-generic-instruction counts and isolated
generated-code inspection are the durable evidence.

## Core operations

| Operation | `mv::math` ins/op | Legacy ins/op | Conclusion |
| --- | ---: | ---: | --- |
| Vec3 arithmetic chain | 7 | 7 | Parity |
| Vec2 component chain | 5.51 | 5.51 | Parity |
| Vec3 component chain | 12 | 11 | One extra instruction maintains the private fourth-lane invariant; measured runtime was effectively parity. |
| Vec4 component chain | 11 | 11 | Parity |
| Dot | 13 | 13 | Parity |
| Cross | 13 | 13 | Parity |
| Checked direction construction / legacy unchecked normalize | 31 | 23 | Intentional boundary-contract cost: finite input validation, status, and scaling-safe fallback. Prevalidated `Direction3` hot paths do not repeat it. |
| Rotate vector | 23 | 30 | Current API is leaner. |

## Matrix operations

| Operation | `mv::math` ins/op | Legacy ins/op | Conclusion |
| --- | ---: | ---: | --- |
| Mat3 transform | 16 | 18 | Current API is leaner. |
| Mat3 multiply | 42 | 42 | Parity |
| Mat3 determinant | 27 | 59 | Current API is substantially leaner. |
| Mat3 checked inverse / legacy unchecked inverse | 103 | 68 | Intentional finite/singular-result validation cost. |
| Mat3 from rotation | 58 | 58 | Parity |
| Mat4 transform | 21 | 19 | The working-set loop needs two extra pointer-induction instructions because public `Vec4` rows may alias outputs. The isolated operation is code-generation-identical at 19 instructions. |
| Mat4 multiply | 73 | 73 | Parity |
| Mat4 determinant | 75 | 75 | Parity |
| Mat4 checked inverse / legacy unchecked inverse | 226 | 187 | Intentional finite/singular-result validation cost. |
| Checked perspective / legacy unchecked perspective | 174.3 | 72 | Explicit parameter, handedness, clip-depth, and depth-direction validation. This is a camera-configuration boundary, not a per-object hot loop. |
| Checked orthographic / legacy unchecked orthographic | 85 | 32 | Same stronger boundary contract. |
| Checked look-at / legacy unchecked look-at | 196 | 101 | Direction normalization and coincident/parallel input failure are explicit. |

## Decision

No unexplained performance regression remains. Ordinary hot arithmetic is at
parity or faster. The two small representation differences are understood, and
the larger differences compare checked construction boundaries against legacy
unchecked operations. The cross-library capability suite remains the ongoing
performance tool; correctness-first ISA parity tests remain its CI gate.
