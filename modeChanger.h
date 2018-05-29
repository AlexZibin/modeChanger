#include "Timer.h" // https://github.com/AlexZibin/timer
#include "DualFunctionButton.h"

extern bool rotaryTurnLeft (); 
extern bool rotaryTurnRight (); 
extern DualFunctionButton button;

enum class LoopDir {FORWARD, BACK, FORWARD_AND_BACK, BACK_AND_FORWARD};
enum class returnValue {CONTINUE, NEXT, TERMINATE, ERROR, SHORTPRESS, LONGPRESS};
enum class LoopMode {ONCE, INFINITE};

typedef returnValue (*fPtr)(long); 

struct ControlStruct {
    fPtr *funcArray;
    int funcArrayLen;
    void (*endingFunction) (long);
    LoopMode loopMode;
    ControlStruct *nextPress;
    ControlStruct *nextLongPress;
    int startMode;
};

class ModeChanger {
    ControlStruct *controlStructPtr;
    int _currMode = 0; // -1 is an indication of an error (index out of range; -1 = array not initialized; -2 = function not found; etc);
    int _prevMode = -100;
    long currentCallNumber = 0;
    Timer timer;
    LoopDir direction;
  public:
    ModeChanger (ControlStruct *_controlStructPtr) {changeCtlArray (_controlStructPtr); }
    void changeCtlArray (ControlStruct *_controlStructPtr) { 
                                                             controlStructPtr = _controlStructPtr; 
                                                             applyMode (controlStructPtr->startMode);
                                                             _prevMode = -100;
                                                             //currentCallNumber = 0;
                                                             direction = LoopDir::FORWARD;
                                                             timer.switchOff ();
                                                           }
    int getCurrModeNumber (void) { return _currMode; }
    int err (void);
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
    
    //bool loopThruModeFunc (LoopDir direction, long numCycles=1); 
};
