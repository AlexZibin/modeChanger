#include "modeChanger.h"

bool ModeChanger::loopThruModeFunc (int nSec, int numCycles, LoopDir direction) {
//bool ModeChanger::loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero) {
    // Each function in _funcArray is called for nSec seconds
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
    if (callCurrModeFunc (_currentCallNumber++)) { // ���� ������� ���������� ��-����, ������ - ������������� �� ��������� �������	
		timer.prepareToTrigger ();
	}
    return false;
}

// moves to next function only when current function returns zero:
bool ModeChanger::loopThruModeFunc (LoopDir direction, long numCycles) {return false;}
//bool ModeChanger::loopThruModeFunc (int nSec, int numCycles, LoopDir direction, bool switchAtZero) {

void ModeChanger::testOp () {
  (*_funcArray[0])(1);
  (*_funcArray[1])(1); 
  (*_funcArray[2])(1); 
}

bool ModeChanger::modeJustChanged (void) {
    if (_prevMode != _currMode) {
        _prevMode = _currMode;
        return true;
    }
    return false;
}

int ModeChanger::nextMode (void) {
    if (_currMode > -1) { // Negative stands for some error
        if (++_currMode == _numModes) { // Close the circle
            _currMode = 0;
        }
    }    
	_currentCallNumber = 0;
    Serial.print ("\nTrying to switch to mode ");
    Serial.println (_currMode);

    return _currMode; 
}

int ModeChanger::prevMode (void) {
    if (_currMode > -1) { // Negative stands for some error
        if (--_currMode == -1) { // Close the circle backwards
            _currMode = _numModes-1;
        }
    }    
    _currentCallNumber = 0;
	return _currMode; 
}

int ModeChanger::applyMode (int newMode) {
  
    if ((newMode >=0) && (newMode < _numModes)) {
        _currMode = newMode;
    } else {
        _currMode = -10; // out of range error;
    }
    _currentCallNumber = 0;
    return _currMode; 
}

int ModeChanger::applyMode (fPtr newModeFunc) {
    _currMode = -2; // negative value is an indication of an error

    for (int i = 0; i < _numModes; i++) {
       // compare two pointers
       //Serial.print("_funcArray[i]: ");
       //Serial.println((unsigned long)_funcArray[i]);
       if (newModeFunc == _funcArray[i]) {
            _currMode = i;
        }
    }
	_currentCallNumber = 0;
    if (_currMode < 0) { // newModeFunc not found in the list of our registered functions! This is not allowed!
        Serial.println ("\n\nERROR! MODE NOT FOUND!\n\n");
        delay (1000);
    }
    return _currMode; 
}

int ModeChanger::callCurrModeFunc (long param) {
    if (_currMode > -1) { // Negative stands for some error
        return (*_funcArray[_currMode]) (param);
    }
    return -1; // error
}
