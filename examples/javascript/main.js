print("Hello from JerryScript!!!!");

timer.setInterval = function (callback, interval) {
    var next = timer.now() + interval;
    var interval_handler = function () {
        callback();
        next += interval;
        next_interval = next - timer.now();
        timer.setCallback(interval_handler, next_interval);
    }

    return timer.setCallback(interval_handler, interval);
}

var n = 0;
timer.setInterval(function () { n += 1; print(n, "triggers, time is", timer.now()); }, 1000000)
