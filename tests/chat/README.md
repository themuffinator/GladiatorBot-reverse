# Chat regression tests

The `bot_chat_tests` executable validates critical `botlib` chat behaviors
without booting the full game. The suite is intentionally narrow so failures
immediately highlight regressions in the translator and parser code paths that
feed the chat system.

## Build and wiring

- `tests/chat/CMakeLists.txt` enables the target only when `BUILD_TESTING` is
  set. The executable is compiled as `bot_chat_tests` with the C11 feature set
  and inherits the global `${PROJECT_SOURCE_DIR}/src` include path so the tests
  can include headers directly from the botlib sources.
- The test harness statically links the concrete implementations for
  `ai_chat.c`, `l_precomp.c`, and `l_script.c` because their helpers are not
  exposed through a dedicated library yet. `test_bot_chat_stubs.c` replaces the
  engine-facing hooks (logging, memory allocation, and `bot_import_t` plumbing)
  with minimal doubles so the chat code can run in isolation.
- `BOT_ASSET_ROOT` is injected via `target_compile_definitions` and defaults to
  `${PROJECT_SOURCE_DIR}/dev_tools/assets`. The tests must be executed from a
  checkout that includes the Gladiator assets under that path. At a minimum the
  following files are required:
  - `dev_tools/assets/rchat.c` for reply-table coverage.
  - `dev_tools/assets/match.c` for join-context coverage.

## Test layout (`test_bot_chat.c`)

`main` runs a collection of focused assertions. Each helper allocates an
isolated `bot_chatstate_t`, loads the relevant chat file, and frees the state
when finished so resource leaks are easy to spot. The existing coverage includes:

| Test | Purpose |
| --- | --- |
| `test_reply_chat_death_context` | Confirms that the `"unit-test"` context resolves to the death reply table and records a template for type `1`. |
| `test_reply_chat_falls_back_to_reply_table` | Verifies that unmatched contexts fall back to the raw reply table (context `5`). |
| `test_enter_chat_enqueues_message` | Exercises the context queue and asserts the join message is emitted with type `2`. |
| `test_enter_chat_cooldown_blocks_repeated_messages` | Validates the cooldown tracking path by forcing the log line `"context 2 blocked by cooldown"`. |
| `test_reply_chat_logs_missing_contexts` | Uses `BotLib_TestResetLastMessage` / `BotLib_TestGetLastMessage` from the stubs to ensure missing contexts are surfaced via `BotLib_Print` and the console queue remains empty. |
| `test_synonym_lookup_contains_nearbyitem_entries` | Spot-checks that the synonym tables expose expected phrases. |
| `test_known_template_is_registered` | Asserts the raw reply templates are registered once the file loads. |
| `test_include_path_too_long_is_rejected` | Bypasses the chat layer and exercises the precompiler diagnostics for oversized `#include` fragments. |

A small helper, `drain_console`, clears any queued chat messages between steps so
subsequent checks read only the newly generated entries.

## Stub helpers (`test_bot_chat_stubs.c`)

The stub translation unit provides the bare minimum implementation surface area
expected by the linked botlib sources:

- `BotLib_Print`, `BotLib_LogWrite`, and `BotLib_Error` record messages to an
  in-memory buffer and mirror them to `stderr`. The getters allow tests to assert
  on the latest log entry without depending on stdout capture.
- Memory helpers (`GetMemory`, `GetClearedMemory`, `FreeMemory`) forward to the
  C runtime. The chat code allocates short-lived buffers via these hooks, so any
  future replacements must remain compatible with malloc-style semantics.
- Engine-facing shims such as `BotInterface_GetImportTable`, `LibVarValue`, and
  `BotLib_LocateAssetRoot` currently return sentinel values because the chat
  tests do not depend on them. If additional functionality is required, extend
  the stubs in this file so the rest of the suite can reuse the same doubles.

## Conventions for new tests

- Use `BotAllocChatState` / `BotFreeChatState` for each scenario. Sharing a
  single chat state across cases introduces order dependence and makes failures
  difficult to diagnose.
- Prefer direct `assert` checks for clarity. When logging is required, reset the
  stub recorder before invoking the code under test so your assertions are
  deterministic.
- Keep asset usage explicit: hardcode relative paths based on `BOT_ASSET_ROOT`
  and document any additional files required inside this README to simplify CI
  configuration.
- New helpers or fixtures should live in `test_bot_chat_stubs.c` when they are
  expected to be shared. That keeps `test_bot_chat.c` focused on behavior rather
  than plumbing.
