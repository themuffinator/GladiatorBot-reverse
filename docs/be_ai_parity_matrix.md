# be_ai Parity Matrix

This matrix tracks the Quake III / Gladiator `be_ai_*` exports against the in-repo `src/botlib/ai*` implementations to surface missing or divergent behaviour.

## Goal module
| Function | Retail reference | Gladiator status | Acceptance criteria |
| --- | --- | --- | --- |
| BotResetGoalState | `be_ai_goal.h` export set.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L52-L118】 | implemented | Reset stack/avoidance and last reachability in place; matches `bot_goal.c` handle guard path.【F:src/botlib/ai_goal/bot_goal.c†L141-L168】 |
| BotResetAvoidGoals | `be_ai_goal.h` export set.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L52-L118】 | implemented | Clears avoid-goal list and count for a valid state.【F:src/botlib/ai_goal/bot_goal.c†L300-L322】 |
| BotRemoveFromAvoidGoals | `be_ai_goal.h` export set.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L52-L118】 | implemented | Removes matching avoid goal and compacts entries.【F:src/botlib/ai_goal/bot_goal.c†L364-L384】 |
| BotPushGoal / BotPopGoal / BotEmptyGoalStack | Stack exports in retail header.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L58-L76】 | implemented | Push/pop preserve LIFO semantics with overflow trimming per `bot_goal.c`.【F:src/botlib/ai_goal/bot_goal.c†L450-L506】 |
| BotDumpAvoidGoals / BotDumpGoalStack | Diagnostics expected by exports.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L64-L68】 | implemented | Emit formatted state contents to BotLib logging for valid handles.【F:src/botlib/ai_goal/bot_goal.c†L1042-L1107】 |
| BotGoalName | Retail helper for debug output.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L68-L82】 | implemented | Resolves level item names or falls back to numeric identifier.【F:src/botlib/ai_goal/bot_goal.c†L1000-L1021】 |
| BotGetTopGoal / BotGetSecondGoal | Stack peek helpers from retail.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L70-L82】 | implemented | Return highest entries when available; handle guards match `bot_goal.c`.【F:src/botlib/ai_goal/bot_goal.c†L508-L551】 |
| BotChooseLTGItem / BotChooseNBGItem | Retail item selection exports.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L74-L83】 | implemented | Score items using travel time and weights, respecting avoid lists and respawn timers.【F:src/botlib/ai_goal/bot_goal.c†L760-L906】 |
| BotTouchingGoal | Retail proximity predicate.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L82-L94】 | implemented | Bounding-box containment check around goal origin.【F:src/botlib/ai_goal/bot_goal.c†L942-L972】 |
| BotAvoidGoalTime / BotSetAvoidGoalTime | Retail avoidance timers.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L93-L100】 | implemented | Lookup/update avoid goal expiry using global time cache.【F:src/botlib/ai_goal/bot_goal.c†L386-L415】 |
| BotLoadItemWeights / BotFreeItemWeights | Retail weight IO.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L106-L114】 | implemented | Resolve asset path, load weight config, and allocate indices or free allocations.【F:src/botlib/ai_goal/bot_goal.c†L220-L287】 |
| BotAllocGoalState / BotFreeGoalState | Retail lifecycle entry points.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L111-L118】 | implemented | Handle-indexed allocations with per-client metadata and cleanup guards.【F:src/botlib/ai_goal/bot_goal.c†L76-L134】 |
| BotItemGoalInVisButNotVisible | Vision parity hook.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L84-L92】【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_goal.c†L1637-L1667】 | missing | Implement AAS/trace visibility check mirroring retail: return true when AAS reports visibility yet the client trace fails (inputs viewer, eye origin, view angles, goal). |
| BotGetLevelItemGoal | Retail classname search.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L86-L92】 | missing | Provide classname-indexed search over level items with resumable index, filling `bot_goal_t` for found entries. |
| BotGetNextCampSpotGoal | Retail camp navigation.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L88-L94】 | missing | Iterate configured `info_camp` spots, returning goal data per retail ordering. |
| BotGetMapLocationGoal | Retail location lookup.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L89-L94】 | missing | Match map location names to goals (target_locations) with proper area resolution. |
| BotInitLevelItems | Retail level scan.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L97-L104】【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_goal.c†L537-L672】 | missing | Parse map entities into iteminfo/levelitem tables, set respawn timers, and seed avoid states; must honour game type filters and bounding boxes. |
| BotUpdateEntityItems | Retail dynamic updates.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L97-L104】【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_goal.c†L1009-L1175】 | missing | Refresh dropped/temporary item goals each frame, expiring timed-out entries and recomputing goal origins/areas. |
| BotInterbreedGoalFuzzyLogic / BotMutateGoalFuzzyLogic | Retail genetic utilities.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L101-L106】 | missing | Port fuzzy-logic combination and mutation to evolve weight trees given parent handles and mutation range. |
| BotSaveGoalFuzzyLogic | Retail persistence hook.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L103-L106】 | missing | Serialize current goal fuzzy logic to disk, respecting retail format and filenames. |
| BotSetupGoalAI / BotShutdownGoalAI | Retail init/teardown.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_goal.h†L115-L118】 | missing | Allocate goal caches and free them on shutdown; mirror retail return codes and idempotence. |

