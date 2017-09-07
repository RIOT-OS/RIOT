print("Hello from JerryScript!!!!");

timer.setInterval = function (callback, interval) {
    var next = timer.now() + interval;
    var interval_handler = function () {
        callback();
        next += interval;
        next_interval = next - timer.now();
        timer.setTimeout(interval_handler, next_interval);
    }

    return timer.setTimeout(interval_handler, interval);
}

var n = 0;
timer.setInterval(function () { n += 1; print(n, "triggers, time is", timer.now()); }, 1000000)
