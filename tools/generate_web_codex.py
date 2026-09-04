#!/usr/bin/env python3
"""
Web Codex & Visual Dashboard Generator for 108: Yin-Yang Chronicle (108: 음양견문록)
Generates an interactive, standalone HTML5/CSS3 visual dashboard for all 108 Yokai.
"""

import os
import json

def generate_html_dashboard(prompts_path, output_html_path):
    with open(prompts_path, "r", encoding="utf-8") as f:
        yokai_data = json.load(f)

    # Element color mappings
    elem_colors = {
        "Physical": "#A0A898",
        "Fire": "#C83828",
        "Water": "#2868B0",
        "Earth": "#B88020",
        "Light": "#F0C030",
        "Dark": "#502868"
    }

    elem_names_ko = {
        "Physical": "물리 (物理)",
        "Fire": "화기 (火氣)",
        "Water": "수기 (水氣)",
        "Earth": "토기 (土氣)",
        "Light": "양기 (陽氣)",
        "Dark": "음기 (陰氣)"
    }

    cards_json_str = json.dumps(yokai_data, ensure_ascii=False)

    html_content = f"""<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>108: 음양견문록 — 108 요괴 전승 및 AI 비주얼 도감</title>
<style>
  :root {{
    --bg-dark: #081820;
    --card-bg: #14242C;
    --border-gold: #D4AF37;
    --text-primary: #E0F8D0;
    --text-muted: #88C070;
    --accent-red: #C83828;
    --accent-yellow: #F0C030;
    --accent-blue: #2868B0;
  }}
  * {{ box-sizing: border-box; margin: 0; padding: 0; }}
  body {{
    background-color: var(--bg-dark);
    color: var(--text-primary);
    font-family: 'Malgun Gothic', -apple-system, sans-serif;
    padding: 24px;
    line-height: 1.5;
  }}
  header {{
    text-align: center;
    padding: 20px 0 30px 0;
    border-bottom: 2px solid var(--border-gold);
    margin-bottom: 24px;
  }}
  h1 {{
    font-size: 2.2rem;
    color: var(--accent-yellow);
    letter-spacing: 2px;
    margin-bottom: 8px;
    text-shadow: 2px 2px 4px #000;
  }}
  .subtitle {{
    color: var(--text-muted);
    font-size: 1rem;
  }}
  .stats-banner {{
    display: flex;
    justify-content: center;
    gap: 30px;
    margin-top: 16px;
    flex-wrap: wrap;
  }}
  .stat-badge {{
    background: #1C3038;
    border: 1px solid var(--border-gold);
    padding: 6px 14px;
    border-radius: 4px;
    font-size: 0.9rem;
  }}
  .controls {{
    display: flex;
    gap: 16px;
    justify-content: space-between;
    align-items: center;
    flex-wrap: wrap;
    margin-bottom: 24px;
    background: #101E24;
    padding: 16px;
    border-radius: 6px;
    border: 1px solid #284450;
  }}
  .search-box {{
    flex: 1;
    min-width: 250px;
  }}
  .search-box input {{
    width: 100%;
    padding: 10px 14px;
    background: #081820;
    border: 1px solid var(--text-muted);
    border-radius: 4px;
    color: #FFF;
    font-size: 1rem;
  }}
  .filters {{
    display: flex;
    gap: 8px;
    flex-wrap: wrap;
  }}
  .filter-btn {{
    padding: 6px 12px;
    background: #1C3038;
    border: 1px solid #346856;
    border-radius: 4px;
    color: var(--text-primary);
    cursor: pointer;
    font-size: 0.85rem;
    transition: all 0.2s;
  }}
  .filter-btn:hover, .filter-btn.active {{
    background: var(--border-gold);
    color: #081820;
    font-weight: bold;
  }}
  .grid {{
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
    gap: 20px;
  }}
  .yokai-card {{
    background: var(--card-bg);
    border: 1px solid #284450;
    border-radius: 6px;
    padding: 16px;
    display: flex;
    flex-direction: column;
    gap: 12px;
    box-shadow: 0 4px 12px rgba(0,0,0,0.5);
    transition: transform 0.2s, border-color 0.2s;
  }}
  .yokai-card:hover {{
    transform: translateY(-4px);
    border-color: var(--border-gold);
  }}
  .card-header {{
    display: flex;
    justify-content: space-between;
    align-items: center;
    border-bottom: 1px solid #284450;
    padding-bottom: 8px;
  }}
  .dex-num {{
    font-weight: bold;
    color: var(--accent-yellow);
    font-size: 1.1rem;
  }}
  .yokai-name {{
    font-size: 1.25rem;
    color: #FFF;
    font-weight: bold;
  }}
  .badges {{
    display: flex;
    gap: 6px;
  }}
  .badge {{
    font-size: 0.75rem;
    padding: 2px 8px;
    border-radius: 3px;
    font-weight: bold;
    text-transform: uppercase;
  }}
  .origin-tag {{
    font-size: 0.8rem;
    color: var(--text-muted);
    font-style: italic;
  }}
  .prompt-box {{
    background: #081820;
    border: 1px solid #1C3844;
    padding: 10px;
    border-radius: 4px;
    font-size: 0.82rem;
    color: #CADEC0;
    max-height: 90px;
    overflow-y: auto;
    white-space: pre-wrap;
    word-break: break-word;
  }}
  .btn-copy {{
    background: #244856;
    color: #FFF;
    border: none;
    padding: 6px 12px;
    border-radius: 4px;
    cursor: pointer;
    font-size: 0.8rem;
    align-self: flex-start;
    transition: background 0.2s;
  }}
  .btn-copy:hover {{
    background: var(--accent-yellow);
    color: #081820;
  }}
  .copied {{
    background: #289058 !important;
    color: #FFF !important;
  }}
  footer {{
    text-align: center;
    padding: 40px 0 20px 0;
    color: var(--text-muted);
    font-size: 0.85rem;
    border-top: 1px solid #284450;
    margin-top: 40px;
  }}
</style>
</head>
<body>

<header>
  <h1>108: 음양견문록 (108: Yin-Yang Chronicle)</h1>
  <div class="subtitle">조선시대 민속 판타지 턴제 수집 RPG — 108종 전승 요괴 및 AI 비주얼 도감 대시보드</div>
  <div class="stats-banner">
    <div class="stat-badge">총 요괴 수: <strong>108종</strong></div>
    <div class="stat-badge">한국 전승: <strong>70% (76종)</strong></div>
    <div class="stat-badge">해외 전승: <strong>30% (32종)</strong></div>
    <div class="stat-badge">용량 한계: <strong>1.44MB (현재 656 KB)</strong></div>
    <div class="stat-badge">화풍: <strong>단청 16색 오방색 픽셀아트</strong></div>
  </div>
</header>

<div class="controls">
  <div class="search-box">
    <input type="text" id="searchInput" placeholder="요괴명, 도감 번호, 전승 출처 검색...">
  </div>
  <div class="filters">
    <button class="filter-btn active" data-elem="ALL">전체 원소</button>
    <button class="filter-btn" data-elem="Physical">물리</button>
    <button class="filter-btn" data-elem="Fire">화기(火)</button>
    <button class="filter-btn" data-elem="Water">수기(水)</button>
    <button class="filter-btn" data-elem="Earth">토기(土)</button>
    <button class="filter-btn" data-elem="Light">양기(陽)</button>
    <button class="filter-btn" data-elem="Dark">음기(陰)</button>
  </div>
</div>

<div class="grid" id="yokaiGrid"></div>

<footer>
  <p>108: 음양견문록 (108: Yin-Yang Chronicle) &copy; 2026. Standalone C++17 Windows Retro Game Engine.</p>
</footer>

<script>
  const YOKAI_DATA = {cards_json_str};
  const elemColors = {json.dumps(elem_colors)};
  let currentFilter = "ALL";
  let searchQuery = "";

  function renderGrid() {{
    const grid = document.getElementById("yokaiGrid");
    grid.innerHTML = "";

    const filtered = YOKAI_DATA.filter(y => {{
      const matchElem = (currentFilter === "ALL" || y.element === currentFilter);
      const text = (y.name_ko + " " + y.name_en + " " + y.folklore_origin + " " + y.dex_number).toLowerCase();
      const matchSearch = text.includes(searchQuery.toLowerCase());
      return matchElem && matchSearch;
    }});

    filtered.forEach(y => {{
      const card = document.createElement("div");
      card.className = "yokai-card";
      const elemColor = elemColors[y.element] || "#FFF";

      card.innerHTML = `
        <div class="card-header">
          <div>
            <span class="dex-num">#${{String(y.dex_number).padStart(3, '0')}}</span>
            <span class="yokai-name">${{y.name_ko}}</span>
          </div>
          <div class="badges">
            <span class="badge" style="background:${{elemColor}}; color:#081820;">${{y.element}}</span>
            <span class="badge" style="background:#346856; color:#FFF;">${{y.grade}}등급</span>
          </div>
        </div>
        <div class="origin-tag">📖 ${{y.folklore_origin}} (${{y.name_en}})</div>
        <div style="font-size:0.8rem; color:#88C070;">[AI 생성 프롬프트 (PixelLab / RetroDiffusion)]:</div>
        <div class="prompt-box">${{y.pixel_lab_prompt}}</div>
        <button class="btn-copy" onclick="copyPrompt(this, '${{encodeURIComponent(y.pixel_lab_prompt)}}')">📋 프롬프트 복사</button>
      `;
      grid.appendChild(card);
    }});
  }}

  function copyPrompt(btn, encodedPrompt) {{
    const text = decodeURIComponent(encodedPrompt);
    navigator.clipboard.writeText(text).then(() => {{
      const orig = btn.innerText;
      btn.innerText = "✓ 복사 완료!";
      btn.classList.add("copied");
      setTimeout(() => {{
        btn.innerText = orig;
        btn.classList.remove("copied");
      }}, 1500);
    }});
  }}

  document.getElementById("searchInput").addEventListener("input", (e) => {{
    searchQuery = e.target.value;
    renderGrid();
  }});

  document.querySelectorAll(".filter-btn").forEach(btn => {{
    btn.addEventListener("click", () => {{
      document.querySelectorAll(".filter-btn").forEach(b => b.classList.remove("active"));
      btn.classList.add("active");
      currentFilter = btn.dataset.elem;
      renderGrid();
    }});
  }});

  renderGrid();
</script>
</body>
</html>
"""

    os.makedirs(os.path.dirname(output_html_path), exist_ok=True)
    with open(output_html_path, "w", encoding="utf-8") as f:
        f.write(html_content)

    print(f"[Done] Generated Interactive Web Codex Dashboard: '{output_html_path}'")

def main():
    prompts_path = "data/yokai_ai_prompts.json"
    out_html = "dist/codex_dashboard.html"
    generate_html_dashboard(prompts_path, out_html)

if __name__ == "__main__":
    main()
