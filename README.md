# LinuxProcessShell
A process management shell for creating, stopping, and viewing running processes.

----------------------------------------------------------

### Compiling PMan.c

To compile PMan.c, place the attached Makefile 
in the same directory as PMan.c, process.c, and process.h.

Type "make" in the command line, this will compile PMan.c 
with process.c and process.h files.

----------------------------------------------------------
### Running PMan.c

To run PMan.c, type "./PMan" in the command line.

The user will get prompted for input.
Below is a listing of each command that PMan.c will run.

----------------------------------------------------------

COMMAND:	bg

USAGE:		bg args

FUNCTION:	Starts a new child background process based on the list of arguments provided.

EXAMPLE:	bg ./inf * 1000

----------------------------------------------------------

COMMAND:	bglist

USAGE:		bglist

FUNCTION:	Lists all currently running and stopped background processes that were started usingm the bg command in PMan.

EXAMPLE:	bglist

----------------------------------------------------------

COMMAND:	bgkill

USAGE:		bgkill pid

FUNCTION:	Terminates the background process with the specified pid. If the pid does not exist, then it will not terminate the process.

EXAMPLE:	bgkill 1234

----------------------------------------------------------

COMMAND:	bgstop

USAGE:		bgstop pid

FUNCTION:	Stops the background process with the specified pid. If the pid does not exist, then it will not stop the process.

EXAMPLE:	bgstop 1234

----------------------------------------------------------

COMMAND:	bgstart

USAGE:		bgstart pid

FUNCTION:	Restarts a stopped background process with the specified pid. If the pid does not exist, then it will not restart a process.

EXAMPLE:	bgstart 1234

----------------------------------------------------------

COMMAND:	pstat

USAGE:		pstat pid

FUNCTION:	Lists the following information related to the specified pid. An example is shown below:

			comm:                       Example Output
			state:                      Example Output					
			utime:                      Example Output
			stime:                      Example Output
			rss:                        Example Output
			voluntary_ctxt_switches:    Example Output
			nonvoluntary_ctxt_switches: Example Output

EXAMPLE:	pstat 1234

----------------------------------------------------------
