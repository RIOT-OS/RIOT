/**
@defgroup pkg_lv_drivers LittlevGL Drivers
@ingroup  pkg
@brief    Display controller and touchpad driver to can be directly used with
          LittlevGL.

@note     This package needs the 32bit version of SDL2

@see      https://github.com/lvgl/lv_drivers

## Configuration options

To change the size of the screen `SDL_HOR_RES` and `SDL_VER_RES` can be changed
via `CFLAGS` or in `lv_drv_conf.h`.

For small screen a zoom factor can be applied through `SDL_ZOOM`, e.g: `SDL_ZOOM=2`
will apply a x2 zoom to the display.

### SDL Requirements

To run lvgl inside a simulator (native), SDL2 is required. Install instructions
can be found at https://docs.lvgl.io/latest/en/html/get-started/pc-simulator.html#install-sdl-2,
note that the 32bit version is needed.

On debian/ubuntu this could be as simple as installing the `libsdl2-dev:i386` package.

Using SDL2 requires more stack so in case you are using it add

```
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=48*1024'
```

to your makefile. 48kB is enough for the test, other uses may need more or may
need this to be applied to other threads using `THREAD_STACKSIZE_DEFAULT`

 */
