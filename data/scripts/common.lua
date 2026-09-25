-- Shared helpers and test-only events (never called from game flow).
-- Engine API: see src/script.cpp (say, ask, battle, flag, set_flag, give, take, count, money,
-- warp, rest, shop, scene, toast, clock, phase, tile, searched, mark_searched, daily_roll, face, face_player).

function __test_error() undefined_function_for_test() end
function __test_ask() set_flag("t_ans", ask("", "테스트", {"가", "나"})) end
