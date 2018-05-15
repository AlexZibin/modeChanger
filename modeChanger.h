#include "Timer.h" // https://github.com/AlexZibin/timer

enum class LoopDir {FORWARD, BACK, FORWARD_AND_BACK, BACK_AND_FORWARD};
//enum class returnValue {CONTINUE, FORWARD, BACK};
//typedef returnValue (*fPtr)(long); 

typedef int (*fPtr)(long); 

class ModeChanger {
    fPtr *_funcArray;
    int _numModes;
    int _currMode = 0; // -1 is an indication of an error (index out of range; -1 = array not initialized; -2 = function not found; etc);
    int _prevMode = -100;
	long _currentCallNumber = 0;
    Timer timer;
  public:
    ModeChanger (fPtr *funcArray, int numModes) : _funcArray(funcArray), _numModes(numModes) {}
    void testOp ();
    int getCurrModeNumber (void) { return _currMode; };
    int nextMode (void);
    int prevMode (void);
    int applyMode (int newMode);
    int applyMode (fPtr newModeFunc);
    int callCurrModeFunc (long param);
    bool modeJustChanged (void);
    bool loopThruModeFunc (int nSec=10, int numCycles=1, LoopDir direction = LoopDir::FORWARD);
    //bool loopThruModeFunc (int nSec=10, int numCycles=1, LoopDir direction = LoopDir::FORWARD, int startMode = 0);
    //bool loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero=false) {
    
    // moves to next function only when current function returns zero:
    bool loopThruModeFunc (LoopDir direction = LoopDir::FORWARD, long numCycles=1); 
};

