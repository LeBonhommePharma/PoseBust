# Clean-Room Policy

**PoseBust** is Apache-2.0 first-party code. Contributors and agents must keep it
that way.

## Allowed

- Original implementations from first principles (geometry, graph connectivity,
  steric clash heuristics, PDB/SDF I/O).
- Public documentation, papers, and **published check names** used as
  interoperability labels (e.g. PoseBusters report column keys).
- Permissive dependencies only if needed later: Apache-2.0, BSD, MIT, MPL-2.0
  (document in `THIRD_PARTY_LICENSES.md` and `NOTICE` if incorporated).
- Subprocess invocation of user-installed tools (PoseBusters `bust`, `inchi-1`,
  `openssl`) without vendoring their source.

## Forbidden

- Copying or translating source from **PoseBusters**, **RDKit**, or any
  GPL/AGPL project into this tree.
- Linking GPL/AGPL libraries into `libposebust` or the CLI.
- “Inspired by” rewrites that closely track GPL source structure or comments.
- Committing proprietary binary blobs without a documented, compatible license.

## Claim vs diagnostic

| Backend | License of this code | External tool | Claim-ready `pb_pass`? |
|---------|----------------------|---------------|------------------------|
| NativePoseQC (`--native`) | Apache-2.0 (this repo) | none required | **No** — diagnostic |
| Upstream bust (`--bust`) | Apache-2.0 driver only | BSD PoseBusters CLI | **Yes** — when tool present |

Never describe NativePoseQC-only results as official PoseBusters success.

## PR checklist

- [ ] New files carry Copyright + `SPDX-License-Identifier: Apache-2.0`
- [ ] No new dependency without license review in `THIRD_PARTY_LICENSES.md`
- [ ] No vendored third-party source without NOTICE update
- [ ] Tests pass: `ctest --test-dir build --output-on-failure`
