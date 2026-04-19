# openocd_fixup.tcl — Board-independent OpenOCD event handler fix
# =========================================================================
# PROBLEM (why this file exists)
#
#   Zephyr board configs set "gdb_breakpoint_override hard" inside the
#   gdb-attach event.  This forces EVERY breakpoint to consume a hardware
#   FPB comparator (STM32F407 has 6; nRF52840 has 8; ESP32 has 2/core).
#
#   Cortex-Debug opens TWO GDB connections per session:
#     1. Main debug GDB
#     2. CDLiveWatch GDB (Live Watch variables panel)
#
#   Both trigger gdb-attach.  The second gdb-attach also runs "reset halt",
#   resetting the CPU just as the first GDB session is running to main().
#   Then both connections set hard breakpoints, exhausting all FPB slots:
#     "Can not find free FPB Comparator"
#
# FIX
#   1. Track GDB connection count.  Only halt on the FIRST connection.
#      The LiveWatch second connection attaches silently — no CPU reset.
#
#   2. Use "gdb_breakpoint_override soft":
#        Breakpoints in flash → OpenOCD patches the flash instruction directly
#                               (no FPB comparator consumed — unlimited)
#        Breakpoints in RAM   → OpenOCD patches the RAM instruction
#      All 6 FPB comparators remain free for data watchpoints.
#
#   3. Only resume on the LAST disconnect.  If LiveWatch disconnects first,
#      the main debug GDB is still active — do NOT resume the CPU.
#
# BOARD-INDEPENDENCE
#   "foreach t [target names]" iterates whatever targets the board config
#   created.  Works for STM32 (1 target), nRF52840 (1 target), ESP32
#   (2 targets: esp32.cpu0 / esp32.cpu1), RISC-V, etc. — no changes needed.
#
# USAGE — add as the SECOND configFiles entry in every launch.json config:
#   "configFiles": [
#       "<path to board openocd.cfg>",          ← board-specific
#       "${workspaceFolder}/.vscode/openocd_fixup.tcl"   ← never changes
#   ]
# =========================================================================

# Per-target connection counter (global, initialised when this script loads)
set __dbg_conn 0

foreach t [target names] {
    $t configure -event gdb-attach {
        global __dbg_conn
        incr __dbg_conn
        if {$__dbg_conn == 1} {
            echo "Debugger attaching: halting target (first connection)"
            halt
            gdb_breakpoint_override soft
        } else {
            echo "Debugger attaching: secondary connection #$__dbg_conn (LiveWatch)"
        }
    }

    $t configure -event gdb-detach {
        global __dbg_conn
        incr __dbg_conn -1
        if {$__dbg_conn <= 0} {
            set __dbg_conn 0
            echo "All debuggers detached: resuming target"
            resume
        } else {
            echo "Debugger detached: $__dbg_conn connection(s) still active"
        }
    }
}

