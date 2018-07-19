#include "modeChanger.h"

/*bool ModeChanger::loopThruModeFunc (int nSec, int numCycles, LoopDir direction) {
    // Each function in _funcArray is called for nSec seconds or until it returns a flag
    static int _numCycles;
      
    if (!timer.isOn ()) {
        _numCycles = numCycles;
        timer.setInterval ("ms", nSec*1000);
        timer.switchOn ();
        applyMode (0);      
        
        switch (direction) {
          case LoopDir::BACK:
          case LoopDir::BACK_AND_FORWARD:
              prevMode (); // Starting from last function and looping to the first one
        };
    } else if (timer.needToTrigger ()) {
        _currentCallNumber = 0;
        switch (direction) {
          case LoopDir::FORWARD:
          case LoopDir::FORWARD_AND_BACK: // FORWARD_AND_BACK now is a stub; will be developed later
                Serial.println ("Switching forward");
                nextMode ();
                if (getCurrModeNumber () == 0) { // We've travelled the whole loop of functions!
                    if (--_numCycles == 0) {
                        timer.switchOff ();
                        return true; 
                    }
                }
                break;
          case LoopDir::BACK:
          case LoopDir::BACK_AND_FORWARD: // BACK_AND_FORWARD now is a stub; will be developed later
              prevMode ();
              if (getCurrModeNumber () == _numModes-1) { // We've travelled the whole loop of functions!
                  if (--_numCycles == 0) {
                      timer.switchOff ();
                      return true; 
                  }
              }
              break;
        }
    } 
    switch (callCurrModeFunc (_currentCallNumber++)) { 
        case returnValue::NEXT:            // routine asks to forward-change mode
            timer.prepareToTrigger ();
            break;
        case returnValue::TERMINATE:       // routine asks to terminate the loop of functions
            currentCallNumber = 0;
            timer.switchOff ();
            return true; 
    }
    return false;
}*/

returnValue ModeChanger::callCurrModeFunc (long param) {
    if (!err ()) { // Negative stands for some error
        
        if (controlStructPtr->secondsForEachMode > 0) {
            if (!timer.isOn ()) {
                timer.setInterval ("ms", controlStructPtr->secondsForEachMode*1000UL);
                timer.switchOn ();
            } else {
                if (timer.needToTrigger()) {
                    timer.switchOff ();
                    return returnValue::NEXT;
                }
            }
        }
        
        returnValue retVal = (*(controlStructPtr->funcArray)[_currMode]) (param);
        if (controlStructPtr->endingFunction != nullptr) { (*(controlStructPtr->endingFunction)) (param); }
        
        if (retVal != returnValue::CONTINUE)
            timer.switchOff ();
        
        return retVal;
    }
    return returnValue::ERROR; // error
}

//////////////////////////////////////////////////////////////////////////////
// moves to next function only when current function returns returnValue::NEXT
// returns true to signal to terminate the loop
bool ModeChanger::loopThruModeFunc (void) {
    // Current function in controlStructPtr->funcArray is called until it returns a flag

    switch (callCurrModeFunc (currentCallNumber++)) { 
        case returnValue::NEXT:            // routine asks to change to next mode
			if (direction == LoopDir::FORWARD) {
				nextMode ();
                if (controlStructPtr->loopMode == LoopMode::ONCE) {
                    if (_currMode == 0) {
                        changeCtlArray (controlStructPtr->nextPress);
                    }
                }
			}
			else {
                if (controlStructPtr->loopMode == LoopMode::ONCE) {
                    if (_currMode == 0) {
                        changeCtlArray (controlStructPtr->nextPress);
                    }
                    else {
                        prevMode ();
                    }
                } else {
                    prevMode ();
                }
			}
            break;
        case returnValue::SHORTPRESS:
        case returnValue::TERMINATE:
            //Serial.println("\n loopThruModeFunc/SHORTPRESS or TERMINATE");
            changeCtlArray (controlStructPtr->nextPress);
            return true; 
            break;
        case returnValue::LONGPRESS:
            //Serial.println("\n loopThruModeFunc/LONGPRESS ");
            changeCtlArray (controlStructPtr->nextLongPress);
            return true;
            break;
    }


//    if (controlStructPtr->endingFunction != nullptr) (*(controlStructPtr->endingFunction)) (0);
    
	/*static int counter;
	if (counter++ > 200) {
		counter = 0;
		Serial.println ("loopThruModeFunc is working!");
	}*/
	
    if (controlStructPtr->funcArrayLen> 1) {
		if (rotaryTurnLeft ()) {
			//Serial.println ("loopThruModeFunc/rotaryTurnLeft is working!");
			direction = LoopDir::BACK;
			prevMode ();
		}    
		if (rotaryTurnRight ()) {
			direction = LoopDir::FORWARD;
			nextMode ();
		}
	}
    if (button.shortPress()) {
        changeCtlArray (controlStructPtr->nextPress);
    }
    if (button.longPress ()) {
        changeCtlArray (controlStructPtr->nextLongPress);
    }

    return false;
}

