# trap.S
# Assembly-level trap handler.
.section .text
.global m_mode_trap_vector
m_mode_trap_vector:
    # We get here when the CPU is interrupted
	# for any reason.
    mret