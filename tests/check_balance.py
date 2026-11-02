#!/usr/bin/env python3
"""Run visible-board strategies against the same core and all delivered JSON.

This compares deterministic policies, not human difficulty or a win-rate study.
Timing jitter changes thinking time, not the level seed or game rules.
"""
import csv
import io
import hashlib
import json
import pathlib
import subprocess
import sys

binary = pathlib.Path(sys.argv[1]).resolve()
levels = pathlib.Path("ASuePotGame/levels")
out = pathlib.Path("ASuePotGame/tests")
# User acceptance: the new first stage is exactly the previous stage50.
baseline = json.loads((out / "fixtures/v08_level50.json").read_text(encoding="utf-8"))
first = json.loads((levels / "level_01.json").read_text(encoding="utf-8"))
for data in (baseline, first):
    data.pop("level_id"); data.pop("notes")
assert first == baseline, "New level1 no longer matches the delivered v0.8 level50"
# Existing v0.9 stages remain byte-for-byte unchanged in the100-stage expansion.
for name, digest in json.loads((out / "fixtures/v09_level_hashes.json").read_text()).items():
    assert hashlib.sha256((levels / name).read_bytes()).hexdigest() == digest, f"Changed original stage {name}"
# All100 existing stages are frozen for this expansion.
for name, digest in json.loads((out / "fixtures/v10_level_hashes.json").read_text()).items():
    assert hashlib.sha256((levels / name).read_bytes()).hexdigest() == digest, f"Changed v1.0 stage {name}"
assert {p.name for p in levels.glob("level_*.json")} == {f"level_{i:02d}.json" for i in range(1, 151)}
profiles = [
    ("plan100", 100, 3, 1, 0), ("space100", 100, 4, 1, 0),
    ("largest150", 150, 0, 1, 0), ("plan150", 150, 3, 1, 0),
    ("space150", 150, 4, 1, 0), ("largest250", 250, 0, 1, 0),
    ("plan250", 250, 3, 1, 0), ("space250", 250, 4, 1, 0),
    ("random350", 350, 1, 123, 0), ("first500", 500, 2, 1, 0),
    *[(f"plan_jitter{s}", 100, 3, s, 150) for s in range(1, 5)],
    *[(f"space_jitter{s}", 100, 4, s, 150) for s in range(1, 5)],
]
rows, by_profile, summary = [], {}, {}
for name, delay, policy, seed, jitter in profiles:
    run = subprocess.run([str(binary), str(levels), str(delay), str(policy),
                          str(seed), str(jitter)], capture_output=True, text=True, check=True)
    result = list(csv.DictReader(io.StringIO(run.stdout)))
    assert [int(r["level"]) for r in result] == list(range(1, 151))
    assert all(int(r["target"]) > 0 for r in result)
    by_profile[name] = result
    summary[name] = {"wins": sum(r["state"] == "4" for r in result),
                     "score_failures": sum(r["failure"] == "2" for r in result),
                     "overflow_failures": sum(r["failure"] == "1" for r in result)}
    for r in result:
        rows.append(dict(profile=name, delay_ms=delay, jitter_ms=jitter,
                         policy=policy, decision_seed=seed, **r))

with (out / "balance_results.csv").open("w", encoding="utf-8", newline="") as f:
    w = csv.DictWriter(f, fieldnames=rows[0].keys()); w.writeheader(); w.writerows(rows)
(out / "balance_summary.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
# Keep the complete diagnostic result even when a new target needs tuning.
# Every stage must have a non-instant path using only visible information.
normal = [name for name, *_ in profiles[:8]]
missing = [i+1 for i in range(150) if not any(by_profile[name][i]["state"] == "4" for name in normal)]
assert not missing, f"No timed path for levels {missing}"
print("PASS: all150 have a timed visible-board path; 18 profiles / 2700 trials. "
      f"random350 wins={summary['random350']['wins']}; first500 wins={summary['first500']['wins']}; "
      f"plan150 wins={summary['plan150']['wins']}; space150 wins={summary['space150']['wins']}. "
      "Not a human win-rate measurement.")
