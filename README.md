# eos-relibc

**E-OS fork of [`redox-os/relibc`](https://gitlab.redox-os.org/redox-os/relibc).** Part of the [**E-OS**](https://github.com/Gh0s777tt/E-OS) ecosystem — a hardened, Crimson-branded downstream of [Redox OS](https://www.redox-os.org).

This repository is Redox's **C library (`relibc`) and dynamic loader (`ld.so`)**.

## E-OS changes vs upstream

- **`ld.so` fixes (R-402b)** — bind unresolved **weak** PLT symbols to 0; fix an ET_EXEC `d_val` overflow; correct static-TLS layout / alignment / TPOFF offsets.
- Link `ld.so` at vaddr 0 so its load base is **ASLR-randomized**; release overflow-checks.

## How it's pinned

The E-OS build pins this fork in [`recipes/core/relibc/recipe.toml`](https://github.com/Gh0s777tt/E-OS/blob/main/recipes/core/relibc/recipe.toml):

- branch **`eos-july`** · rev **`7e9a95d06ebf`**
- **7 commit(s) behind** upstream master

## Build standalone

This fork is normally built by the E-OS cookbook (`make CI=1 …` in the [main repo](https://github.com/Gh0s777tt/E-OS)). To build it on its own you need the Redox toolchain; see the main repo's [build guide](https://github.com/Gh0s777tt/E-OS/blob/main/docs/building.md).

## Hosting

**GitLab (source of truth):** https://gitlab.com/e-os/eos-relibc  
**GitHub (read-only mirror):** https://github.com/Gh0s777tt/eos-relibc

## License

MIT (inherited from upstream Redox). The E-OS project as a whole is AGPL-3.0; see the [main repo](https://github.com/Gh0s777tt/E-OS/blob/main/LICENSE).

---
[E-OS main repo](https://github.com/Gh0s777tt/E-OS) · [Docs](https://github.com/Gh0s777tt/E-OS/tree/main/docs) · [Upstream](https://gitlab.redox-os.org/redox-os/relibc)
