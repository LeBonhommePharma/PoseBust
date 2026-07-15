# Third-Party Licenses

**PoseBust** is licensed under the **Apache License, Version 2.0**.  
See [LICENSE](LICENSE) and [NOTICE](NOTICE).

This document lists **optional** or **test-only** third-party components that
may appear in a developer or deployment environment. **None of the following
are vendored into the PoseBust source tree** unless explicitly noted.

---

## First-party Work

| Component | Copyright | License |
|-----------|-----------|---------|
| PoseBust library, CLI, tests, docs | 2026 Le Bonhomme Pharma; 2026 Louis-Philippe Morency | Apache-2.0 |

SPDX: `Apache-2.0`

---

## Optional runtime tools (subprocess only — not linked)

These binaries are **never** compiled into `libposebust`. They are executed via
argv-based process helpers only when the user opts in (CLI flags / env vars).

### PoseBusters (`bust`) — BSD License

| | |
|--|--|
| **Project** | PoseBusters — automated quality checks for docking/generative poses |
| **Upstream** | https://github.com/maabuu/posebusters |
| **License** | BSD (see upstream `LICENSE`) |
| **Use in PoseBust** | Optional **authoritative** `pb_pass` claim gate via `--bust` / `Backend::BustCli` |
| **Resolution** | `POSEBUSTERS_BIN`, `POSEBUST_BUST_BIN`, `FLEXAIDDS_POSEBUSTERS_BIN`, or `PATH` |

PoseBust does **not** redistribute the PoseBusters Python package. NativePoseQC
(`--native`) is an independent clean-room diagnostic suite that reuses only
public check **key names** for report parity.

### InChI (`inchi-1`) — IUPAC InChI License

| | |
|--|--|
| **Project** | IUPAC International Chemical Identifier software |
| **Upstream** | https://www.inchi-trust.org/ |
| **Use in PoseBust** | Optional soft check `inchi_convertible` in NativePoseQC |
| **Resolution** | `POSEBUST_INCHI_BIN`, `FLEXAIDDS_INCHI_BIN`, Homebrew paths, or `PATH` |

When `inchi-1` is missing, the native suite soft-passes that check with an
explicit `inchi-1_missing` detail (documented in check output).

### OpenSSL (`openssl dgst`) — Apache-2.0

| | |
|--|--|
| **Project** | OpenSSL |
| **Upstream** | https://www.openssl.org/ |
| **Use in PoseBust** | Optional file SHA-256 via `openssl dgst -sha256` for provenance helpers |
| **License** | Apache License 2.0 (OpenSSL 3.x) |

---

## Test / CI only (FetchContent)

### GoogleTest — BSD-3-Clause

| | |
|--|--|
| **Project** | GoogleTest / GoogleMock |
| **Upstream** | https://github.com/google/googletest |
| **Tag** | `v1.15.2` (see `CMakeLists.txt`) |
| **Use** | Unit tests when `POSEBUST_BUILD_TESTS=ON` |
| **Linked into** | `posebust_tests` only — **not** `libposebust` or the `posebust` CLI |

```
Copyright 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

---

## Build system

| Tool | Typical license | Role |
|------|-----------------|------|
| CMake ≥ 3.28 | BSD-3-Clause | Configure / build |
| C++26 compiler (Clang / GCC / MSVC / Apple Clang) | Vendor | Compile |

---

## Forbidden dependencies

To preserve Apache-2.0 purity of the **Work**:

- **Do not** introduce GPL, AGPL, or other strong-copyleft libraries into the
  link graph of `libposebust` or the CLI.
- **Do not** vendor RDKit, PoseBusters Python sources, or any GPL cheminformatics
  toolkit into this repository.
- **Do not** copy algorithm implementations from GPL-licensed docking or
  validation codebases. See [docs/licensing/clean-room-policy.md](docs/licensing/clean-room-policy.md).

---

## Compatibility summary

| Consumer | Can use PoseBust (Apache-2.0)? |
|----------|--------------------------------|
| FlexAIDdS (Apache-2.0) | Yes |
| Proprietary / closed docking stacks | Yes (subject to Apache-2.0 terms) |
| Academic redistribution | Yes |
| Combined with optional BSD `bust` subprocess | Yes (separate process; separate license) |

For questions: open a GitHub issue on
[LeBonhommePharma/PoseBust](https://github.com/LeBonhommePharma/PoseBust).
