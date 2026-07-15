# PoseBust

**Standalone C++26 pose validation** — clean-room NativePoseQC plus an optional bridge to the official [PoseBusters](https://github.com/maabuu/posebusters) Python CLI (`bust`).

Apache-2.0. **No RDKit, no posebusters source, no FlexAIDdS dependency.**

Extracted from the FlexAIDdS `LIB/PoseBust` module so docking engines, benchmark harnesses, and packaging pipelines can validate poses without pulling in a full docking codebase.

## What it is (and is not)

| Mode | Purpose | Claim-ready? |
|------|---------|--------------|
| **NativePoseQC** (`--native`) | Fast C++ diagnostic suite (load, connectivity, steric clash, protein distance/volume) | **No** — diagnostic / parity target |
| **Upstream `bust`** (`--bust`) | Official PoseBusters CLI | **Yes** — use for `pb_pass` / S2 gates |

Success for modern docking benchmarks is typically:

```text
RMSD ≤ 2.0 Å  ∧  PoseBusters pass (upstream bust)
```

NativePoseQC is useful for CI speed and fail-closed extraction checks; do not report it as official PoseBusters success.

## Requirements

- CMake ≥ 3.28
- C++26 compiler (Clang ≥ 18 / Apple Clang ≥ 16 / GCC ≥ 14 / MSVC ≥ 19.40)
- Optional: `bust` on `PATH` (or `POSEBUSTERS_BIN`) for `--bust`
- Optional: `inchi-1` for native `inchi_convertible` soft check

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DPOSEBUST_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Artifacts:

- `build/posebust` — CLI
- `build/libposebust.a` (or shared, if configured)
- `build/posebust_tests`

## CLI

```bash
# Clean-room native QC (SDF ligand + protein PDB)
./build/posebust --native --pred ligand.sdf --protein receptor.pdb

# With crystal topology reference
./build/posebust --native --pred pose.sdf --protein rec.pdb -l crystal.sdf --sidecar /tmp/pb

# FlexAID-style complex PDB (CONECT / optimizable residue extraction)
./build/posebust --paths complex.pdb receptor.pdb crystal.sdf

# Official PoseBusters CLI (must have `bust` installed)
./build/posebust --bust --pred pose.sdf --protein rec.pdb -l crystal.sdf
```

Exit code `0` = full suite pass (native) or `pb_pass` (bust); non-zero on fail / hard error.

## Library API

```cpp
#include <posebust/Engine.h>
#include <posebust/BustCli.h>
#include <posebust/Loaders.h>

posebust::Molecule lig, prot;
posebust::load_sdf("pose.sdf", lig);
posebust::load_pdb_protein_heavy("rec.pdb", prot);

posebust::EvaluateOptions opt;
opt.suite = posebust::Suite::Dock;
auto report = posebust::evaluate(lig, prot, /*ligand_true=*/nullptr, opt);

// Official claim gate (optional)
auto bust = posebust::run_upstream_bust("pose.sdf", "rec.pdb", "crystal.sdf");
bool pb_pass = bust.pb_pass;
```

CMake consumers:

```cmake
find_package(PoseBust REQUIRED)
target_link_libraries(my_app PRIVATE PoseBust::posebust)
```

## Environment

| Variable | Meaning |
|----------|---------|
| `POSEBUSTERS_BIN` | Path to upstream `bust` binary |
| `POSEBUST_BACKEND` | `native` \| `bust` \| `off` |
| `POSEBUST` | Set to `0` to disable |
| `POSEBUST_INCHI_BIN` | Optional `inchi-1` |
| `POSEBUST_ROOT` | Root for `.venv-posebusters/bin/bust` lookup |

**FlexAIDdS aliases** are still honored (`FLEXAIDDS_POSEBUSTERS_BIN`, `FLEXAIDDS_POSEBUST_BACKEND`, …) so existing harnesses keep working.

## Repository layout

```text
PoseBust/
├── include/posebust/   # public headers (Types, Engine, Loaders, checks, BustCli)
├── src/                # implementation
├── apps/posebust_main.cpp
├── tests/              # GoogleTest (self-contained fixtures)
├── CMakeLists.txt
└── LICENSE             # Apache-2.0
```

## Relationship to FlexAIDdS

- FlexAIDdS still ships a vendored copy under `LIB/PoseBust` for in-tree DatasetRunner.
- This repo is the **independent** product surface: same clean-room algorithms, namespace `posebust`, no docking engine linkage.
- Interop: `load_pdb_flexaid_ligand()` understands FlexAID CONECT / REMARK optimizable residues.

## Licensing & clean-room

- **This project**: Apache-2.0
- **PoseBusters** (optional external CLI): BSD — not vendored; invoked only via argv exec when present
- Algorithms are original; check *keys* match PoseBusters naming for parity. No posebusters/RDKit source is copied.

## Author

Le Bonhomme Pharma / Louis-Philippe Morency — extracted for independent use from FlexAIDdS.
