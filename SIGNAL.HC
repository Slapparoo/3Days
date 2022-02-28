#if !IsWindows
extern U0 Backtrace(I64,I64); //PARSER.HC
U0 ArmSignals();
U0 SignalHandler(I64 sig) {
    Backtrace(0,0);
    ArmSignals;
    //if(!(tried_backtrace++))
    //    Backtrace(-1,0);
    //tried_backtrace=0;
    switch(sig) {
    case SIGILL:
    throw('SIGILL');
    case SIGABRT:
    throw('SIGABRT');
    case SIGBUS:
    throw('SIGBUS');
    case SIGFPE:
    throw('SIGFPE');
    case SIGKILL:
    throw('SIGKILL');
    case SIGSEGV:
    throw('SIGSEGV');
    case SIGTERM:
    throw('SIGTERM');
    case SIGSTOP:
    throw('SIGSTOP');
    default:
    throw('Signal?');
    }
}
U0 ArmSignals() {
    UnblockSignals;
    signal(SIGILL,&SignalHandler);
    signal(SIGABRT,&SignalHandler);
    signal(SIGBUS,&SignalHandler);
    signal(SIGFPE,&SignalHandler);
    signal(SIGKILL,&SignalHandler);
    signal(SIGSEGV,&SignalHandler);
    signal(SIGTERM,&SignalHandler);
    signal(SIGSTOP,&SignalHandler);
}
ArmSignals;
#else
U0 ArmSignals() {}
#endif
