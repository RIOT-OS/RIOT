print("Hello from JerryScript!!!!");

//var n = 0;
//timer.setInterval(function () { n += 1; print(n, "triggers, time is", timer.now()); }, 1000000);

button = saul.get_one(saul.type.SENSE_BTN);
led = saul.get_one(saul.type.ACT_SWITCH);

led.write(1);

button.on_threshold(0, function(){led.write(0); return true;}, saul.op.EQ);
button.on_threshold(1, function(){led.write(1); return true;});
