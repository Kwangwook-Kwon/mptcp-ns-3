WITH_GDB=0
SUITE="mptcp-tcp"
#SUITE="mptcp-option"
#SUITE="mptcp-mapping"
NS_LOG="Socket:TcpSocketBase:MpTcpSocketBase:MpTcpSubflow:*=error|warn|prefix_node|prefix_func" 
NS_LOG="$NS_LOG:MpTcpSchedulerRoundRobin"
NS_LOG="$NS_LOG:TcpTestCase"
NS_LOG="$NS_LOG:TcpRxBuffer:TcpTxBuffer"
NS_LOG="$NS_LOG:MpTcpMapping"
NS_LOG="$NS_LOG:TcpL4Protocol"
#NS_LOG="$NS_LOG:TraceHelper:PointToPointHelper"
OUT="xp.txt"
#NS_LOG="$NS_LOG:MpTcpTestSuite=*|prefix_func:Socket=*"

export NS_LOG
printenv|grep NS_LOG

if [ $WITH_GDB -gt 0 ]; then
	#COMMAND=
	echo 'gdb'
	read -r  command <<-'EOF'
		./waf --run test-runner --command-template="gdb --args %s --suite=$SUITE"
		EOF
else
	echo 'Without gdb'
	# you can add --out to redirect output to afile instead of standard output
	#--verbose 
	read -r  command <<-EOF
		./waf --run "test-runner --suite=$SUITE  --out=$OUT"
		EOF

fi



eval $command

echo "Exported:\n$NS_LOG"
echo "Executed Command:\n$command"

