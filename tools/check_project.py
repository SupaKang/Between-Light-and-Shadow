from pathlib import Path
import sys

required = [
    Path("AGENTS.md"), Path("README.md"), Path("CMakeLists.txt"),
    Path("src/main.cpp"), Path("data/DATA_RULES.md"),
    Path("docs/tech/TECH_SPIKE.md"), Path("docs/tech/SIZE_POLICY.md"),
    Path("tools/validate_data.py")
]
missing = [str(p) for p in required if not p.exists()]
if missing:
    print("[FAIL] missing required files"); print("\n".join(missing)); sys.exit(1)
print(f"[PASS] project structure verified ({len(required)} required files)")
