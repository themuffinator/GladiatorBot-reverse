# Botlib import callback comparison

## Callbacks expected by the original Gladiator DLL

The disassembly of `gladiator.dll` shows that the bot library copies a 0x28-byte import block and treats it as ten function pointers. During `GetBotAPI`, the DLL performs:

- A `memcpy` of 0x28 bytes from the engine-provided table into `data_10063fe0`.
- Subsequent code calls the imported slots as follows:
  - `data_10063fe0(arg1, s)` – first slot, used to feed a client-specific structure into the botlib.
  - `data_10063fe4(...)` – second slot, invoked when issuing console commands (e.g., `EA_Command`).
  - `data_10063fe8(type, fmt, ...)` – third slot, used for formatted logging across the DLL.
  - `data_10063fec(&trace, start, mins, maxs, end, passent, mask)` – fourth slot, returns a 0x54-byte trace result.
  - `data_10063ff0(point)` – fifth slot, queried with a single vector argument (point contents check).
  - `data_10063ff4(ptr)` and `data_10063ff8(ptr)` – sixth and seventh slots, used to allocate and free heap blocks.
  - `data_10063ffc()` – eighth slot, repeatedly used to obtain integer handles.

These uses align with the Quake II `bot_import_t` expectations (BotInput, BotClientCommand, Print, Trace, PointContents, GetMemory, FreeMemory, DebugLineCreate/Draw primitives), though the DLL only appears to populate ten entries rather than the full sixteen defined by the Quake headers.

## Quake II botlib import surface

The Quake II `bot_import_t` declares sixteen callbacks that the engine should expose to the botlib:

1. `void (*BotInput)(int client, bot_input_t *bi);`
2. `void (*BotClientCommand)(int client, char *str, ...);`
3. `void (*Print)(int type, char *fmt, ...);`
4. `cvar_t *(*CvarGet)(const char *name, const char *default_value, int flags);`
5. `void (*Error)(const char *fmt, ...);`
6. `bsp_trace_t (*Trace)(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);`
7. `int (*PointContents)(vec3_t point);`
8. `void *(*GetMemory)(int size);`
9. `void (*FreeMemory)(void *ptr);`
10. `int (*DebugLineCreate)(void);`
11. `void (*DebugLineDelete)(int line);`
12. `void (*DebugLineShow)(int line, vec3_t start, vec3_t end, int color);`
13. `void (*AddCommand)(const char *name, void (*function)(void));`
14. `void (*RemoveCommand)(const char *name);`
15. `int (*CmdArgc)(void);`
16. `const char *(*CmdArgv)(int index);`

## Current `botlib_import_table_t`

The in-repo mirror only exposes eight callbacks:

- `Print(int type, const char *fmt, ...)`
- `DPrint(const char *fmt, ...)`
- `BotLibVarGet(const char *var_name, char *value, size_t size)`
- `BotLibVarSet(const char *var_name, const char *value)`
- `AddCommand(const char *name, void (*function)(void))`
- `RemoveCommand(const char *name)`
- `CmdArgc(void)`
- `CmdArgv(int index)`

`DPrint` and the libvar helpers do not appear in the Quake II import contract and therefore represent Gladiator-specific additions, while the majority of the Quake-defined callbacks (input submission, command emission, cvar/error accessors, tracing, memory management, and debug line helpers) are currently missing from the struct.

## Gap summary

| Source | Present callbacks | Missing callbacks | Notes |
| --- | --- | --- | --- |
| Quake II `bot_import_t` (16) | AddCommand, RemoveCommand, CmdArgc, CmdArgv, Print | BotInput, BotClientCommand, CvarGet, Error, Trace, PointContents, GetMemory, FreeMemory, DebugLineCreate, DebugLineDelete, DebugLineShow | Full surface exceeds current table size. |
| Gladiator DLL HLIL (≈10 slots) | Logging (Print), command invocation slot, trace, point-contents probe, memory alloc/free, handle generator; plus an initial slot used for per-client input | No explicit CvarGet/Error/debug line hooks observed; table length suggests only ten imports consumed | Import order matches the early subset of Quake II’s table, reinforcing which entries should be restored. |
| Repo `botlib_import_table_t` (8) | Print, DPrint, BotLibVarGet/BotLibVarSet, AddCommand, RemoveCommand, CmdArgc, CmdArgv | All Quake II engine-facing hooks (BotInput, BotClientCommand, CvarGet, Error, Trace, PointContents, GetMemory, FreeMemory, DebugLineCreate/Delete/Show) | Structure shape and naming diverge from both the DLL and the Quake header. |

### Recommendations for alignment

- Expand `botlib_import_table_t` to include the missing Quake II callbacks so that the struct size matches the 0x28-byte block consumed by the original DLL and the full sixteen-entry contract defined in `bot_import_t`.
- Retain `DPrint`/libvar helpers as Gladiator-specific extensions but append them after the Quake-aligned fields to preserve ordering for engine compatibility.