## Weight module
| Function | Retail reference | Gladiator status | Acceptance criteria |
| --- | --- | --- | --- |
| ReadWeightConfig / FreeWeightConfig | Weight config exports.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L63-L70】 | implemented | Parse weight scripts (with global defines) and free fuzzy separators when `bot_reloadcharacters` permits reloading.【F:src/botlib/ai_weight/ai_weight.c†L166-L274】【F:src/botlib/ai_weight/bot_weight.c†L31-L74】 |
| FuzzyWeight / FindFuzzyWeight | Retail evaluators.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L69-L74】 | implemented | Expose fuzzy weight lookup through handle table; return 0 or -1 on invalid handle/weight number.【F:src/botlib/ai_weight/bot_weight.c†L174-L252】【F:src/botlib/ai_weight/bot_weight.c†L344-L382】 |
| BotLoadWeights / BotWriteWeights | Retail IO equivalents to load/write weight configs.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L63-L74】 | implemented | Handle-based load/write with asset resolution and fatal logging on missing files; preserves cached configs.【F:src/botlib/ai_weight/bot_weight.c†L118-L213】【F:src/botlib/ai_weight/bot_weight.c†L264-L319】 |
| BotSetWeight / BotFuzzyWeightHandle | Retail runtime adjustors.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L69-L74】 | implemented | Allow runtime reassignment of fuzzy leaf values and evaluation over inventories via stored configs.【F:src/botlib/ai_weight/bot_weight.c†L322-L382】 |
| WriteWeightConfig | Standalone writer expected by retail.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L67-L74】 | divergent | Provide non-handle writer matching retail signature that serializes a supplied config; reuse handle-based helper to avoid duplication. |
| FuzzyWeightUndecided | Retail undecided branch evaluator.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L71-L74】 | missing | Implement balanced tie-breaking path used when no separator matches, mirroring retail recursion. |
| ScaleWeight / ScaleBalanceRange | Retail scaling utilities.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L74-L78】 | missing | Apply scalar to named weights or balance ranges within a config without corrupting tree structure. |
| EvolveWeightConfig | Retail evolution helper.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L78-L82】 | missing | Mutate config weights using retail probability ranges to support genetic algorithms. |
| InterbreedWeightConfigs | Retail interbreeding helper.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L79-L82】 | missing | Combine two weight configs into an output config using retail crossover strategy. |
| BotShutdownWeights | Retail cache cleanup.【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_weight.h†L81-L83】 | missing | Free cached configs and reset globals on shutdown to mirror retail memory lifecycle. |

## Move module
| Function | Retail reference | Gladiator status | Acceptance criteria |
| --- | --- | --- | --- |
| BotResetMoveState / BotInitMoveState | Retail lifecycle entry points.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L112-L134】 | implemented | Clear state, set presencetype and origin data, and honour or_moveflags; errors flagged on invalid handles.【F:src/botlib/ai_move/bot_move.c†L608-L706】 |
| BotMoveToGoal / BotMoveInDirection | Retail movement primitives.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L115-L118】 | implemented | Drive AAS reachability and direction-based motion with moveresult flags and grapple support.【F:src/botlib/ai_move/bot_move.c†L754-L1151】 |
| BotResetAvoidReach | Retail avoid reach reset.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L118-L122】 | implemented | Clears avoid reach arrays and times on the move state.【F:src/botlib/ai_move/bot_move.c†L707-L727】 |
| BotAllocMoveState / BotFreeMoveState | Retail allocation APIs.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L128-L134】 | implemented | Handle-indexed allocations with fatal logging on exhaustion or misuse.【F:src/botlib/ai_move/bot_move.c†L94-L189】 |
| BotTravel_Grapple | Retail travel variant (inline in be_ai_move).【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L112-L134】 | implemented | Implements grapple reachability including hook precache, view alignment, and slack release.【F:src/botlib/ai_move/bot_move.c†L24-L203】 |
| BotResetLastAvoidReach | Retail reachability reset helper.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L118-L123】 | missing | Track last avoided reachability separately from general avoid list and clear timers per retail logic. |
| BotReachabilityArea | Retail area probe.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L122-L125】 | missing | Return area index for an origin/client pair via AAS, matching retail solid-area handling. |
| BotMovementViewTarget | Retail lookahead target helper.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L124-L126】【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_move.c†L843-L891】 | missing | Compute predicted view target for movement with lookahead distance, populating target vector and success flag. |
| BotPredictVisiblePosition | Retail prediction helper.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L125-L127】【F:dev_tools/Quake-III-Arena-master/code/botlib/be_ai_move.c†L912-L968】 | missing | Predict opponent position visible from a goal using travel flags, returning best-visible point and area success. |
| BotAddAvoidSpot | Retail avoid-spot adder.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L134-L137】 | missing | Maintain avoidspot array with radius/type semantics and clear on AVOID_CLEAR requests. |
| BotSetBrushModelTypes | Retail map-init hook.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L135-L138】 | missing | Cache brush model types for movers (plats/bobs) each map load to support reachability relinking. |
| BotSetupMoveAI / BotShutdownMoveAI | Retail init/teardown.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_move.h†L138-L141】 | missing | Initialise movement globals (model types, AAS callbacks) and free them safely on shutdown. |

