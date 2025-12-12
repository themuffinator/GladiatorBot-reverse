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

### Expected slot order and signatures inferred from HLIL

| Slot | Observed use | Inferred prototype |
| --- | --- | --- |
| 0 | Client data feed | `void (*BotInput)(int client, bot_input_t *bi);` |
| 1 | Console command emission | `void (*BotClientCommand)(int client, char *str, ...);` |
| 2 | Logging | `void (*Print)(int type, const char *fmt, ...);` |
| 3 | BSP trace | `bsp_trace_t (*Trace)(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);` |
| 4 | Point contents | `int (*PointContents)(vec3_t point);` |
| 5 | Allocator | `void *(*GetMemory)(int size);` |
| 6 | Free | `void (*FreeMemory)(void *ptr);` |
| 7 | Handle generator | `int (*DebugLineCreate)(void);` |

## Quake II botlib import surface

The Quake II `bot_import_t` declares sixteen callbacks that the engine should expose to the botlib:

| Order | Callback | Signature |
| --- | --- | --- |
| 0 | BotInput | `void (*BotInput)(int client, bot_input_t *bi);` |
| 1 | BotClientCommand | `void (*BotClientCommand)(int client, char *str, ...);` |
| 2 | Print | `void (*Print)(int type, char *fmt, ...);` |
| 3 | Trace | `bsp_trace_t (*Trace)(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);` |
| 4 | PointContents | `int (*PointContents)(vec3_t point);` |
| 5 | GetMemory | `void *(*GetMemory)(int size);` |
| 6 | FreeMemory | `void (*FreeMemory)(void *ptr);` |
| 7 | DebugLineCreate | `int (*DebugLineCreate)(void);` |
| 8 | DebugLineDelete | `void (*DebugLineDelete)(int line);` |
| 9 | DebugLineShow | `void (*DebugLineShow)(int line, vec3_t start, vec3_t end, int color);` |
| 10 | AddCommand | `void (*AddCommand)(const char *name, void (*function)(void));` |
| 11 | RemoveCommand | `void (*RemoveCommand)(const char *name);` |
| 12 | CmdArgc | `int (*CmdArgc)(void);` |
| 13 | CmdArgv | `const char *(*CmdArgv)(int index);` |
| 14 | CvarGet | `cvar_t *(*CvarGet)(const char *name, const char *default_value, int flags);` |
| 15 | Error | `void (*Error)(const char *fmt, ...);` |

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
