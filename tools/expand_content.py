#!/usr/bin/env python3
"""
tools/expand_content.py - 108: Yin-Yang Chronicle Full Content Expansion
Generates all 108 individual Yokai JSON data files, expanded skill JSON files,
additional artifact JSON files, and regional encounter tables.
Updates yokai_ai_prompts.json to complete all 108 entries with authentic folklore.
"""

import os
import json
from pathlib import Path

# Complete 108 Yokai Registry Definition
YOKAI_REGISTRY_108 = [
    (1, "yokai_001", "도깨비", "Dokkaebi", "I", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 삼국유사", 1, 100, 30, 14, 10, 12, ["skill_001", "skill_002", "skill_007", "skill_004"]),
    (2, "yokai_002", "구미호", "Nine-Tailed Fox", "II", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 삼국유사", 12, 115, 60, 20, 12, 22, ["skill_001", "skill_002", "skill_008", "skill_006"]),
    (3, "yokai_003", "불가사리", "Bulgasari", "II", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 고려야사", 15, 145, 45, 22, 26, 8, ["skill_001", "skill_010", "skill_002", "skill_007"]),
    (4, "yokai_004", "삼족오", "Three-Legged Crow", "III", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 고구려 고분벽화", 24, 150, 80, 26, 18, 26, ["skill_001", "skill_012", "skill_004", "skill_005"]),
    (5, "yokai_005", "이무기", "Imugi", "III", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 용설화", 26, 165, 75, 27, 22, 18, ["skill_001", "skill_003", "skill_009", "skill_004"]),
    (6, "yokai_006", "해태", "Haetae", "III", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 조선왕조실록", 28, 170, 70, 28, 24, 16, ["skill_001", "skill_002", "skill_008", "skill_011"]),
    (7, "yokai_007", "장산범", "Jangsan Tiger", "II", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 현대구전문학", 14, 120, 50, 24, 14, 24, ["skill_001", "skill_006", "skill_010", "skill_007"]),
    (8, "yokai_008", "두시니", "Dusini", "I", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 4, 90, 25, 13, 9, 11, ["skill_001", "skill_006", "skill_002", "skill_004"]),
    (9, "yokai_009", "광척", "Gwangcheok", "I", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 어우야담", 6, 95, 28, 12, 11, 14, ["skill_001", "skill_003", "skill_006", "skill_004"]),
    (10, "yokai_010", "거붕귀", "Giant Shell Ghost", "I", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 해양설화", 7, 110, 20, 11, 18, 6, ["skill_001", "skill_003", "skill_007", "skill_004"]),
    (11, "yokai_011", "천지왕", "Cheonjiwang", "V", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 제주신화", 48, 290, 140, 38, 30, 25, ["skill_001", "skill_012", "skill_011", "skill_005"]),
    (12, "yokai_012", "마고할미", "Mago Giantess", "IV", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 창세설화", 38, 240, 90, 32, 28, 14, ["skill_001", "skill_007", "skill_010", "skill_011"]),
    (13, "yokai_013", "강철이", "Gangcheol Dragon", "IV", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 용재총화", 36, 220, 85, 34, 24, 20, ["skill_001", "skill_002", "skill_008", "skill_007"]),
    (14, "yokai_014", "최가영", "Choe Spirit", "I", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야담집", 5, 88, 24, 12, 8, 13, ["skill_001", "skill_004", "skill_007", "skill_002"]),
    (15, "yokai_015", "예도기", "Spirit Jar Imp", "I", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민속신앙", 3, 82, 35, 10, 8, 10, ["skill_001", "skill_004", "skill_006", "skill_003"]),
    (16, "yokai_016", "물도깨비", "Water Goblin", "I", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 8, 98, 32, 14, 10, 12, ["skill_001", "skill_003", "skill_009", "skill_007"]),
    (17, "yokai_017", "술도깨비", "Wine Goblin", "I", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 9, 105, 30, 15, 12, 9, ["skill_001", "skill_007", "skill_002", "skill_004"]),
    (18, "yokai_018", "강시귀", "Jiangshi", "II", "Dark", "VERIFY_FOREIGN_FOLKLORE", "동양 전승", 16, 130, 40, 23, 20, 10, ["skill_001", "skill_006", "skill_010", "skill_004"]),
    (19, "yokai_019", "백구도래", "White Dog Spirit", "I", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 충견설화", 6, 86, 30, 12, 10, 16, ["skill_001", "skill_011", "skill_004", "skill_005"]),
    (20, "yokai_020", "홍도대", "Red Robed Wraith", "II", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야담", 13, 110, 55, 19, 13, 17, ["skill_001", "skill_002", "skill_008", "skill_006"]),
    (21, "yokai_021", "천록", "Cheonrok", "III", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 경복궁 석수", 25, 160, 70, 26, 25, 18, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (22, "yokai_022", "지네귀신", "Centipede Demon", "II", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 전설의 고향", 17, 125, 48, 22, 16, 19, ["skill_001", "skill_006", "skill_010", "skill_005"]),
    (23, "yokai_023", "묘백", "White Cat Spirit", "I", "Dark", "VERIFY_FOREIGN_FOLKLORE", "동양 전승", 7, 84, 38, 13, 9, 20, ["skill_001", "skill_006", "skill_010", "skill_004"]),
    (24, "yokai_024", "귀신새", "Ghost Bird", "I", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 8, 85, 40, 14, 8, 22, ["skill_001", "skill_006", "skill_003", "skill_004"]),
    (25, "yokai_025", "금덕", "Golden Toad", "II", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 두꺼비 보은설화", 14, 135, 45, 18, 22, 11, ["skill_001", "skill_010", "skill_011", "skill_004"]),
    (26, "yokai_026", "영노", "Yeongno", "III", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야류 설화", 27, 175, 55, 29, 21, 16, ["skill_001", "skill_007", "skill_010", "skill_006"]),
    (27, "yokai_027", "비파귀", "Lute Ghost", "I", "Light", "VERIFY_FOREIGN_FOLKLORE", "동양 악기설화", 5, 80, 50, 11, 9, 15, ["skill_001", "skill_004", "skill_011", "skill_005"]),
    (28, "yokai_028", "자강", "Jagang Hound", "II", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 설화", 15, 120, 42, 21, 15, 21, ["skill_001", "skill_007", "skill_010", "skill_004"]),
    (29, "yokai_029", "청룡", "Azure Dragon", "IV", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 사신도", 39, 230, 95, 33, 26, 24, ["skill_001", "skill_003", "skill_009", "skill_005"]),
    (30, "yokai_030", "백호", "White Tiger", "IV", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 사신도", 40, 225, 80, 36, 25, 26, ["skill_001", "skill_007", "skill_010", "skill_006"]),
    (31, "yokai_031", "주작", "Vermilion Bird", "IV", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 사신도", 39, 210, 110, 34, 22, 28, ["skill_001", "skill_002", "skill_008", "skill_012"]),
    (32, "yokai_032", "현무", "Black Tortoise", "IV", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 사신도", 40, 250, 85, 30, 34, 12, ["skill_001", "skill_003", "skill_009", "skill_011"]),
    (33, "yokai_033", "황룡", "Yellow Dragon", "V", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 신라 설화", 49, 300, 130, 37, 32, 22, ["skill_001", "skill_012", "skill_010", "skill_011"]),
    (34, "yokai_034", "달토끼", "Moon Hare", "II", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 전래동화", 11, 105, 60, 17, 13, 25, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (35, "yokai_035", "태호새", "Sun Falcon", "II", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 고대설화", 14, 110, 50, 21, 12, 24, ["skill_001", "skill_002", "skill_008", "skill_005"]),
    (36, "yokai_036", "어용", "Fish Dragon", "II", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민속", 16, 130, 50, 20, 18, 17, ["skill_001", "skill_003", "skill_009", "skill_004"]),
    (37, "yokai_037", "삼두구", "Three-Headed Dog", "III", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 저승설화", 23, 160, 60, 27, 20, 19, ["skill_001", "skill_006", "skill_010", "skill_007"]),
    (38, "yokai_038", "정업", "Monk Ghost", "I", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 설화", 9, 94, 45, 13, 11, 12, ["skill_001", "skill_004", "skill_011", "skill_002"]),
    (39, "yokai_039", "복신", "God of Fortune Imp", "I", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민속신앙", 6, 92, 35, 12, 14, 10, ["skill_001", "skill_010", "skill_004", "skill_007"]),
    (40, "yokai_040", "용왕", "Dragon King", "IV", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 심청전/별주부전", 37, 235, 100, 31, 27, 21, ["skill_001", "skill_003", "skill_009", "skill_012"]),
    (41, "yokai_041", "그슨대", "Geuseundae", "III", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 25, 175, 65, 29, 21, 14, ["skill_001", "skill_006", "skill_010", "skill_007"]),
    (42, "yokai_042", "두억시니", "Dueoksini", "IV", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 35, 220, 70, 35, 26, 18, ["skill_001", "skill_007", "skill_006", "skill_010"]),
    (43, "yokai_043", "어둑시니", "Eoduksini", "III", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 24, 165, 70, 27, 20, 16, ["skill_001", "skill_006", "skill_010", "skill_004"]),
    (44, "yokai_044", "창귀", "Changgwi", "II", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 박지원 호질", 18, 120, 52, 22, 14, 21, ["skill_001", "skill_006", "skill_010", "skill_007"]),
    (45, "yokai_045", "산군", "Sangun Mountain Lord", "IV", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 호랑이 설화", 38, 230, 75, 36, 25, 25, ["skill_001", "skill_007", "skill_006", "skill_010"]),
    (46, "yokai_046", "처녀귀신", "Virgin Ghost", "II", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 전설의 고향", 15, 110, 65, 21, 13, 20, ["skill_001", "skill_003", "skill_009", "skill_006"]),
    (47, "yokai_047", "몽달귀신", "Bachelor Ghost", "II", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야담", 14, 115, 50, 22, 15, 17, ["skill_001", "skill_002", "skill_008", "skill_007"]),
    (48, "yokai_048", "달걀귀신", "Egg Face Ghost", "II", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 16, 125, 55, 23, 16, 18, ["skill_001", "skill_006", "skill_004", "skill_010"]),
    (49, "yokai_049", "야광귀", "Shoe Stealer Imp", "I", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 세시풍속", 7, 85, 30, 13, 9, 18, ["skill_001", "skill_004", "skill_006", "skill_007"]),
    (50, "yokai_050", "묘두사", "Cat-Headed Serpent", "III", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 신증동국여지승람", 26, 170, 70, 28, 22, 21, ["skill_001", "skill_006", "skill_010", "skill_005"]),
    (51, "yokai_051", "비형랑", "Bihyeongrang", "IV", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 삼국유사", 36, 215, 95, 33, 24, 23, ["skill_001", "skill_007", "skill_006", "skill_011"]),
    (52, "yokai_052", "길달", "Gildal", "II", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 삼국유사", 17, 125, 45, 23, 16, 24, ["skill_001", "skill_007", "skill_010", "skill_002"]),
    (53, "yokai_053", "저승사자", "Grim Messenger", "IV", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 무속신앙", 38, 220, 100, 34, 25, 22, ["skill_001", "skill_006", "skill_011", "skill_004"]),
    (54, "yokai_054", "강림차사", "Gangrim Messenger", "IV", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 제주 차사본풀이", 39, 230, 95, 35, 27, 24, ["skill_001", "skill_006", "skill_007", "skill_011"]),
    (55, "yokai_055", "바리데기", "Princess Bari", "V", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 바리공주 설화", 46, 270, 135, 35, 28, 26, ["skill_001", "skill_011", "skill_012", "skill_009"]),
    (56, "yokai_056", "삼신할머니", "Birth Goddess", "IV", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 무속신앙", 37, 210, 110, 30, 26, 20, ["skill_001", "skill_011", "skill_012", "skill_004"]),
    (57, "yokai_057", "가망신", "Guiding Spirit", "II", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 무속신앙", 18, 130, 60, 21, 17, 18, ["skill_001", "skill_011", "skill_004", "skill_005"]),
    (58, "yokai_058", "성주신", "House Guardian", "III", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 가신신앙", 28, 180, 65, 27, 28, 14, ["skill_001", "skill_010", "skill_011", "skill_007"]),
    (59, "yokai_059", "조왕신", "Kitchen Fire Spirit", "II", "Fire", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 가신신앙", 16, 120, 60, 22, 16, 19, ["skill_001", "skill_002", "skill_008", "skill_011"]),
    (60, "yokai_060", "터주신", "Earth Ground Spirit", "II", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 가신신앙", 17, 135, 50, 20, 24, 12, ["skill_001", "skill_010", "skill_004", "skill_007"]),
    (61, "yokai_061", "업신", "Wealth Snake Spirit", "II", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 가신신앙", 19, 140, 55, 23, 21, 16, ["skill_001", "skill_010", "skill_006", "skill_011"]),
    (62, "yokai_062", "측간귀신", "Latrine Wraith", "I", "Dark", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민속신앙", 8, 92, 34, 14, 9, 13, ["skill_001", "skill_006", "skill_004", "skill_003"]),
    (63, "yokai_063", "문전신", "Gate Ward Spirit", "II", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 제주 문전본풀이", 17, 125, 55, 21, 20, 17, ["skill_001", "skill_011", "skill_004", "skill_007"]),
    (64, "yokai_064", "아기장수 우투리", "Winged Hero Uturi", "IV", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 35, 215, 80, 34, 25, 27, ["skill_001", "skill_007", "skill_012", "skill_011"]),
    (65, "yokai_065", "신립의 망령", "General Sin Rip Ghost", "III", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 임진록 야사", 29, 175, 68, 29, 23, 19, ["skill_001", "skill_003", "skill_007", "skill_006"]),
    (66, "yokai_066", "놋점귀", "Brass Imp", "I", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야철 설화", 9, 96, 28, 15, 13, 10, ["skill_001", "skill_007", "skill_010", "skill_002"]),
    (67, "yokai_067", "물귀신", "Drowning Ghost", "I", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 민담", 10, 95, 36, 14, 10, 16, ["skill_001", "skill_003", "skill_006", "skill_009"]),
    (68, "yokai_068", "신기원 요화", "Mystic Lotus Spirit", "II", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 야담", 16, 115, 65, 19, 14, 19, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (69, "yokai_069", "백택", "Baize Omniscient Beast", "V", "Light", "VERIFY_FOREIGN_FOLKLORE", "동양 산해경", 47, 280, 130, 36, 30, 24, ["skill_001", "skill_012", "skill_011", "skill_004"]),
    (70, "yokai_070", "기린", "Qilin Benevolent Beast", "IV", "Light", "VERIFY_FOREIGN_FOLKLORE", "동양 설화", 38, 225, 95, 32, 27, 24, ["skill_001", "skill_011", "skill_012", "skill_005"]),
    (71, "yokai_071", "봉황", "Fenghuang Phoenix", "IV", "Fire", "VERIFY_FOREIGN_FOLKLORE", "동양 고대설화", 39, 215, 115, 35, 23, 27, ["skill_001", "skill_002", "skill_008", "skill_012"]),
    (72, "yokai_072", "비차", "Flying Chariot Spirit", "II", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 임진왜란 비차비기", 15, 120, 48, 22, 17, 23, ["skill_001", "skill_007", "skill_005", "skill_010"]),
    (73, "yokai_073", "목우", "Wooden Ox Golem", "II", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 도술 설화", 18, 145, 40, 21, 25, 9, ["skill_001", "skill_010", "skill_007", "skill_004"]),
    (74, "yokai_074", "서낭신", "Village Cairn Spirit", "III", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 성황신앙", 27, 170, 70, 26, 26, 17, ["skill_001", "skill_010", "skill_011", "skill_004"]),
    (75, "yokai_075", "당산나무 신목", "Sacred Zelkova", "III", "Earth", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 당산신앙", 28, 185, 65, 25, 29, 12, ["skill_001", "skill_010", "skill_011", "skill_007"]),
    (76, "yokai_076", "장승 목신", "Jangseung Totem Deity", "II", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 장승신앙", 17, 135, 45, 24, 21, 14, ["skill_001", "skill_007", "skill_011", "skill_010"]),
    (77, "yokai_077", "동자귀", "Child Ghost", "I", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 무속설화", 6, 82, 38, 12, 9, 17, ["skill_001", "skill_004", "skill_011", "skill_006"]),
    (78, "yokai_078", "골매", "Crag Falcon Beast", "II", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 산악설화", 16, 118, 48, 23, 14, 25, ["skill_001", "skill_007", "skill_010", "skill_005"]),
    (79, "yokai_079", "신령 백호", "Sacred White Tiger", "IV", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 산신설화", 38, 230, 80, 36, 26, 25, ["skill_001", "skill_007", "skill_006", "skill_012"]),
    (80, "yokai_080", "왕구렁선인", "Giant Serpent Ascetic", "III", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 구렁이 설화", 27, 168, 72, 28, 22, 19, ["skill_001", "skill_003", "skill_009", "skill_011"]),
    (81, "yokai_081", "삼태성군", "Three Terraces Star Lord", "IV", "Light", "VERIFY_FOREIGN_FOLKLORE", "도교 성수신앙", 37, 215, 105, 33, 24, 23, ["skill_001", "skill_012", "skill_011", "skill_005"]),
    (82, "yokai_082", "벽력장군", "Thunderbolt General", "IV", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 무속 뇌신", 39, 220, 95, 35, 23, 26, ["skill_001", "skill_005", "skill_012", "skill_007"]),
    (83, "yokai_083", "풍백", "Wind Lord", "IV", "Physical", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 단군신화", 38, 215, 90, 34, 24, 28, ["skill_001", "skill_007", "skill_010", "skill_005"]),
    (84, "yokai_084", "우사", "Rain Master", "IV", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 단군신화", 38, 225, 95, 32, 25, 21, ["skill_001", "skill_003", "skill_009", "skill_011"]),
    (85, "yokai_085", "운사", "Cloud Master", "IV", "Light", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 단군신화", 38, 220, 100, 31, 26, 22, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (86, "yokai_086", "치우천왕", "Chiyou War God", "V", "Physical", "VERIFY_FOREIGN_FOLKLORE", "동양 고대전설", 49, 295, 110, 39, 32, 23, ["skill_001", "skill_007", "skill_010", "skill_012"]),
    (87, "yokai_087", "자미대제", "Polar Star Emperor", "V", "Light", "VERIFY_FOREIGN_FOLKLORE", "도교 천문신앙", 50, 300, 150, 38, 31, 26, ["skill_001", "skill_012", "skill_011", "skill_005"]),
    (88, "yokai_088", "흑룡", "Abyssal Black Dragon", "V", "Water", "VERIFY_TRADITIONAL_KOREAN_FOLKLORE", "한국 용설화", 48, 285, 120, 37, 29, 24, ["skill_001", "skill_003", "skill_009", "skill_006"]),
    (89, "yokai_089", "응룡", "Winged Rain Dragon", "V", "Water", "VERIFY_FOREIGN_FOLKLORE", "동양 산해경", 48, 280, 125, 36, 28, 26, ["skill_001", "skill_003", "skill_009", "skill_012"]),
    (90, "yokai_090", "가릉빈가", "Kalavinka Bird", "III", "Light", "VERIFY_FOREIGN_FOLKLORE", "불교 설화", 26, 155, 80, 24, 18, 25, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (91, "yokai_091", "긴나라", "Kinnara Singer", "III", "Light", "VERIFY_FOREIGN_FOLKLORE", "불교 팔부중", 27, 160, 75, 25, 19, 24, ["skill_001", "skill_011", "skill_004", "skill_005"]),
    (92, "yokai_092", "마후라가", "Mahoraga Great Serpent", "III", "Earth", "VERIFY_FOREIGN_FOLKLORE", "불교 팔부중", 28, 175, 65, 28, 24, 17, ["skill_001", "skill_010", "skill_006", "skill_007"]),
    (93, "yokai_093", "건달바", "Gandharva Spirit", "III", "Light", "VERIFY_FOREIGN_FOLKLORE", "불교 팔부중", 25, 150, 85, 23, 17, 26, ["skill_001", "skill_011", "skill_004", "skill_012"]),
    (94, "yokai_094", "야차", "Yaksha Swift Guardian", "III", "Dark", "VERIFY_FOREIGN_FOLKLORE", "불교 팔부중", 29, 170, 65, 30, 21, 23, ["skill_001", "skill_006", "skill_007", "skill_010"]),
    (95, "yokai_095", "아수라", "Asura Fighting Lord", "IV", "Dark", "VERIFY_FOREIGN_FOLKLORE", "불교 팔부중", 38, 225, 85, 36, 25, 22, ["skill_001", "skill_007", "skill_006", "skill_010"]),
    (96, "yokai_096", "나찰", "Rakshasa Wrath Demon", "III", "Dark", "VERIFY_FOREIGN_FOLKLORE", "동양 불교설화", 28, 165, 70, 29, 20, 21, ["skill_001", "skill_006", "skill_010", "skill_002"]),
    (97, "yokai_097", "다문천왕", "Vaisravana North King", "IV", "Light", "VERIFY_FOREIGN_FOLKLORE", "불교 사천왕", 39, 225, 100, 33, 27, 21, ["skill_001", "skill_011", "skill_012", "skill_004"]),
    (98, "yokai_098", "증장천왕", "Virudhaka South King", "IV", "Physical", "VERIFY_FOREIGN_FOLKLORE", "불교 사천왕", 39, 230, 90, 35, 26, 22, ["skill_001", "skill_007", "skill_011", "skill_010"]),
    (99, "yokai_099", "광목천왕", "Virupaksa West King", "IV", "Fire", "VERIFY_FOREIGN_FOLKLORE", "불교 사천왕", 39, 220, 105, 34, 25, 23, ["skill_001", "skill_002", "skill_008", "skill_011"]),
    (100, "yokai_100", "지국천왕", "Dhrtarastra East King", "IV", "Light", "VERIFY_FOREIGN_FOLKLORE", "불교 사천왕", 39, 225, 100, 33, 26, 24, ["skill_001", "skill_011", "skill_005", "skill_012"]),
    (101, "yokai_101", "염라대왕", "King Yama Fifth Court", "V", "Dark", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 48, 280, 130, 37, 29, 22, ["skill_001", "skill_006", "skill_011", "skill_004"]),
    (102, "yokai_102", "진광대왕", "King Qinguang First Court", "IV", "Dark", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 38, 220, 95, 33, 25, 20, ["skill_001", "skill_006", "skill_011", "skill_004"]),
    (103, "yokai_103", "초강대왕", "King Chujiang Second Court", "IV", "Water", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 38, 225, 95, 32, 26, 19, ["skill_001", "skill_003", "skill_009", "skill_006"]),
    (104, "yokai_104", "송제대왕", "King Songdi Third Court", "IV", "Earth", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 38, 230, 90, 33, 27, 18, ["skill_001", "skill_010", "skill_006", "skill_011"]),
    (105, "yokai_105", "오관대왕", "King Wuguan Fourth Court", "IV", "Physical", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 38, 225, 90, 34, 26, 21, ["skill_001", "skill_007", "skill_006", "skill_010"]),
    (106, "yokai_106", "변성대왕", "King Biancheng Sixth Court", "IV", "Fire", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 38, 220, 95, 34, 25, 20, ["skill_001", "skill_002", "skill_008", "skill_006"]),
    (107, "yokai_107", "태산대왕", "King Taishan Seventh Court", "IV", "Dark", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 39, 235, 100, 35, 28, 21, ["skill_001", "skill_006", "skill_011", "skill_010"]),
    (108, "yokai_108", "전륜대왕", "King Tenth Wheel Court", "V", "Light", "VERIFY_FOREIGN_FOLKLORE", "명부 시왕설화", 50, 290, 140, 38, 30, 25, ["skill_001", "skill_012", "skill_011", "skill_004"])
]

# Skills expansion (12 total skills)
SKILLS_DATA = [
    {"id": "skill_001", "name_ko": "기본 공격", "power": 20, "qi_cost": 0, "accuracy": 100, "status": "none", "seal_turns": 0},
    {"id": "skill_002", "name_ko": "잔불 부적", "power": 25, "qi_cost": 5, "accuracy": 95, "status": "burn", "seal_turns": 0},
    {"id": "skill_003", "name_ko": "서리 숨결", "power": 22, "qi_cost": 6, "accuracy": 90, "status": "freeze", "seal_turns": 0},
    {"id": "skill_004", "name_ko": "봉인 문양", "power": 15, "qi_cost": 8, "accuracy": 85, "status": "seal", "seal_turns": 2},
    {"id": "skill_005", "name_ko": "벼락 소환", "power": 28, "qi_cost": 8, "accuracy": 85, "status": "paralysis", "seal_turns": 0},
    {"id": "skill_006", "name_ko": "귀기 안광", "power": 18, "qi_cost": 7, "accuracy": 90, "status": "fear", "seal_turns": 0},
    {"id": "skill_007", "name_ko": "도깨비 방망이", "power": 35, "qi_cost": 10, "accuracy": 90, "status": "none", "seal_turns": 0},
    {"id": "skill_008", "name_ko": "여우불 폭풍", "power": 38, "qi_cost": 12, "accuracy": 90, "status": "burn", "seal_turns": 0},
    {"id": "skill_009", "name_ko": "서릿발 곡소리", "power": 32, "qi_cost": 11, "accuracy": 85, "status": "freeze", "seal_turns": 0},
    {"id": "skill_010", "name_ko": "암흑 발톱", "power": 30, "qi_cost": 9, "accuracy": 95, "status": "fear", "seal_turns": 0},
    {"id": "skill_011", "name_ko": "벽사 결계", "power": 20, "qi_cost": 10, "accuracy": 100, "status": "seal", "seal_turns": 3},
    {"id": "skill_012", "name_ko": "천벌 벽력", "power": 45, "qi_cost": 16, "accuracy": 85, "status": "paralysis", "seal_turns": 0}
]

# Artifacts expansion (3 total artifacts)
ARTIFACTS_DATA = [
    {
        "id": "artifact_001",
        "name_ko": "신목의 파편",
        "origin": "VERIFY_TRADITIONAL_KOREAN_FOLKLORE",
        "passive": "공격력 +3",
        "drawback": "전투당 HP 2 감소",
        "destroyable": True,
        "attack_bonus": 3,
        "hp_loss_per_battle": 2
    },
    {
        "id": "artifact_002",
        "name_ko": "도깨비 은패",
        "origin": "VERIFY_TRADITIONAL_KOREAN_FOLKLORE",
        "passive": "영력 소비 20% 감소",
        "drawback": "방어력 2 감소",
        "destroyable": True,
        "attack_bonus": 0,
        "hp_loss_per_battle": 0
    },
    {
        "id": "artifact_003",
        "name_ko": "해태의 방울",
        "origin": "VERIFY_TRADITIONAL_KOREAN_FOLKLORE",
        "passive": "화상 및 공포 저항",
        "drawback": "민첩성 2 감소",
        "destroyable": True,
        "attack_bonus": 0,
        "hp_loss_per_battle": 0
    }
]

# Encounter Tables
ENCOUNTERS_DATA = [
    (
        "encounter_village_edge.json",
        {
            "region_id": "village_edge",
            "encounter_step_interval": 12,
            "min_level": 1,
            "max_level": 5,
            "encounters": [
                {"yokai_id": "yokai_001", "weight": 60},
                {"yokai_id": "yokai_008", "weight": 25},
                {"yokai_id": "yokai_014", "weight": 15}
            ]
        }
    ),
    (
        "encounter_mountain_pass.json",
        {
            "region_id": "mountain_pass",
            "encounter_step_interval": 8,
            "min_level": 5,
            "max_level": 15,
            "encounters": [
                {"yokai_id": "yokai_001", "weight": 25},
                {"yokai_id": "yokai_002", "weight": 25},
                {"yokai_id": "yokai_007", "weight": 25},
                {"yokai_id": "yokai_022", "weight": 25}
            ]
        }
    ),
    (
        "encounter_temple_grounds.json",
        {
            "region_id": "temple_grounds",
            "encounter_step_interval": 10,
            "min_level": 15,
            "max_level": 25,
            "encounters": [
                {"yokai_id": "yokai_003", "weight": 35},
                {"yokai_id": "yokai_041", "weight": 35},
                {"yokai_id": "yokai_046", "weight": 30}
            ]
        }
    )
]

def run():
    root = Path(__file__).resolve().parent.parent / "data"
    
    # 1. Write skills
    for s in SKILLS_DATA:
        slug = s["id"]
        # Map IDs to filenames
        fname = f"{slug}_{s['name_ko'].replace(' ', '_')}.json"
        # For skill_001 to 004 keep standard names
        mapping = {
            "skill_001": "skill_001_basic_strike.json",
            "skill_002": "skill_002_ember.json",
            "skill_003": "skill_003_frost.json",
            "skill_004": "skill_004_seal.json",
            "skill_005": "skill_005_thunder.json",
            "skill_006": "skill_006_fear_gaze.json",
            "skill_007": "skill_007_club_slam.json",
            "skill_008": "skill_008_foxfire.json",
            "skill_009": "skill_009_chill_wail.json",
            "skill_010": "skill_010_dark_claw.json",
            "skill_011": "skill_011_holy_ward.json",
            "skill_012": "skill_012_heavenly_wrath.json"
        }
        target_path = root / mapping[slug]
        with open(target_path, "w", encoding="utf-8") as f:
            json.dump(s, f, ensure_ascii=False, indent=2)
        print(f"Wrote skill: {target_path.name}")

    # 2. Write artifacts
    art_mapping = {
        "artifact_001": "artifact_001_shrine_shard.json",
        "artifact_002": "artifact_002_dokkaebi_token.json",
        "artifact_003": "artifact_003_haetae_bell.json"
    }
    for a in ARTIFACTS_DATA:
        target_path = root / art_mapping[a["id"]]
        with open(target_path, "w", encoding="utf-8") as f:
            json.dump(a, f, ensure_ascii=False, indent=2)
        print(f"Wrote artifact: {target_path.name}")

    # 3. Write encounters
    for fname, enc_obj in ENCOUNTERS_DATA:
        target_path = root / fname
        with open(target_path, "w", encoding="utf-8") as f:
            json.dump(enc_obj, f, ensure_ascii=False, indent=2)
        print(f"Wrote encounter: {target_path.name}")

    # 4. Write all 108 Yokai individual JSON files
    for entry in YOKAI_REGISTRY_108:
        dex_num, y_id, name_ko, name_en, grade, elem, origin_cat, folk_src, lvl, hp, qi, atk, df, spd, skls = entry
        y_obj = {
            "id": y_id,
            "dex_number": dex_num,
            "name_ko": name_ko,
            "name_en": name_en,
            "origin": origin_cat,
            "folklore_source": folk_src,
            "grade": grade,
            "element": elem,
            "level": lvl,
            "stats": {
                "hp": hp,
                "qi": qi,
                "attack": atk,
                "defense": df,
                "speed": spd
            },
            "skills": skls,
            "trait": f"특성_{name_ko}",
            "status_resistance": {
                "burn": 0,
                "freeze": 0,
                "seal": 0,
                "paralysis": 0,
                "fear": 0
            }
        }
        # Format filename e.g. yokai_001_dokkaebi.json
        fname = f"{y_id}_{name_en.lower().replace(' ', '_').replace('-', '_')}.json"
        if dex_num == 1:
            fname = "yokai_001_dokkaebi.json" # keep existing name
        target_path = root / fname
        with open(target_path, "w", encoding="utf-8") as f:
            json.dump(y_obj, f, ensure_ascii=False, indent=2)

    print(f"Wrote {len(YOKAI_REGISTRY_108)} yokai files.")

    # 5. Update yokai_ai_prompts.json to complete all 108 entries
    prompts_path = root / "yokai_ai_prompts.json"
    existing_prompts = []
    if prompts_path.exists():
        with open(prompts_path, "r", encoding="utf-8") as f:
            existing_prompts = json.load(f)

    # Build map of existing prompts by dex_number
    p_map = {p.get("dex_number"): p for p in existing_prompts}

    updated_prompts = []
    for entry in YOKAI_REGISTRY_108:
        dex_num, y_id, name_ko, name_en, grade, elem, origin_cat, folk_src, lvl, hp, qi, atk, df, spd, skls = entry
        existing = p_map.get(dex_num)
        if existing and "#" not in existing.get("name_ko", "") and "전승요괴" not in existing.get("name_ko", ""):
            # Preserve existing crafted prompts
            existing["dex_number"] = dex_num
            existing["name_ko"] = name_ko
            existing["name_en"] = name_en
            existing["grade"] = grade
            existing["element"] = elem
            existing["folklore_origin"] = folk_src
            updated_prompts.append(existing)
        else:
            # Generate new crafted prompt pack
            pixel_prompt = f"pixel art, {name_ko} ({name_en}), {folk_src} folklore creature, 16-bit retro sprite, side view battle stance, clean outlines, sharp dithering, Joseon folklore fantasy style, solid black background"
            retro_prompt = f"pixelart, {name_en}, {name_ko}, Game Boy Advance color palette, crisp 48x48 pixel monster battler, dark background, no antialiasing"
            prompt_obj = {
                "id": f"YOK_{dex_num:03d}_{name_en.upper().replace(' ', '_').replace('-', '_')}",
                "dex_number": dex_num,
                "name_ko": name_ko,
                "name_en": name_en,
                "grade": grade,
                "element": elem,
                "folklore_origin": folk_src,
                "pixel_lab_prompt": pixel_prompt,
                "retro_diffusion_prompt": retro_prompt,
                "negative_prompt": "blurry, smooth gradients, anti-aliased edges, modern 3D render, high-res photograph, mixels, watermark, bad anatomy",
                "target_resolution": {
                    "battle": [48, 48],
                    "field": [16, 24]
                },
                "recommended_palette": "dancheong16"
            }
            updated_prompts.append(prompt_obj)

    with open(prompts_path, "w", encoding="utf-8") as f:
        json.dump(updated_prompts, f, ensure_ascii=False, indent=2)

    print(f"Updated yokai_ai_prompts.json with {len(updated_prompts)} completed entries.")

if __name__ == "__main__":
    run()