## Weapon module
| Function | Retail reference | Gladiator status | Acceptance criteria |
| --- | --- | --- | --- |
| BotAllocWeaponState / BotFreeWeaponState / BotResetWeaponState | Retail lifecycle exports.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_weap.h†L89-L104】 | implemented | Handle-indexed weapon states clear cached weights/config and log on invalid handles.【F:src/botlib/ai_weapon/bot_weapon.c†L16-L120】 |
| BotLoadWeaponWeights | Retail load hook.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_weap.h†L95-L100】 | implemented | Loads weight + config pair, attaches to state, and reports `BLERR_CANNOTLOADWEAPONWEIGHTS` on failure.【F:src/botlib/ai_weapon/bot_weapon.c†L122-L182】 |
| BotChooseBestFightWeapon / BotGetTopRankedWeapon | Retail selection helpers.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_weap.h†L93-L99】 | implemented | Evaluate fuzzy weights over inventory and cache last best weapon/weight for subsequent queries.【F:src/botlib/ai_weapon/bot_weapon.c†L184-L245】 |
| BotGetWeaponInfo | Retail info fetch.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_weap.h†L94-L99】 | implemented | Copies weapon info for indexed weapon, guarding bounds and null configs.【F:src/botlib/ai_weapon/bot_weapon.c†L247-L280】 |
| BotSetupWeaponAI / BotShutdownWeaponAI | Retail init/teardown.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_weap.h†L89-L92】 | missing | Wire weapon parsing caches and free globals on shutdown; ensure configs and projectiles preloaded as in retail. |

## Character module
| Function | Retail reference | Gladiator status | Acceptance criteria |
| --- | --- | --- | --- |
| BotLoadCharacter / BotLoadCharacterSkill | Retail character loaders.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_char.h†L33-L48】 | implemented | Case-insensitive handle caching keyed by filename+skill with refcounts and asset load logging.【F:src/botlib/ai_character/bot_character.c†L59-L151】 |
| BotFreeCharacter / BotFreeCharacterStrings | Retail free paths.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_char.h†L33-L47】 | implemented | Decrement refcount, free profile when zero, or free provided profile strings for transient callers.【F:src/botlib/ai_character/bot_character.c†L153-L210】 |
| Characteristic_Float / BFloat / Integer / BInteger / String | Retail characteristic accessors.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_char.h†L37-L47】 | implemented | Route through loaded profile with clamping/bounds and safe buffer handling for strings.【F:src/botlib/ai_character/bot_character.c†L220-L305】 |
| BotShutdownCharacters | Retail cache shutdown.【F:dev_tools/Quake-III-Arena-master/code/game/be_ai_char.h†L33-L48】 | missing | Release cached character handles and reset table on shutdown to align with retail lifecycle. |

## Remediation schedule (prioritised by gameplay impact)
- [ ] High: restore movement perception exports (`BotMovementViewTarget`, `BotPredictVisiblePosition`, `BotReachabilityArea`, `BotResetLastAvoidReach`) to unblock combat navigation and aiming.
- [ ] High: reintroduce goal/item lifecycle (`BotInitLevelItems`, `BotUpdateEntityItems`, `BotGetLevelItemGoal`, `BotAddAvoidSpot`) so pickups and dropped items affect decision making.
- [ ] Medium: visibility and camp support (`BotItemGoalInVisButNotVisible`, `BotGetNextCampSpotGoal`, `BotGetMapLocationGoal`, `BotSaveGoalFuzzyLogic`) to complete positional awareness.
- [ ] Medium: weapon/weight lifecycle gaps (`BotSetupWeaponAI`, `BotShutdownWeaponAI`, `BotShutdownWeights`, `WriteWeightConfig`, `FuzzyWeightUndecided`, `ScaleWeight`, `ScaleBalanceRange`, `BotSetupMoveAI`, `BotShutdownMoveAI`) for consistent load/unload parity.
- [ ] Low: genetic utilities (`BotInterbreedGoalFuzzyLogic`, `BotMutateGoalFuzzyLogic`, `EvolveWeightConfig`, `InterbreedWeightConfigs`, `BotSetBrushModelTypes`, `BotSetupGoalAI`, `BotShutdownGoalAI`, `BotShutdownCharacters`) to finish parity tooling.

