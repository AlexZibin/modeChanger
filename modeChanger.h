#include "Timer.h" // https://github.com/AlexZibin/timer

enum class LoopDir {FORWARD, BACK, FORWARD_AND_BACK, BACK_AND_FORWARD};
enum class returnValue {CONTINUE, NEXT, TERMINATE, ERROR};
typedef returnValue (*fPtr)(long); 

class ModeChanger {
    fPtr *_funcArray;
    fPtr _endingFunction;
    int _numModes;
    int _currMode = 0; // -1 is an indication of an error (index out of range; -1 = array not initialized; -2 = function not found; etc);
    int _prevMode = -100;
    long _currentCallNumber = 0;
    Timer timer;
  public:
    //ModeChanger (fPtr *funcArray, int numModes) : _funcArray(funcArray), _numModes(numModes), _endingFunction (nullptr) {}
    //ModeChanger (fPtr *funcArray, int numModes, fPtr endingFunction = nullptr) : _funcArray(funcArray), _numModes(numModes) {setEndingFunction (endingFunction); }
    ModeChanger (fPtr *funcArray, int numModes, fPtr endingFunction = nullptr) {changeCtlArray (funcArray, numModes, endingFunction); }
    void changeCtlArray (fPtr *funcArray, int numModes, fPtr endingFunction = nullptr) : _funcArray(funcArray), _numModes(numModes) {setEndingFunction (endingFunction); }
    void setEndingFunction (fPtr ptr) : _endingFunction (ptr) {}
    int getCurrModeNumber (void) { return _currMode; }
    int nextMode (void);
    int prevMode (void);
    int applyMode (int newMode);
    int applyMode (fPtr newModeFunc);
    returnValue callCurrModeFunc (long param);
    bool modeJustChanged (void);

    // moves to next function only when current function returns returnValue::FORWARD
    // returns true if all functions are executed of if current function returns returnValue::TERMINATE
    bool loopThruModeFunc (int nSec=10, int numCycles=1, LoopDir direction = LoopDir::FORWARD);
    
    //bool loopThruModeFunc (int nSec=10, int numCycles=1, LoopDir direction = LoopDir::FORWARD, int startMode = 0);
    //bool loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero=false) {
    
    bool loopThruModeFunc (LoopDir direction = LoopDir::FORWARD, long numCycles=1); 
};

