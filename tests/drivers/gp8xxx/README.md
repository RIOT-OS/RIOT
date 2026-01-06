# About
This is a test application for the Guestgood GP8xxx I2C DACs.

# Usage
This test application will initialize all DACs and provide a shell for
interacting with the DACs.

By default, this test expects the GP8403. Multiple variations of the GP8xxx can
be tested at once, by enabling multiple drivers using `USEMODULE`. See the
`Makefile` for the possible options.

Be aware of the default address selection. Popular development boards of the
GP8xxx chips (e.g. by DFRobot) have non-default I2C addresses. The address can
be overridden by overriding `GP8XXX_PARAM_ADDR` (e.g. `GP8403_PARAM_ADDR`).
