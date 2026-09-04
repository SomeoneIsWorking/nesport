#!/usr/bin/env python3
from __future__ import annotations

import argparse
import os
from pathlib import Path

from verification import policy
from verification.process import capture, run


def source_files(root: Path) -> list[str]:
    paths: list[Path] = []
    for directory in (root / "include", root / "src", root / "tests"):
        paths.extend(path for path in directory.rglob("*") if path.suffix in policy.SOURCE_SUFFIXES)
    return [str(path.relative_to(root)) for path in sorted(paths)]


def verify(root: Path, *, jobs: int) -> None:
    policy.check(root)
    sources = source_files(root)
    clang_format = os.environ.get("CLANG_FORMAT", "clang-format")
    clang_tidy = os.environ.get("CLANG_TIDY", "clang-tidy")
    run([clang_format, "--dry-run", "--Werror", *sources], root=root)

    build = root / "build" / "verify"
    environment = dict(os.environ)
    environment.setdefault("CC", "clang")
    environment.setdefault("CXX", "clang++")
    configure = [
        "cmake", "-S", ".", "-B", str(build), "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=RelWithDebInfo", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
    ]
    mesence_source = environment.get("MESENCE_SOURCE_DIR")
    if mesence_source:
        configure.append(f"-DMESENCE_SOURCE_DIR={mesence_source}")
    toolchain = environment.get("CMAKE_TOOLCHAIN_FILE")
    if toolchain:
        configure.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain}")
    triplet = environment.get("VCPKG_TARGET_TRIPLET")
    if triplet:
        configure.append(f"-DVCPKG_TARGET_TRIPLET={triplet}")
    run(configure, root=root, environment=environment)
    cache = capture(["cmake", "-LA", "-N", str(build)], root=root)
    if "CMAKE_CXX_COMPILER:FILEPATH=" not in cache or not any(compiler in cache for compiler in ("clang++", "clang-cl")):
        raise RuntimeError("verification build is not configured with clang++ or clang-cl")
    run(["cmake", "--build", str(build), "--target", "nesport_runtime_test", "-j", str(jobs)], root=root, environment=environment)
    run(["ctest", "--test-dir", str(build), "--output-on-failure"], root=root, environment=environment)
    run([clang_tidy, "-p", str(build), "src/machine.cpp", "src/native_overrides.cpp"], root=root, environment=environment)
    print("nesport verification passed")


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the nesport landing gate")
    parser.add_argument("--jobs", type=int, default=max(1, min(8, os.cpu_count() or 1)))
    args = parser.parse_args()
    verify(Path(__file__).resolve().parents[1], jobs=args.jobs)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
