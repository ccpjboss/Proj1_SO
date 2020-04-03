/**
 * ! AGENT
 * * 1->The agent program creates a "agent.pid" file and writes its PID to it.
 * * 2->The agent then read the contents of a file nammed "text.in" nad prints it.
 * ! Sensitive to SIGHUP and SIGTERM
 * * SIGHUP -> reads the contents of "text.in".
 * * SIGTERM -> Process terminates... and exit.
 * 
 * ! CONTROLLER
 * TODO: 1-> Checks for runnig agent by fetching the PID from "agent.pid".
 * TODO: 2-> If it cannot find agent --> prints("Error: no agent found.") and exits otherwise prints("Agent found.")
 * TODO: 3-> Signal menu: 
 *                 * Choose a signal to send [1: HUP; 15: TERM]:
 * ! SIGTERM also terminates the controller
 * ! If the user presses Crtl+C the controller should send a SIGTERM to agent and exits
 */