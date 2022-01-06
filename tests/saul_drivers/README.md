saul_drivers
============

This application is only used by the CI to check the build of drivers that
provide a SAUL interface.
All such drivers are added automatically to the build along with the
`saul_default` module. This will check:
- the build of the saul interface,
- the build of driver auto initialization.
