# 검증 명령

```powershell
python tools\check_project.py
python tools\validate_data.py
powershell -ExecutionPolicy Bypass -File tools\build_release.ps1
ctest --test-dir build_nmake_release2
```

첫 번째 명령은 기본 프로젝트 골격을, 두 번째 명령은 콘텐츠 JSON을 검사한다. C++ 실행 검증은 Windows C++17 컴파일러와 CMake 설치 후 수행한다.
