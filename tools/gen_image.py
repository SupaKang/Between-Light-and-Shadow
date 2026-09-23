#!/usr/bin/env python3
"""Generate art for the prompt manifest.

Providers
  hf     (default, free, no key): public Hugging Face FLUX Spaces via their Gradio API
         (multimodalart/FLUX.1-merged, then black-forest-labs/FLUX.1-schnell). ~10-30 s per image;
         the anonymous ZeroGPU quota is small (set HF_TOKEN from a free account for more).
  horde  (free): AI Horde community GPUs, https://aihorde.net. Anonymous jobs sit at the bottom of the
         queue; set AI_HORDE_KEY (free registration) for priority. Default model "AIO Pixel Art".
  gemini : Google Gemini image models. Key from GEMINI_API_KEY or the nano-banana MCP entry in ~/.claude.json.

  tools/gen_image.py batch assets/prompts/manifest.json [id-prefix ...] [--provider horde|gemini]
                     [--model M] [--limit N] [--parallel P] [--force]      (--provider default: hf)
  tools/gen_image.py gemini-list

batch skips files that already exist (unless --force). Output is always PNG at the manifest's "out" path.
"""
import base64
import io
import json
import os
import re
import sys
import time
import urllib.error
import urllib.request

# ---------------------------------------------------------------- Gemini
GEMINI = "https://generativelanguage.googleapis.com/v1beta/models"


def gemini_key():
    if os.environ.get("GEMINI_API_KEY"):
        return os.environ["GEMINI_API_KEY"]
    m = re.search(r'"GEMINI_API_KEY"\s*:\s*"([^"]+)"', open(os.path.expanduser("~/.claude.json")).read())
    if not m:
        raise SystemExit("GEMINI_API_KEY not found")
    return m.group(1)


def gemini_generate(model, text):
    body = {"contents": [{"parts": [{"text": text}]}], "generationConfig": {"responseModalities": ["IMAGE"]}}
    req = urllib.request.Request(f"{GEMINI}/{model}:generateContent?key={gemini_key()}", data=json.dumps(body).encode(),
                                 headers={"Content-Type": "application/json"})
    d = json.load(urllib.request.urlopen(req, timeout=240))
    for p in d["candidates"][0]["content"]["parts"]:
        if "inlineData" in p:
            return base64.b64decode(p["inlineData"]["data"])
    raise RuntimeError("no image in response: " + json.dumps(d)[:200])


# ---------------------------------------------------------------- Hugging Face Spaces (FLUX)
# Tried in order; each builds the Gradio /infer argument list. ZeroGPU Spaces share one anonymous quota.
HF_SPACES = [
    ("https://multimodalart-flux-1-merged.hf.space/gradio_api", lambda p, s, w, h: [p, s, False, w, h, 3.5, 8]),
    ("https://black-forest-labs-flux-1-schnell.hf.space/gradio_api", lambda p, s, w, h: [p, s, False, w, h, 4]),
]
HF_SIZE = {"field_down": (512, 768), "field_side": (512, 768), "field_up": (512, 768), "portrait": (768, 768), "battle_front": (768, 768), "battle_back": (768, 768),
           "tileset": (1024, 1024), "item_icon": (512, 512), "mini_icon": (512, 512), "ui_sheet": (1024, 576), "concept": (1024, 576)}


def hf_generate(entry, seed=11):
    h = {"Content-Type": "application/json", "User-Agent": "Mozilla/5.0 yinyang-chronicle/0.2"}
    if os.environ.get("HF_TOKEN"):
        h["Authorization"] = "Bearer " + os.environ["HF_TOKEN"]
    w, ht = HF_SIZE.get(entry["type"], (768, 768))
    last = ""
    for base, args in HF_SPACES:
        try:
            body = json.dumps({"data": args(entry["prompt"], seed, w, ht)}).encode()
            eid = json.load(urllib.request.urlopen(urllib.request.Request(base + "/call/infer", data=body, headers=h), timeout=60))["event_id"]
            stream = urllib.request.urlopen(urllib.request.Request(f"{base}/call/infer/{eid}", headers=h), timeout=400).read().decode()
        except Exception as err:
            last = str(err); continue
        for line in stream.splitlines():
            if line.startswith("data:") and '"url"' in line:
                url = json.loads(line[5:])[0]["url"]
                return urllib.request.urlopen(urllib.request.Request(url, headers=h), timeout=120).read()
        last = stream[-200:]
    raise RuntimeError("all hf spaces failed (quota?): " + last.replace("\n", " "))


# ---------------------------------------------------------------- AI Horde
HORDE = "https://aihorde.net/api/v2"
HORDE_HEADERS = {"Client-Agent": "yinyang-chronicle:0.2:github.com/supakang", "User-Agent": "Mozilla/5.0 yinyang-chronicle/0.2",
                 "Content-Type": "application/json", "apikey": os.environ.get("AI_HORDE_KEY", "0000000000")}
HORDE_SIZE = {"field_down": (512, 768), "field_side": (512, 768), "field_up": (512, 768), "portrait": (512, 512), "battle_front": (512, 512), "battle_back": (512, 512),
              "tileset": (512, 512), "item_icon": (512, 512), "mini_icon": (512, 512), "ui_sheet": (768, 512), "concept": (768, 448)}


