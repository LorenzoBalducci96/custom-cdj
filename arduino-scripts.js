var Arduino = {};

Arduino.hotCueShift = false;

Arduino.hotCueShifOn = function (channel, control, value, status, group) {
    Arduino.hotCueShift = true;
}

Arduino.hotCueShifOff = function (channel, control, value, status, group) {
    Arduino.hotCueShift = false;
}

Arduino.hotCue1Pressed = function (channel, control, value, status, group) {
    if(Arduino.hotCueShift == false)
		engine.setValue(group, 'hotcue_1_activate', 1);
	else
		engine.setValue(group, 'hotcue_1_clear', 1);
}

Arduino.hotCue2Pressed = function (channel, control, value, status, group) {
    if(Arduino.hotCueShift == false)
		engine.setValue(group, 'hotcue_2_activate', 1);
	else
		engine.setValue(group, 'hotcue_2_clear', 1);
}

Arduino.hotCue3Pressed = function (channel, control, value, status, group) {
    if(Arduino.hotCueShift == false)
		engine.setValue(group, 'hotcue_3_activate', 1);
	else
		engine.setValue(group, 'hotcue_3_clear', 1);
}

Arduino.pitchRange = function (channel, control, value, status, group) {
	var currentRange = Math.round(engine.getValue(group,"rateRange")*100)/100;
    	var currentPitch = engine.getValue(group,"rate") * currentRange;
	
	if(currentRange != 0.1){
		engine.setValue(group,"rateRange",0.1);
		engine.setValue(group,"rate",currentPitch/0.1);
	}
	else if(currentRange == 0.1){
		engine.setValue(group,"rateRange",0.9);
		engine.setValue(group,"rate",currentPitch/0.9);
	}
}

// The button that enables/disables scratching
Arduino.wheelTouch = function (channel, control, value, status, group) {
    if ((status & 0xF0) === 0x90) {    // If button down
  //if (value === 0x7F) {  // Some wheels send 0x90 on press and release, so you need to check the value
        var alpha = 1.0/8;
        var beta = alpha/32;
        engine.scratchEnable(1, 128, 33+1/3, alpha, beta);
    } else {    // If button up
        engine.scratchDisable(1);
    }
}

// The wheel that actually controls the scratching
Arduino.wheelTurn = function (channel, control, value, status, group) {

    // B: For a control that centers on 0x40 (64):
    var newValue = value - 64;
    
    // --- End choice
    
    // In either case, register the movement
    if (engine.isScratching(1)) {
        engine.scratchTick(1, newValue); // Scratch!
    } else {
        engine.setValue('[Channel'+ 1 +']', 'jog', newValue); // Pitch bend
    }
}

