# Botlib Parity Audit Plan

## Baseline audit
- Parity tests are built by default when `BUILD_TESTING=ON`, and remaining `cmocka_skip()` markers in `test_bot_interface.c` signal unimplemented scenarios that need assertions once the HLIL behaviour is confirmed.【F:docs/parity_testing_guide.md†L26-L47】
- Several parity fixtures skip when required sample assets are missing, including mover coverage that depends on `dev_tools/assets/maps/test_mover.{bsp,aas}` and bot interface setup helpers that halt if the asset environment cannot be staged.【F:tests/README.md†L95-L113】【F:tests/parity/test_bot_interface.c†L546-L670】
- AI weight regression tests also skip when reference weight scripts are unavailable, indicating asset availability is a recurring prerequisite across parity checks.【F:tests/ai/test_ai_weight.c†L18-L36】
- The headless Quake II parity harness remains opt-in and requires external game assets and environment variables before it can validate runtime parity against a dedicated server.【F:docs/testing/headless_quake2_parity_check.md†L1-L105】

## Plan and status
The items below are ordered; "Work on the plan" means executing the next unchecked task.

1. [x] Publish the baseline audit and execution plan (this document).
2. [ ] Harden parity harness prerequisites so asset-dependent skips become actionable failures.
   - Document minimal asset pack and environment setup in `tests/README.md` with exact paths for mover (`dev_tools/assets/maps/test_mover.{bsp,aas}`), lexer samples, weight scripts, and Quake II assets.
   - Add a `dev_tools/scripts/verify_parity_assets.sh` helper that checks presence/permissions for all required assets and exits non-zero with remediation hints.
   - Wire the verifier into CTest (pre-test step) and CI jobs so missing assets surface as failed setup rather than silent skips.
3. [ ] Replace remaining `cmocka_skip()` placeholders in `tests/parity/test_bot_interface.c` with assertions that mirror the HLIL contract and `tests/parity/README.md` scenarios.
   - Catalogue each skip with the intended behaviour, data fixture, and any HLIL reference needed; track progress in an in-file checklist comment.
   - Implement deterministic fixtures that do not rely on ad hoc runtime availability (e.g., staged movers/weights) and assert expected exports/state transitions.
   - Remove `cmocka_skip()` usage except for cases explicitly blocked by documented upstream gaps; ensure such gaps are cross-referenced in this plan.
4. [ ] Expand subsystem parity coverage using expectations from `tests/parity/README.md` and `docs/parity_testing_guide.md`.
   - Weight config guards: add tests for malformed weights, missing parameters, and boundary handling against `tests/reference/botlib_contract.json`.
   - Movement state exports: extend mover parity to cover crouch/ladder/water states and navigation flags, reusing staged `test_mover` assets.
   - Weapon state exports: add parity assertions for ammo counts, cooldowns, and weapon switching, ensuring fixtures seed predictable inventories.
   - Chat exports: validate chat event propagation and filters; include locale/encoding edge cases if supported by HLIL.
5. [ ] Keep `tests/reference/botlib_contract.json` and supporting documentation in sync with new HLIL findings before enabling stricter assertions.
   - Version the contract updates alongside test changes; require updates in PR checklist when parity behaviours change.
   - Capture rationale for each contract delta directly in the JSON (via comments where allowed) or in an adjacent changelog entry.
   - Add a CI check to diff the contract against expected schemas to prevent accidental drift.
6. [ ] Exercise the headless Quake II parity harness with staged assets, triage divergences, and integrate the run into routine validation once stable.
   - Script a repeatable harness invocation (env setup + command) under `dev_tools/scripts/run_headless_parity.sh` with documented parameters.
   - Record observed deltas between HLIL and dedicated server runs, filing follow-up tasks here with owners and target releases.
   - Gate merging of major botlib changes on a green headless parity run once flakiness is resolved.

## Maintenance notes
- Update this plan as tasks complete or new parity gaps are discovered, keeping the ordered list accurate so follow-on work can proceed from the top.
- Track asset dependencies alongside each task to minimise future skips and keep parity runs reproducible across environments.
