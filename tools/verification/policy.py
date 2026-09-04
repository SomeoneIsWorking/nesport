from __future__ import annotations

import re
from pathlib import Path


SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp"}
PRODUCT_ROOTS = ("include", "src")
FORBIDDEN_PRODUCT_PATTERNS = {
    "direct process diagnostics": re.compile(r"\b(?:fprintf\s*\(\s*stderr|std::cerr|printf\s*\()"),
    "environment access outside configuration": re.compile(r"\b(?:getenv|std::getenv)\s*\("),
    "offline guest-source generation": re.compile(r"\b(?:codegen|guest_generator|generated_guest)\b", re.IGNORECASE),
    "runtime backend selector": re.compile(r"\b(?:interpreter_mode|use_interpreter|cpu_backend)\b", re.IGNORECASE),
}
MAX_SOURCE_LINES = 1_200


def tracked_files(root: Path) -> list[Path]:
    files: list[Path] = []
    for path in root.rglob("*"):
        if not path.is_file() or any(part in {".git", "build", "scratch"} for part in path.parts):
            continue
        files.append(path)
    return files


def check(root: Path) -> None:
    violations: list[str] = []
    files = tracked_files(root)
    shell_files = [path for path in files if path.suffix == ".sh" and path.name != "run.sh"]
    violations.extend(f"non-launcher shell script: {path.relative_to(root)}" for path in shell_files)

    source_files = [path for path in files if path.suffix in SOURCE_SUFFIXES and path.parts[len(root.parts)] in PRODUCT_ROOTS]
    for path in source_files:
        text = path.read_text(encoding="utf-8")
        line_count = text.count("\n") + 1
        if line_count > MAX_SOURCE_LINES:
            violations.append(f"source file exceeds {MAX_SOURCE_LINES} lines: {path.relative_to(root)} ({line_count})")
        for label, pattern in FORBIDDEN_PRODUCT_PATTERNS.items():
            if pattern.search(text):
                violations.append(f"{label}: {path.relative_to(root)}")

    required = {
        "typed configuration": root / "include/nesport/config.h",
        "logger boundary": root / "include/nesport/logger.h",
        "native registry": root / "include/nesport/native_overrides.h",
        "runtime composition": root / "include/nesport/machine.h",
        "goals": root / "docs/project-goals.md",
        "state": root / "docs/project-state.md",
        "codemap": root / "docs/codemap.md",
    }
    violations.extend(f"missing {label}: {path.relative_to(root)}" for label, path in required.items() if not path.is_file())

    if violations:
        raise RuntimeError("repository policy failed:\n" + "\n".join(f"- {item}" for item in violations))
    print(f"repository policy: {len(files)} files, {len(source_files)} product sources, 0 violations")