// stub, not implemented yet:
//bool ModeChanger::loopThruModeFunc (LoopDir direction, long numCycles) {return false;}
//bool ModeChanger::loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero) {

bool ModeChanger::modeJustChanged (void) {
    if (_prevMode != _currMode) {
        _prevMode = _currMode;
        return true;
    }
    return false;
}

int ModeChanger::nextMode (void) {
    if (!err ()) { // Negative stands for some error
        if (++_currMode >= controlStructPtr->funcArrayLen) { // Close the circle
            _currMode = 0;
        }
    } else _currMode = 0;
    currentCallNumber = 0;
    /*Serial.print ("\nSwitched to mode ");
    Serial.println (_currMode);
*/
    timer.switchOff (); // This forces the timer to restart at each manual mode change when using (controlStructPtr->secondsForEachMode > 0)
    
    return _currMode; 
}

int ModeChanger::prevMode (void) {
    if (!err ()) { // Negative stands for some error
        if (--_currMode == -1) { // Close the circle backwards
            _currMode = controlStructPtr->funcArrayLen - 1;
        }
    } else _currMode = 0;
    currentCallNumber = 0;

    timer.switchOff (); // This forces the timer to restart at each manual mode change when using (controlStructPtr->secondsForEachMode > 0)
    
    return _currMode; 
}

int ModeChanger::applyMode (int newMode) {
  
    if ((newMode >=0) && (newMode < controlStructPtr->funcArrayLen)) {
        _currMode = newMode;
    } else {
        _currMode = -10; // out of range error;
    }
    currentCallNumber = 0;

    timer.switchOff (); // This forces the timer to restart at each manual mode change when using (controlStructPtr->secondsForEachMode > 0)
    
    return _currMode; 
}

int ModeChanger::applyMode (fPtr newModeFunc) {
    _currMode = -2; // negative value is an indication of an error

    for (int i = 0; i < controlStructPtr->funcArrayLen; i++) {
       // compare two pointers
       //Serial.print("_funcArray[i]: ");
       if (newModeFunc == controlStructPtr->funcArray[i]) {
            _currMode = i;
        }
    }
	currentCallNumber = 0;
    if (_currMode < 0) { // newModeFunc not found in the list of our registered functions! This is not allowed!
        Serial.println (F("\n\nERROR! MODE NOT FOUND!\n\n"));
        delay (1000);
    }

    timer.switchOff (); // This forces the timer to restart at each manual mode change when using (controlStructPtr->secondsForEachMode > 0)
    
    return _currMode; 
}

int ModeChanger::err (void) {
    if ((_currMode < 0) || (_currMode >= controlStructPtr->funcArrayLen)) {
        //Serial.print (F("\n\n ModeChanger::err ERROR! _currMode = ")); Serial.println (_currMode);
        return _currMode; 
    }
    return 0; 
}
