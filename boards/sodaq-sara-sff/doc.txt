/**
 * @defgroup    boards_sodaq-sara-sff SODAQ SARA SFF
 * @ingroup     boards
 * @brief       Support for the SODAQ SARA SFF boards
 *
 * ### General information
 *
 * General information about this board can be found on the
 * [SODAQ support](https://support.sodaq.com/Boards/Sara_SFF/)
 * website.
 * Schematic can be found [here](https://learn.sodaq.com/Boards/Sara_SFF/sodaq_sara_sff_r2.pdf)
)
 *
 * ### Flash the board
 *
 * Use `BOARD=sodaq-one` with the `make` command.<br/>
 * Example with `hello-world` application:
 * ```
 *      make BOARD=sodaq-one -C examples/hello-world flash
 * ```
 *
 * @note     If the application crashes, automatic reflashing via USB, as explained
 *           above won't be possible. In this case, the board must be set in
 *           bootloader mode by double tapping the reset button before running the
 *           flash command.
 *
 * ### Accessing STDIO via UART
 *
 * STDIO of RIOT is directly available over the USB port.
 *
 */
