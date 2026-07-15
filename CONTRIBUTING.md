# Contributing to PoseBust

Thank you for helping maintain a fast, clean-room, Apache-2.0 pose validator.

## 1. Licensing (required)

- PoseBust is licensed under the **Apache License, Version 2.0**
  ([LICENSE](LICENSE), [NOTICE](NOTICE)).
- **By opening a pull request, you agree** that your contribution is original
  (or you have rights to submit it) and may be redistributed under Apache-2.0
  as part of this project.
- Do **not** submit GPL/AGPL code, or code derived from PoseBusters, RDKit, or
  other restricted sources. See
  [docs/licensing/clean-room-policy.md](docs/licensing/clean-room-policy.md).
- New source files must include:

  ```text
  Copyright 2026 Le Bonhomme Pharma
  SPDX-License-Identifier: Apache-2.0
  ```

  (Use the current year for new files; keep historical years on existing files.)

- If you add a dependency, update `THIRD_PARTY_LICENSES.md` and `NOTICE` in the
  same PR. Allowed: Apache-2.0, BSD, MIT, MPL-2.0. Forbidden: GPL/AGPL.

## 2. How to contribute

1. Fork and branch from `main`.
2. Prefer an issue for behavior changes, new checks, or API surface changes.
3. Implement with tests (GoogleTest under `tests/`).
4. Build and verify:

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release -DPOSEBUST_BUILD_TESTS=ON
   cmake --build build -j
   ctest --test-dir build --output-on-failure
   ./build/posebust --help
   ```

5. Open a PR that states:
   - what changed and why;
   - whether NativePoseQC keys or the bust bridge changed;
   - any license / dependency impact.

## 3. Coding guidelines

- **C++26**, no exceptions required in the public API (bool + optional `err`).
- Namespace: `posebust` (library) and `posebust::shell_exec` (exec helpers).
- Prefer argv-based process execution; never interpolate untrusted paths into
  a shell string without `shell_quote` / path validation.
- Keep **claim language precise**:
  - NativePoseQC → diagnostic only;
  - upstream `bust` → claim-ready `pb_pass`.
- Public headers live under `include/posebust/`; installable as
  `#include <posebust/Engine.h>`.

## 4. Scientific honesty

- Do not claim “PoseBusters pass” for NativePoseQC-only results.
- RMSD is out of scope for this library’s success gate; consumers combine
  RMSD ≤ 2 Å with `pb_pass` themselves.

## 5. Code of conduct

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## 6. Maintainers

- **Le Bonhomme Pharma** — https://github.com/LeBonhommePharma  
- **Louis-Philippe Morency** — project lead  

Questions: GitHub Issues on this repository.
