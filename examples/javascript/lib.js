print("Hello from lib.js");

timer.setInterval = function (callback, interval) {
    var next = timer.now() + interval;
    var interval_handler = function () {
        var res = callback();
        if (res == true) {
            next += interval;
            next_interval = next - timer.now();
            timer.setTimeout(interval_handler, next_interval);
        }
    }

    return timer.setTimeout(interval_handler, interval);
}

saul.type = {
    CLASS_UNDEF     : 0x00,     /**< device class undefined */
    ACT_ANY         : 0x40,     /**< any actuator - wildcard */
    ACT_LED_RGB     : 0x42,     /**< actuator: RGB LED */
    ACT_SERVO       : 0x43,     /**< actuator: servo motor */
    ACT_MOTOR       : 0x44,     /**< actuator: motor */
    ACT_SWITCH      : 0x45,     /**< actuator: simple on/off switch */
    ACT_DIMMER      : 0x46,     /**< actuator: dimmable switch */
    SENSE_ANY       : 0x80,     /**< any sensor - wildcard */
    SENSE_BTN       : 0x81,     /**< sensor: simple button */
    SENSE_TEMP      : 0x82,     /**< sensor: temperature */
    SENSE_HUM       : 0x83,     /**< sensor: humidity */
    SENSE_LIGHT     : 0x84,     /**< sensor: light */
    SENSE_ACCEL     : 0x85,     /**< sensor: accelerometer */
    SENSE_MAG       : 0x86,     /**< sensor: magnetometer */
    SENSE_GYRO      : 0x87,     /**< sensor: gyroscope */
    SENSE_COLOR     : 0x88,     /**< sensor: (light) color */
    SENSE_PRESS     : 0x89,     /**< sensor: pressure */
    SENSE_ANALOG    : 0x8a,     /**< sensor: raw analog value */
    SENSE_UV        : 0x8b,     /**< sensor: UV index */
    CLASS_ANY       : 0xff      /**< any device - wildcard */
}

saul.op = {
    LT : 0,
    LE : 1,
    EQ : 2,
    GE : 3,
    GT : 4
}

saul.get_one = function(type) {
    var res = saul._get_one(type);
    if (typeof type !== 'undefined') {
        res.on_threshold = function(threshold, callback, operator) {
            var sensor = this;
            var operator = (typeof operator !== 'undefined') ? operator : saul.op.GE;
            var poller = function() {
                var val = sensor.read();
                var trigger = false;
                switch (operator) {
                    case saul.op.LT:
                        trigger = (val < threshold);
                        break;
                    case saul.op.LE:
                        trigger = (val <= threshold);
                        break;
                    case saul.op.EQ:
                        trigger = (val == threshold);
                        break;
                    case saul.op.GE:
                        trigger = (val >= threshold);
                        break;
                    case saul.op.GT:
                        trigger = (val > threshold);
                        break;
                }
                if (trigger) {
                    return callback();
                } else {
                    return true;
                }
            }

            return timer.setInterval(poller, 100000);
        }
        res.sample = function(n) {
            var result = {
                min : Number.MAX_VALUE,
                max : Number.MIN_VALUE,
                avg : 0
            }

            var left = n
            while (left > 0) {
                var val = this.read();
                result.avg += val;
                result.max = Math.max(result.max, val);
                result.min = Math.min(result.min, val);
                left -= 1;
            }
            result.avg /= n;
            return result;
        }
    }
    return res;
}
