from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Mapping, Sequence


def run(command: Sequence[str], *, root: Path, environment: Mapping[str, str] | None = None) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=root, env=environment, check=True)


def capture(command: Sequence[str], *, root: Path) -> str:
    return subprocess.run(command, cwd=root, check=True, text=True, stdout=subprocess.PIPE).stdout.strip()
