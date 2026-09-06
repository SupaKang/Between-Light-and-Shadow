from pathlib import Path
import json, sys

root = Path("data"); errors = []
skill_ids = {json.loads(p.read_text(encoding="utf-8")).get("id") for p in root.glob("skill_*.json") if "schema" not in p.name}
yokai_ids = {json.loads(p.read_text(encoding="utf-8")).get("id") for p in root.glob("yokai_*.json") if "schema" not in p.name and isinstance(json.loads(p.read_text(encoding="utf-8")), dict)}
for path in sorted(root.glob("*.json")):
    try: obj=json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc: errors.append(f"{path}: invalid JSON ({exc})"); continue
    if not isinstance(obj, dict): continue
    if path.name.startswith("yokai_") and "schema" not in path.name:
        for key in ("id","name_ko","origin","grade","level","stats","skills"):
            if key not in obj: errors.append(f"{path}: missing {key}")
        if obj.get("grade") not in {"I","II","III","IV","V"}: errors.append(f"{path}: invalid grade")
        if not 1 <= obj.get("level",0) <= 50: errors.append(f"{path}: invalid level")
        if len(obj.get("skills",[])) > 4: errors.append(f"{path}: more than 4 skills")
        for ref in obj.get("skills",[]):
            if ref not in skill_ids: errors.append(f"{path}: missing skill reference {ref}")
    if path.name.startswith("skill_") and "schema" not in path.name:
        for key in ("id","name_ko","power","qi_cost","accuracy"): 
            if key not in obj: errors.append(f"{path}: missing {key}")
        if obj.get("power", -1) < 0: errors.append(f"{path}: invalid power")
        if obj.get("qi_cost", -1) < 0: errors.append(f"{path}: invalid qi cost")
        if not 0 <= obj.get("accuracy", -1) <= 100: errors.append(f"{path}: invalid accuracy")
        if not 0 <= obj.get("seal_turns", 0) <= 10: errors.append(f"{path}: invalid seal duration")
    if path.name.startswith("encounter_") and obj.get("encounter_step_interval",0) <= 0: errors.append(f"{path}: invalid interval")
    if path.name.startswith("encounter_"):
        for entry in obj.get("encounters",[]):
            if entry.get("yokai_id") not in yokai_ids: errors.append(f"{path}: missing yokai reference {entry.get('yokai_id')}")
    if path.name == "status_effects.json":
        required = {"burn", "freeze", "seal", "paralysis", "fear"}
        found = {s.get("id") for s in obj.get("statuses", [])}
        for status in sorted(required - found): errors.append(f"{path}: missing status {status}")
        for status in obj.get("statuses", []):
            if not 1 <= status.get("duration_turns", 0) <= 10: errors.append(f"{path}: invalid duration {status.get('id')}")
            if not 0 <= status.get("action_fail_rate", -1) <= 1: errors.append(f"{path}: invalid fail rate {status.get('id')}")
            if status.get("damage_taken_multiplier", 0) < 0: errors.append(f"{path}: invalid damage multiplier {status.get('id')}")
    if path.name.startswith("artifact_") and "schema" not in path.name:
        for key in ("id", "name_ko", "origin", "passive", "drawback", "destroyable"):
            if key not in obj: errors.append(f"{path}: missing {key}")
        if not isinstance(obj.get("destroyable"), bool): errors.append(f"{path}: destroyable must be boolean")
if errors:
    print("[FAIL]"); print("\n".join(errors)); sys.exit(1)
print(f"[PASS] validated {len(list(root.glob('*.json')))} JSON data files")
