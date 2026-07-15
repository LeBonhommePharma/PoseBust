# Applying the Apache License 2.0 to PoseBust files

Copyright owner for this project:

```text
Copyright 2026 Le Bonhomme Pharma
Copyright 2026 Louis-Philippe Morency
```

SPDX short identifier: **`Apache-2.0`**

## Source file header (C/C++)

Place at the top of every `.h` / `.cpp` file (after an optional one-line title):

```cpp
// <FileName> — <one-line purpose>
//
// Copyright 2026 Le Bonhomme Pharma
// SPDX-License-Identifier: Apache-2.0
```

Full legal notice (optional for short headers; the SPDX line is required):

```cpp
// Copyright 2026 Le Bonhomme Pharma
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
```

## Documentation / scripts

Markdown and shell scripts:

```text
<!-- Copyright 2026 Le Bonhomme Pharma -->
<!-- SPDX-License-Identifier: Apache-2.0 -->
```

or

```bash
# Copyright 2026 Le Bonhomme Pharma
# SPDX-License-Identifier: Apache-2.0
```

## Distribution package contents

When shipping binary or source distributions, include at least:

1. `LICENSE` — full Apache-2.0 text  
2. `NOTICE` — copyright and attribution  
3. `THIRD_PARTY_LICENSES.md` — optional/external tools  

## Contributions

By submitting a pull request, contributors license their changes under
Apache-2.0 to the project copyright holders, as stated in `CONTRIBUTING.md`.
