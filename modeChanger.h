#include "Timer.h" // https://github.com/AlexZibin/timer

enum class LoopDir {FORWARD, BACK, FORWARD_AND_BACK, BACK_AND_FORWARD};
enum class returnValue {CONTINUE, NEXT, TERMINATE, ERROR};
enum class LoopMode {ONCE, INFINITE};

typedef returnValue (*fPtr)(long); 

struct ControlStruct {
    fPtr *funcArray;
    int funcArrayLen;
    void (*endingFunction) (long);
    LoopMode loopMode;
    ControlStruct *nextPress;
    ControlStruct *nextLongPress;
};

class ModeChanger {
    ControlStruct *controlStructPtr;
    int currMode = 0; // -1 is an indication of an error (index out of range; -1 = array not initialized; -2 = function not found; etc);
    int prevMode = -100;
    long currentCallNumber = 0;
    Timer timer;
  public:
    ModeChanger (ControlStruct *_controlStructPtr) {changeCtlArray (_controlStructPtr); }
    void changeCtlArray (ControlStruct *_controlStructPtr) { controlStructPtr = _controlStructPtr; 
                                                             currMode = 0;
                                                             prevMode = -100;
                                                             currentCallNumber = 0;
                                                             timer.switchOff ();
                                                           }
    //void setEndingFunction (fPtr ptr) : _endingFunction (ptr) {}
    int getCurrModeNumber (void) { return _currMode; }
    int nextMode (void);
    int prevMode (void);
    int applyMode (int newMode);
    int applyMode (fPtr newModeFunc);
    returnValue callCurrModeFunc (long param);
    bool modeJustChanged (void);

    // returns true if all functions are executed of if current function returns returnValue::TERMINATE
    bool loopThruModeFunc (int nSec, int numCycles=1, LoopDir direction = LoopDir::FORWARD);

    // moves to next function only when current function returns returnValue::NEXT
    bool loopThruModeFunc ();
    
    //bool loopThruModeFunc (int nSec=10, int numCycles=1, LoopDir direction = LoopDir::FORWARD, int startMode = 0);
    //bool loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero=false) {
    
    bool loopThruModeFunc (LoopDir direction = LoopDir::FORWARD, long numCycles=1); 
};
