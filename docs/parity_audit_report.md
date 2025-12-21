# Parity Audit Report

## Executive Summary

This audit compares the current source state against the original `gladiation.dll` HLIL reference as documented in `docs/be_ai_parity_matrix.md`. The goal is to quantify the current parity level and identify critical tasks to close the gap.

## Parity Statistics

Based on the detailed analysis of the parity matrix:

| Module    | Implemented | Divergent | Missing | Total | Parity % |
|-----------|-------------|-----------|---------|-------|----------|
| Goal      | 12          | 0         | 9       | 21    | 57.1%    |
| Weight    | 4           | 1         | 5       | 10    | 40.0%    |
| Move      | 5           | 0         | 7       | 12    | 41.7%    |
| Weapon    | 4           | 0         | 1       | 5     | 80.0%    |
| Character | 3           | 0         | 1       | 4     | 75.0%    |
| **Total** | **28**      | **1**     | **23**  | **52**| **53.8%**|

*Note: "Divergent" items are considered not fully implemented for the purpose of strict parity calculation.*

**Overall Parity: 53.8%**

## Remediation Tasks

To accurately and elegantly narrow the parity gap, the following 10 tasks have been identified. These are prioritized based on gameplay impact, focusing first on movement perception and goal/item lifecycle, which are critical for bot agency.

### High Priority: Movement Perception
1.  **Implement `BotMovementViewTarget`**
    *   **Goal:** Restore lookahead target logic.
    *   **Reasoning:** Essential for smooth navigation and realistic aiming while moving. Without it, bots lack predictive looking.

2.  **Implement `BotPredictVisiblePosition`**
    *   **Goal:** Enable opponent position prediction.
    *   **Reasoning:** Critical for combat effectiveness. Bots need to know where an enemy *will be* to intercept or evade effectively.

3.  **Implement `BotReachabilityArea`**
    *   **Goal:** Provide accurate area probing.
    *   **Reasoning:** Fundamental for AAS (Area Awareness System) interaction. Ensures bots correctly identify navigable areas vs. solid geometry.

4.  **Implement `BotResetLastAvoidReach`**
    *   **Goal:** Properly track and reset reachability avoidance.
    *   **Reasoning:** Prevents bots from getting stuck in loops avoiding valid paths due to stale data.

5.  **Implement `BotAddAvoidSpot`**
    *   **Goal:** Allow dynamic addition of avoidance spots.
    *   **Reasoning:** Needed for reacting to dynamic dangers (grenades, temporary hazards).

### High Priority: Goal & Item Lifecycle
6.  **Implement `BotInitLevelItems`**
    *   **Goal:** Parse map entities into internal item tables.
    *   **Reasoning:** The foundation of the goal system. Without this, bots are blind to the level's resources (weapons, health).

7.  **Implement `BotUpdateEntityItems`**
    *   **Goal:** Handle dynamic updates for dropped/respawning items.
    *   **Reasoning:** Ensures bots react to the changing state of the world (e.g., picking up a dropped weapon).

8.  **Implement `BotGetLevelItemGoal`**
    *   **Goal:** Enable classname-indexed search for items.
    *   **Reasoning:** Allows efficient querying of specific item types, essential for "need-based" goal selection.

### Medium Priority: Vision & Camp Support
9.  **Implement `BotItemGoalInVisButNotVisible`**
    *   **Goal:** Refine visibility checks (AAS vs. Trace).
    *   **Reasoning:** Resolves edge cases where an item is theoretically visible by area but blocked by geometry, preventing false positives in goal selection.

10. **Implement `BotGetNextCampSpotGoal`**
    *   **Goal:** Enable strategic camping.
    *   **Reasoning:** Adds tactical depth by allowing bots to utilize designated camp spots defined in the map.
