#!/usr/bin/env python3
"""Lightweight repository guideline checks.

Emits structured violations with fields:
  file, line, rule_id, severity, message
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, List, Tuple


SOURCE_EXTS = {".ino", ".h", ".hpp", ".c", ".cc", ".cpp"}
TEXT_EXTS = SOURCE_EXTS | {".md", ".txt", ".py", ".sh"}


@dataclass
class Violation:
    file: str
    line: int
    rule_id: str
    severity: str
    message: str


def run_git(args: List[str]) -> str:
    try:
        out = subprocess.check_output(["git", *args], stderr=subprocess.DEVNULL)
    except Exception:
        return ""
    return out.decode("utf-8", errors="replace")


def changed_files() -> List[Path]:
    staged = run_git(["diff", "--cached", "--name-only", "--diff-filter=ACMR"])
    unstaged = run_git(["diff", "--name-only", "--diff-filter=ACMR"])
    untracked = run_git(["ls-files", "--others", "--exclude-standard"])
    names = {n.strip() for n in (staged + "\n" + unstaged + "\n" + untracked).splitlines() if n.strip()}
    return [Path(n) for n in sorted(names)]


def all_repo_files(root: Path) -> List[Path]:
    out = run_git(["ls-files"])
    if not out:
        return [p for p in root.iterdir() if p.is_file()]
    return [Path(line.strip()) for line in out.splitlines() if line.strip()]


def is_text_candidate(path: Path) -> bool:
    return path.suffix.lower() in TEXT_EXTS


def is_source(path: Path) -> bool:
    return path.suffix.lower() in SOURCE_EXTS


def read_lines(path: Path) -> List[str]:
    try:
        return path.read_text(encoding="utf-8").splitlines()
    except UnicodeDecodeError:
        return path.read_text(encoding="latin-1").splitlines()


def collect_waivers(lines: List[str]) -> dict[int, set[str]]:
    waivers: dict[int, set[str]] = {}
    for i, line in enumerate(lines, start=1):
        m = re.search(r"GUIDELINE-WAIVER:\s*([0-9]+\.[0-9]+)", line)
        if m:
            waivers.setdefault(i, set()).add(m.group(1))
    return waivers


def has_waiver(waivers: dict[int, set[str]], line_no: int, rule_id: str) -> bool:
    for ln in range(max(1, line_no - 2), line_no + 1):
        if rule_id in waivers.get(ln, set()):
            return True
    return False


def add(vs: List[Violation], path: Path, line: int, rule_id: str, severity: str, message: str, waivers: dict[int, set[str]]):
    if has_waiver(waivers, line, rule_id):
        return
    vs.append(Violation(str(path), line, rule_id, severity, message))


def check_whitespace(path: Path, lines: List[str], vs: List[Violation], waivers):
    blank_run = 0
    for i, line in enumerate(lines, start=1):
        if re.search(r"[ \t]+$", line):
            add(vs, path, i, "8.2", "ADVISORY", "Trailing whitespace.", waivers)
        if line.strip() == "":
            blank_run += 1
            if blank_run > 2:
                add(vs, path, i, "8.3", "ADVISORY", "More than 2 consecutive blank lines.", waivers)
        else:
            blank_run = 0


def check_file_length(path: Path, lines: List[str], vs: List[Violation], waivers):
    n = len(lines)
    if n > 800:
        add(vs, path, 1, "1.4", "BLOCKER", f"File length {n} exceeds max 800 lines.", waivers)
    elif n > 500:
        add(vs, path, 1, "1.4", "STRONG", f"File length {n} exceeds limit 500 lines.", waivers)


def iter_function_spans(lines: List[str]) -> Iterable[Tuple[int, int, str, str]]:
    pending_start = None
    pending_sig = ""
    depth = 0
    fn_start = None
    fn_sig = ""
    for i, line in enumerate(lines, start=1):
        stripped = line.strip()
        if depth == 0:
            if pending_start is None:
                if "(" in line and not stripped.startswith("#"):
                    pending_start = i
                    pending_sig = stripped
            else:
                pending_sig += " " + stripped
            if pending_start is not None and "{" in line:
                sig = pending_sig
                if re.search(r"\b(if|for|while|switch|catch)\s*\(", sig):
                    pending_start = None
                    pending_sig = ""
                else:
                    fn_start = pending_start
                    fn_sig = sig
                    pending_start = None
                    pending_sig = ""
        depth += line.count("{") - line.count("}")
        if fn_start is not None and depth == 0:
            yield fn_start, i, fn_sig, ""
            fn_start = None
            fn_sig = ""


def check_function_length(path: Path, lines: List[str], vs: List[Violation], waivers):
    for start, end, sig, _ in iter_function_spans(lines):
        length = end - start + 1
        if length > 40:
            add(vs, path, start, "1.1", "BLOCKER", f"Function length {length} exceeds max 40 lines.", waivers)
        elif length > 30:
            add(vs, path, start, "1.1", "STRONG", f"Function length {length} exceeds limit 30 lines.", waivers)


def count_bool_params(sig: str) -> int:
    inside = re.search(r"\((.*)\)", sig)
    if not inside:
        return 0
    params = inside.group(1)
    return len(re.findall(r"\bbool\b", params))


def check_bool_params(path: Path, lines: List[str], vs: List[Violation], waivers):
    for start, _end, sig, _ in iter_function_spans(lines):
        n = count_bool_params(sig)
        if n >= 2:
            add(vs, path, start, "2.2", "STRONG", f"Function has {n} bool parameters (>=2).", waivers)


def check_header_include_count(path: Path, lines: List[str], vs: List[Violation], waivers):
    if path.suffix.lower() not in {".h", ".hpp"}:
        return
    includes = [i for i, l in enumerate(lines, start=1) if l.strip().startswith("#include")]
    if len(includes) > 8:
        add(vs, path, includes[8], "4.3", "BLOCKER", f"Header include count {len(includes)} exceeds 8.", waivers)


def check_waiver_prefix(path: Path, lines: List[str], vs: List[Violation], waivers):
    if not is_source(path):
        return
    valid = re.compile(r"GUIDELINE-WAIVER:\s*[0-9]+\.[0-9]+")
    for i, line in enumerate(lines, start=1):
        if "GUIDELINE-WAIVER" in line and not valid.search(line):
            vs.append(Violation(str(path), i, "WAIVER-PREFIX", "STRONG", "Malformed waiver. Use: GUIDELINE-WAIVER: <rule-id> <reason>."))
        if "waiver" in line.lower() and "GUIDELINE-WAIVER" not in line and ("//" in line or "/*" in line):
            vs.append(Violation(str(path), i, "WAIVER-PREFIX", "STRONG", "Waiver comment must use exact prefix: GUIDELINE-WAIVER: <rule-id>."))


def check_log_format(path: Path, lines: List[str], vs: List[Violation], waivers):
    i = 0
    while i < len(lines):
        line = lines[i]
        if "SerialPrint::error" not in line:
            i += 1
            continue
        start = i + 1
        stmt = line
        while ");" not in stmt and i + 1 < len(lines):
            i += 1
            stmt += " " + lines[i].strip()
        literals = " ".join(re.findall(r'"([^"]*)"', stmt))
        if "Location::" not in literals:
            add(vs, path, start, "10.2", "STRONG", "Error log must include Location:: tag.", waivers)
        if "Errors::" not in literals:
            add(vs, path, start, "10.2", "STRONG", "Error log must include stable Errors:: identifier.", waivers)
        if not re.search(r"\b[a-zA-Z0-9_.-]+=", literals):
            add(vs, path, start, "10.2", "STRONG", "Error log must include key context fields (key=value).", waivers)
        i += 1


def check_ino_discipline(path: Path, lines: List[str], vs: List[Violation], waivers):
    if path.suffix.lower() != ".ino":
        return

    for start, end, sig, _ in iter_function_spans(lines):
        before_paren = sig.split("(", 1)[0].strip()
        name = before_paren.split()[-1] if before_paren else ""
        if name in {"setup", "loop"}:
            body = "\n".join(lines[start - 1 : end])
            if re.search(r"\b(if|for|while|switch)\b", body):
                add(vs, path, start, "6.1", "BLOCKER", f"{name}() contains control-flow logic; delegate to module functions.", waivers)
        else:
            add(vs, path, start, "6.1", "BLOCKER", f".ino must not define helper function '{name}'.", waivers)


def gather_files(root: Path, changed_only_flag: bool) -> List[Path]:
    candidates = changed_files() if changed_only_flag else all_repo_files(root)
    out: List[Path] = []
    for rel in candidates:
        p = (root / rel).resolve()
        if not p.exists() or not p.is_file():
            continue
        if ".git" in p.parts:
            continue
        if is_text_candidate(rel):
            out.append(rel)
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--changed-only", action="store_true", help="Scan only changed files (staged + unstaged).")
    ap.add_argument("--format", choices=["text", "json"], default="text")
    args = ap.parse_args()

    root = Path.cwd()
    files = gather_files(root, args.changed_only)

    violations: List[Violation] = []
    for rel in files:
        path = root / rel
        lines = read_lines(path)
        waivers = collect_waivers(lines)

        check_waiver_prefix(rel, lines, violations, waivers)
        check_whitespace(rel, lines, violations, waivers)
        check_file_length(rel, lines, violations, waivers)

        if is_source(rel):
            check_function_length(rel, lines, violations, waivers)
            check_bool_params(rel, lines, violations, waivers)
            check_log_format(rel, lines, violations, waivers)
            check_header_include_count(rel, lines, violations, waivers)
            check_ino_discipline(rel, lines, violations, waivers)

    violations.sort(key=lambda v: (v.file, v.line, v.rule_id))

    if args.format == "json":
        print(json.dumps([asdict(v) for v in violations], indent=2))
    else:
        for v in violations:
            print(f"{v.file}:{v.line}: {v.severity} {v.rule_id}: {v.message}")
        print(f"Scanned files: {len(files)} | Violations: {len(violations)}")

    has_blocking = any(v.severity in {"BLOCKER", "STRONG"} for v in violations)
    return 1 if has_blocking else 0


if __name__ == "__main__":
    sys.exit(main())