def horde(method, path, body=None):
    req = urllib.request.Request(HORDE + path, data=json.dumps(body).encode() if body else None, headers=HORDE_HEADERS, method=method)
    return json.load(urllib.request.urlopen(req, timeout=120))


def horde_submit(model, entry):
    w, h = HORDE_SIZE.get(entry["type"], (512, 512))
    body = {"prompt": entry.get("prompt_short") or entry["prompt"][:900],
            "params": {"width": w, "height": h, "steps": 25, "n": 1, "cfg_scale": 7, "sampler_name": "k_euler_a"},
            "models": [model], "r2": True, "nsfw": False, "censor_nsfw": True}
    return horde("POST", "/generate/async", body)["id"]


def horde_fetch(job):
    c = horde("GET", f"/generate/check/{job}")
    if c.get("faulted"):
        raise RuntimeError("horde job faulted")
    if not c.get("done"):
        return None, c
    g = horde("GET", f"/generate/status/{job}").get("generations") or []
    if not g:
        raise RuntimeError("horde job finished without image")
    img = g[0]["img"]
    return (urllib.request.urlopen(urllib.request.Request(img, headers=HORDE_HEADERS), timeout=120).read()
            if img.startswith("http") else base64.b64decode(img)), c


# ---------------------------------------------------------------- common
def save_png(data, out):
    from PIL import Image
    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    Image.open(io.BytesIO(data)).convert("RGB").save(out)


def batch(args):
    manifest, rest = args[0], args[1:]
    opt = {"--provider": "hf", "--model": None, "--limit": None, "--parallel": "3"}
    force, prefixes, i = False, [], 0
    while i < len(rest):
        if rest[i] in opt: opt[rest[i]] = rest[i + 1]; i += 2
        elif rest[i] == "--force": force = True; i += 1
        else: prefixes.append(rest[i]); i += 1
    provider = opt["--provider"]
    model = opt["--model"] or {"horde": "AIO Pixel Art", "gemini": "gemini-3-pro-image", "hf": "FLUX (HF Spaces)"}[provider]
    entries = json.load(open(manifest, encoding="utf-8"))["entries"]
    todo = [e for e in entries if (not prefixes or any(e["id"].startswith(p) for p in prefixes))
            and (force or not os.path.exists(e["out"]))]
    if opt["--limit"]:
        todo = todo[:int(opt["--limit"])]
    print(f"{len(todo)} to generate via {provider} ({model})", flush=True)
    done = 0
    if provider == "hf":
        for e in todo:
            print(f"- {e['id']}", flush=True)
            try:
                save_png(hf_generate(e), e["out"]); done += 1
            except Exception as err:
                msg = err.read()[:200] if isinstance(err, urllib.error.HTTPError) else err
                print("  failed:", msg, flush=True)
                if "quota" in str(msg).lower():
                    print("ZeroGPU quota reached; stopping. Re-run later or set HF_TOKEN."); break
    elif provider == "gemini":
        for e in todo:
            for attempt in range(6):
                try:
                    print(f"- {e['id']}", flush=True)
                    save_png(gemini_generate(model, e["prompt"]), e["out"]); done += 1
                    break
                except urllib.error.HTTPError as err:
                    if err.code != 429:
                        print("  HTTP", err.code, err.read()[:200]); break
                    print(f"  429 quota; waiting {30 * 2 ** attempt}s", flush=True); time.sleep(30 * 2 ** attempt)
            else:
                print("quota still exhausted; stopping."); break
    else:
        queue, running, par = list(todo), {}, int(opt["--parallel"])
        while queue or running:
            while queue and len(running) < par:
                e = queue.pop(0)
                try:
                    running[horde_submit(model, e)] = e
                    print(f"- submitted {e['id']}", flush=True)
                except urllib.error.HTTPError as err:
                    print("  submit failed", err.code, err.read()[:200], flush=True); queue.insert(0, e); time.sleep(30); break
            time.sleep(15)
            for job, e in list(running.items()):
                try:
                    data, st = horde_fetch(job)
                except urllib.error.HTTPError as err:
                    if err.code == 404:  # Horde prunes jobs that waited too long: resubmit
                        print(f"  {e['id']}: job expired, resubmitting", flush=True); queue.append(e)
                    else:
                        print(f"  {e['id']}: HTTP {err.code}", flush=True)
                    del running[job]; continue
                except Exception as err:
                    print(f"  {e['id']}: {err}", flush=True); del running[job]; continue
                if data:
                    save_png(data, e["out"]); done += 1; del running[job]
                    print(f"  saved {e['out']}", flush=True)
                else:
                    print(f"  {e['id']}: queue {st.get('queue_position')} eta {st.get('wait_time')}s", flush=True)
    print(f"generated {done}/{len(todo)}")


if __name__ == "__main__":
    a = sys.argv[1:]
    if not a:
        print(__doc__); sys.exit(1)
    if a[0] == "gemini-list":
        d = json.load(urllib.request.urlopen(f"{GEMINI}?key={gemini_key()}&pageSize=200"))
        print([m["name"] for m in d["models"] if "image" in m["name"]])
    elif a[0] == "batch":
        batch(a[1:])
    else:
        print(__doc__); sys.exit(1)
