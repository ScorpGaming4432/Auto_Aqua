#!/usr/bin/env python3
"""Generate compile_commands.json for Arduino sketches from verbose arduino-cli output."""

from __future__ import annotations

import argparse
import json
import pathlib
import shlex
import subprocess
import sys
from typing import Iterable

CPP_EXTENSIONS = (".cpp", ".cc", ".cxx", ".ino.cpp")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Run `arduino-cli compile --verbose` and transform compiler invocations "
            "into compile_commands.json entries for .cpp and .ino files."
        )
    )
    parser.add_argument("--fqbn", required=True, help="Fully Qualified Board Name.")
    parser.add_argument(
        "--sketch",
        default=".",
        help="Sketch directory or sketch .ino path (default: current directory).",
    )
    parser.add_argument(
        "--build-path",
        default=".arduino-build",
        help="Build directory passed to arduino-cli (default: .arduino-build).",
    )
    parser.add_argument(
        "--output",
        default="compile_commands.json",
        help="Output compile_commands.json path (default: compile_commands.json).",
    )
    parser.add_argument(
        "--arduino-cli",
        default="arduino-cli",
        help="arduino-cli executable (default: arduino-cli).",
    )
    return parser.parse_args()


def normalize_path(path_str: str, cwd: pathlib.Path) -> str:
    path = pathlib.Path(path_str)
    if not path.is_absolute():
        path = cwd / path
    return str(path.resolve())


def infer_sketch_file(sketch_arg: str, cwd: pathlib.Path) -> pathlib.Path:
    sketch_path = pathlib.Path(sketch_arg)
    if not sketch_path.is_absolute():
        sketch_path = cwd / sketch_path
    sketch_path = sketch_path.resolve()

    if sketch_path.suffix == ".ino":
        return sketch_path

    ino_candidates = sorted(sketch_path.glob("*.ino"))
    if not ino_candidates:
        raise FileNotFoundError(f"No .ino file found in sketch directory: {sketch_path}")

    preferred = sketch_path / f"{sketch_path.name}.ino"
    if preferred.exists():
        return preferred.resolve()

    return ino_candidates[0].resolve()


def find_source_argument(arguments: Iterable[str]) -> tuple[int, str] | tuple[None, None]:
    for index, token in enumerate(arguments):
        if token.endswith(CPP_EXTENSIONS):
            return index, token
    return None, None


def parse_compile_invocations(raw_output: str, cwd: pathlib.Path) -> list[list[str]]:
    commands: list[list[str]] = []
    for line in raw_output.splitlines():
        if " -c " not in line:
            continue
        if not any(compiler in line for compiler in ("g++", "clang++", "c++")):
            continue

        try:
            tokens = shlex.split(line)
        except ValueError:
            continue

        if not tokens:
            continue

        _, source = find_source_argument(tokens)
        if not source:
            continue

        if source.endswith(CPP_EXTENSIONS):
            tokens[0] = normalize_path(tokens[0], cwd)
            commands.append(tokens)
    return commands


def make_compile_database(
    command_lines: list[list[str]], sketch_file: pathlib.Path, cwd: pathlib.Path
) -> list[dict[str, object]]:
    database: list[dict[str, object]] = []
    seen_files: set[str] = set()

    for arguments in command_lines:
        source_index, source = find_source_argument(arguments)
        if source_index is None or source is None:
            continue

        normalized_source = normalize_path(source, cwd)
        normalized_args = [normalize_path(arg, cwd) if i == source_index else arg for i, arg in enumerate(arguments)]

        if normalized_source not in seen_files:
            database.append(
                {
                    "directory": str(cwd),
                    "file": normalized_source,
                    "arguments": normalized_args,
                }
            )
            seen_files.add(normalized_source)

        if normalized_source.endswith(".ino.cpp"):
            ino_args = list(normalized_args)
            ino_source = str(sketch_file)
            ino_args[source_index] = ino_source
            if "-x" not in ino_args:
                ino_args.extend(["-x", "c++"])
            if "-include" not in ino_args:
                ino_args.extend(["-include", "Arduino.h"])

            if ino_source not in seen_files:
                database.append(
                    {
                        "directory": str(cwd),
                        "file": ino_source,
                        "arguments": ino_args,
                    }
                )
                seen_files.add(ino_source)

    return database


def run_arduino_compile(args: argparse.Namespace, cwd: pathlib.Path) -> str:
    compile_cmd = [
        args.arduino_cli,
        "compile",
        "--fqbn",
        args.fqbn,
        "--build-path",
        str((cwd / args.build_path).resolve()),
        "--verbose",
        str((cwd / args.sketch).resolve()),
    ]

    process = subprocess.run(
        compile_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )

    if process.returncode != 0:
        print(process.stdout, file=sys.stderr)
        raise RuntimeError(f"arduino-cli compile failed with exit code {process.returncode}")

    return process.stdout


def main() -> int:
    args = parse_args()
    cwd = pathlib.Path.cwd()
    sketch_file = infer_sketch_file(args.sketch, cwd)
    output_path = pathlib.Path(args.output)
    if not output_path.is_absolute():
        output_path = (cwd / output_path).resolve()

    raw_output = run_arduino_compile(args, cwd)
    commands = parse_compile_invocations(raw_output, cwd)
    database = make_compile_database(commands, sketch_file, cwd)

    if not database:
        raise RuntimeError(
            "No C++ compile invocations were found in verbose arduino-cli output. "
            "Ensure `arduino-cli compile --verbose` produced compiler command lines."
        )

    output_path.write_text(json.dumps(database, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {len(database)} translation units to {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
