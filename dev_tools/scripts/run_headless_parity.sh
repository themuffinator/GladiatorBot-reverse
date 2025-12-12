#!/usr/bin/env bash
set -euo pipefail

print_usage() {
cat <<'USAGE'
Usage: run_headless_parity.sh --basedir <path> --dedicated <path> [options]

Stages the rebuilt Gladiator module into a Quake II install tree, exports the
headless harness environment variables, and launches the CTest entry
`headless_quake2_parity` using the documented defaults.

Required arguments:
  --basedir <path>      Root directory that contains the baseq2 assets.
  --dedicated <path>    Path to the Quake II dedicated server binary (q2ded/quake2).

Optional arguments:
  --build-dir <path>    CMake build directory containing the gladiator target (default: ./build).
  --module <path>       Override the rebuilt Gladiator module path (defaults to auto-detection in the build directory).
  --capture-dir <path>  Directory to store harness logs/configs (default: <build>/headless-parity-captures).
  --mod-dir <path>      Game directory inside the Quake II install where the module should be staged (default: <basedir>/gladiator).
  --extra-args <args>   Additional arguments appended to the dedicated server command (space-delimited string).
  --ctest-args <args>   Extra arguments forwarded to ctest after the regex filter.
  -h, --help            Show this help text.
USAGE
}

require_arg() {
local name="$1"
local value="$2"
if [[ -z "$value" ]]; then
echo "Missing required argument: $name" >&2
print_usage
exit 1
fi
}

resolve_path() {
local path="$1"
python3 - <<PYCODE
from pathlib import Path
print(Path("${path}").expanduser().resolve())
PYCODE
}

auto_detect_module() {
local build_dir="$1"
local -a candidates=(
"${build_dir}/libgladiator.so"
"${build_dir}/libgladiator.dylib"
"${build_dir}/gladiator.dll"
"${build_dir}/Release/gladiator.dll"
"${build_dir}/RelWithDebInfo/gladiator.dll"
)
for candidate in "${candidates[@]}"; do
if [[ -f "$candidate" ]]; then
resolve_path "$candidate"
return 0
fi
done
local found
found=$(find "$build_dir" -maxdepth 3 -type f \( -name 'libgladiator.*' -o -name 'gladiator.dll' \) | head -n1 || true)
if [[ -n "$found" ]]; then
resolve_path "$found"
return 0
fi
echo "Failed to locate the rebuilt Gladiator module in ${build_dir}. Use --module to specify it explicitly." >&2
exit 1
}

basedir=""
dedicated=""
build_dir="$(pwd)/build"
module_path=""
capture_dir=""
mod_dir=""
extra_args=""
ctest_args=""

while [[ $# -gt 0 ]]; do
case "$1" in
--basedir)
basedir="$2"
shift 2
;;
--dedicated)
dedicated="$2"
shift 2
;;
--build-dir)
build_dir="$2"
shift 2
;;
--module)
module_path="$2"
shift 2
;;
--capture-dir)
capture_dir="$2"
shift 2
;;
--mod-dir)
mod_dir="$2"
shift 2
;;
--extra-args)
extra_args="$2"
shift 2
;;
--ctest-args)
ctest_args="$2"
shift 2
;;
-h|--help)
print_usage
exit 0
;;
*)
echo "Unknown argument: $1" >&2
print_usage
exit 1
;;
esac
done

require_arg "--basedir" "$basedir"
require_arg "--dedicated" "$dedicated"

basedir="$(resolve_path "$basedir")"
dedicated="$(resolve_path "$dedicated")"
build_dir="$(resolve_path "$build_dir")"

if [[ -z "$module_path" ]]; then
module_path="$(auto_detect_module "$build_dir")"
else
module_path="$(resolve_path "$module_path")"
fi

if [[ -z "$capture_dir" ]]; then
capture_dir="$(resolve_path "${build_dir}/headless-parity-captures")"
else
capture_dir="$(resolve_path "$capture_dir")"
fi

if [[ -z "$mod_dir" ]]; then
mod_dir="$(resolve_path "${basedir}/gladiator")"
else
mod_dir="$(resolve_path "$mod_dir")"
fi

if [[ ! -d "$basedir/baseq2" ]]; then
echo "Expected baseq2 assets under ${basedir}; see docs/testing/headless_quake2_parity_check.md" >&2
exit 1
fi

if [[ ! -x "$dedicated" ]]; then
echo "Dedicated server is missing or not executable: ${dedicated}" >&2
exit 1
fi

mkdir -p "$capture_dir"

export GLADIATOR_Q2_DEDICATED_SERVER="$dedicated"
export GLADIATOR_Q2_BASEDIR="$basedir"
export GLADIATOR_Q2_MODULE_PATH="$module_path"
export GLADIATOR_Q2_CAPTURE_DIR="$capture_dir"
export GLADIATOR_Q2_MOD_DIR="$mod_dir"

if [[ -n "$extra_args" ]]; then
export GLADIATOR_Q2_EXTRA_ARGS="$extra_args"
fi

echo "[headless-parity] GLADIATOR_Q2_DEDICATED_SERVER=${GLADIATOR_Q2_DEDICATED_SERVER}"
echo "[headless-parity] GLADIATOR_Q2_BASEDIR=${GLADIATOR_Q2_BASEDIR}"
echo "[headless-parity] GLADIATOR_Q2_MODULE_PATH=${GLADIATOR_Q2_MODULE_PATH}"
echo "[headless-parity] GLADIATOR_Q2_CAPTURE_DIR=${GLADIATOR_Q2_CAPTURE_DIR}"
echo "[headless-parity] GLADIATOR_Q2_MOD_DIR=${GLADIATOR_Q2_MOD_DIR}"

read -r -a ctest_extra <<<"${ctest_args:-}"
ctest --test-dir "$build_dir" --output-on-failure -R headless_quake2_parity "${ctest_extra[@]}"
