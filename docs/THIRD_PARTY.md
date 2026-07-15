# Third-party notes

## This repository

- All first-party C++ sources: **Apache-2.0** (Copyright 2026 Le Bonhomme Pharma).
- Clean-room implementation of pose-validation *checks* using publicly documented PoseBusters check **names** for parity. No PoseBusters or RDKit source code is included.

## Optional external tools (not vendored)

| Tool | Role | License | How used |
|------|------|---------|----------|
| [PoseBusters](https://github.com/maabuu/posebusters) `bust` | Official `pb_pass` claim gate | BSD | Subprocess argv exec only when user selects `--bust` |
| InChI `inchi-1` | Optional native `inchi_convertible` | IUPAC/InChI | Subprocess when binary present |
| OpenSSL `openssl dgst` | File SHA-256 helper | Apache-2.0 | Subprocess for `sha256_file` |
| GoogleTest (FetchContent) | Unit tests | BSD-3 | Test builds only |

## Forbidden

- GPL / AGPL dependencies or source inspiration for incorporated code.
- Vendoring RDKit or posebusters Python package into this tree.
