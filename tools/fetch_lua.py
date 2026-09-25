#!/usr/bin/env python3
"""Vendor Lua 5.4.7 sources (MIT license) into third_party/lua. Pinned by SHA-256."""
import hashlib, io, os, shutil, tarfile, urllib.request

URL = "https://www.lua.org/ftp/lua-5.4.7.tar.gz"
SHA256 = "9fbf5e28ef86c69858f6d3d34eccc32e911c1a28b4120ff3e84aaa70cfbf1e30"
data = urllib.request.urlopen(URL, timeout=120).read()
digest = hashlib.sha256(data).hexdigest()
print("sha256", digest)
if SHA256 and digest != SHA256:
    raise SystemExit("checksum mismatch: refusing to vendor")
dst = "third_party/lua"
shutil.rmtree(dst, ignore_errors=True)
os.makedirs(dst)
with tarfile.open(fileobj=io.BytesIO(data)) as tar:
    for m in tar.getmembers():
        name = m.name.split("/", 1)[-1]
        if name.startswith("src/") and name.endswith((".c", ".h")) and name not in ("src/lua.c", "src/luac.c"):
            open(os.path.join(dst, os.path.basename(name)), "wb").write(tar.extractfile(m).read())
        if name == "doc/readme.html":
            open(os.path.join(dst, "LICENSE.html"), "wb").write(tar.extractfile(m).read())
print("vendored", len(os.listdir(dst)), "files into", dst)
