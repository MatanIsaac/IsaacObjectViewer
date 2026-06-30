#!/usr/bin/env python3
"""Generate compile_commands.json for clangd/IDE IntelliSense.

It captures the project's real compile commands from a dry-run of the
Makefile (`make -nB all`, covering both the app and the tests) and converts
them to a Clang compilation database. Paths are taken relative to the
current working directory, so run it from the project root:

    python tools/gen_compile_commands.py        # runs `make -nB all` itself
    make compile_commands                       # convenience target

The generated file is machine-specific (absolute paths) and git-ignored.
Re-run it whenever you add or rename source files.
"""
import json
import os
import shlex
import shutil
import subprocess
import sys


def get_make_dump():
    """Return the dry-run make output, from an arg file or by running make."""
    if len(sys.argv) > 1:
        with open(sys.argv[1], "r", errors="replace") as f:
            return f.read()
    make = os.environ.get("MAKE", "make")
    result = subprocess.run(
        [make, "-nB", "all"],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
    )
    return result.stdout


def main():
    root = os.getcwd().replace("\\", "/")
    gxx = (shutil.which("g++") or "g++").replace("\\", "/")

    entries = []
    seen = set()
    for line in get_make_dump().splitlines():
        line = line.strip()
        if " -c " not in line or not (line.startswith("g++") or line.startswith("gcc")):
            continue
        toks = shlex.split(line, posix=True)
        try:
            src = toks[toks.index("-c") + 1]
        except (ValueError, IndexError):
            continue
        if not src.endswith((".cpp", ".c", ".cc")):
            continue
        src = src.replace("\\", "/")
        if src in seen:
            continue
        seen.add(src)
        entries.append({
            "directory": root,
            "file": f"{root}/{src}",
            "arguments": [gxx] + toks[1:],
        })

    if not entries:
        sys.exit("No compile commands found. Is `make` on PATH and run from the project root?")

    with open(os.path.join(root, "compile_commands.json"), "w") as f:
        json.dump(entries, f, indent=2)
    print(f"Wrote {len(entries)} entries to compile_commands.json")


if __name__ == "__main__":
    main()
