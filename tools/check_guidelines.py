#!/usr/bin/env python3
"""Lightweight guideline checker for Auto Aqua.

Checks a focused subset of CODE_GUIDELINES.md with severity tags:
- BLOCKER: 4.1 header guard, 5.1 raw memory calls
- STRONG: 4.4 using namespace in headers, 5.4 C-style casts
- ADVISORY: 1.3 line width >120, 8.2 trailing whitespace
"""

from __future__ import annotations

import argparse
import dataclasses
import pathlib
import re
import subprocess
import sys
from collections import Counter

SOURCE_EXTS = {".ino", ".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hh"}
SEVERITIES = ("BLOCKER", "STRONG", "ADVISORY")


@dataclasses.dataclass
class Finding:
    severity: str
    rule_id: str
    path: pathlib.Path
    line: int
    message: str


def run_git_changed() -> list[pathlib.Path]:
    commands = [
        ["git", "diff", "--cached", "--name-only", "--diff-filter=ACMRTUXB"],
        ["git", "diff", "--name-only", "--diff-filter=ACMRTUXB", "HEAD"],
    ]
    for cmd in commands:
        try:
            out = subprocess.check_output(cmd, text=True).splitlines()
        except subprocess.CalledProcessError:
            continue
        paths = [pathlib.Path(line.strip()) for line in out if line.strip()]
        if paths:
            return paths
    return []


def list_repo_sources() -> list[pathlib.Path]:
    try:
        out = subprocess.check_output(["git", "ls-files"], text=True).splitlines()
    except subprocess.CalledProcessError:
        return []
    return [pathlib.Path(p) for p in out if pathlib.Path(p).suffix.lower() in SOURCE_EXTS]


def should_skip_line(line: str) -> bool:
    return "GUIDELINE-WAIVER:" in line


def check_file(path: pathlib.Path) -> list[Finding]:
    findings: list[Finding] = []
    suffix = path.suffix.lower()
    try:
        content = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        content = path.read_text(encoding="latin-1")
    lines = content.splitlines()

    if suffix in {".h", ".hpp", ".hh"}:
        first_code = ""
        for line in lines:
            stripped = line.strip()
            if stripped and not stripped.startswith("//"):
                first_code = stripped
                break
        if first_code != "#pragma once":
            findings.append(Finding("BLOCKER", "4.1", path, 1, "header missing '#pragma once'"))

    for i, line in enumerate(lines, start=1):
        stripped = line.strip()
        if should_skip_line(line):
            continue

        if suffix in {".h", ".hpp", ".hh"} and re.search(r"\busing\s+namespace\b", line):
            findings.append(Finding("STRONG", "4.4", path, i, "avoid 'using namespace' in headers"))

        if re.search(r"\b(new|delete|malloc|free)\b", line):
            findings.append(Finding("BLOCKER", "5.1", path, i, "raw memory API usage detected"))

        cast_match = re.search(r"\([^\)]+\)\s*[A-Za-z_][\w\->\[\]\.]*", line)
        if cast_match and not re.search(r"\b(static_cast|reinterpret_cast|const_cast|dynamic_cast)\s*<", line):
            if not stripped.startswith("#"):
                findings.append(Finding("STRONG", "5.4", path, i, "possible C-style cast"))

        if len(line) > 120:
            findings.append(Finding("ADVISORY", "1.3", path, i, "line exceeds 120 characters"))

        if line.rstrip(" \t") != line:
            findings.append(Finding("ADVISORY", "8.2", path, i, "trailing whitespace"))

    return findings


def format_table(findings: list[Finding]) -> str:
    if not findings:
        return "No violations found."
    header = "| Severity | Rule | File | Line | Message |\n|---|---|---|---:|---|"
    rows = [
        f"| {f.severity} | {f.rule_id} | `{f.path}` | {f.line} | {f.message} |"
        for f in findings
    ]
    return "\n".join([header, *rows])


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("files", nargs="*")
    parser.add_argument("--changed-only", action="store_true")
    parser.add_argument("--report", type=pathlib.Path)
    parser.add_argument("--fail-on", nargs="*", default=["BLOCKER", "STRONG"])
    args = parser.parse_args()

    fail_on = {s.upper() for s in args.fail_on}
    unknown = fail_on - set(SEVERITIES)
    if unknown:
        print(f"Unknown severities in --fail-on: {sorted(unknown)}", file=sys.stderr)
        return 2

    if args.files:
        candidates = [pathlib.Path(f) for f in args.files]
    elif args.changed_only:
        candidates = run_git_changed()
    else:
        candidates = list_repo_sources()

    files = [p for p in candidates if p.exists() and p.suffix.lower() in SOURCE_EXTS]

    findings: list[Finding] = []
    for path in sorted(set(files)):
        findings.extend(check_file(path))

    counts = Counter(f.severity for f in findings)
    summary = (
        f"Checked {len(files)} files | BLOCKER={counts['BLOCKER']} "
        f"STRONG={counts['STRONG']} ADVISORY={counts['ADVISORY']}"
    )

    print(summary)
    table = format_table(findings)
    print(table)

    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(summary + "\n\n" + table + "\n", encoding="utf-8")

    if any(f.severity in fail_on for f in findings):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
