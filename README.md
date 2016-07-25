# Koruza driver firmware

This repo represents the firmware for [STM32F411RET6][MCU_LINK] MCU. This MCU is located on the [Koruza driver board][BOARD_LINK].

### Why NUCLEO board?

Reasons for choosing open Nucleo environment are:

* it is cheaper then Arduino UNO
* better ARM MCU, a possibility for new features
* open SDK and IDE


### Environment setup

Programing environment is based on System Workbench for STM32. The System Workbench toolchain, called SW4STM32, is a free multi-OS software development environment based on Eclipse, which supports the full range of STM32 microcontrollers and associated boards. The SW4STM32 toolchain may be obtained from the website www.openstm32.org, which includes forums, blogs, and trainings for technical support. Once registered to this site, users will get installation instructions at the Documentation > System Workbench page to proceed with the download of the free toolchain.

After sucesful instalation of toolchain, you nead to download git repository:

```bash
$ git clone https://github.com/IRNAS/grbl_stm32.git
```

Import project to IDE and compile.

Information on importing project, compiling, and downloading code to the board, can be found here www.openstm32.org

### Hardware
Check the [Koruza driver board][BOARD_LINK] repository.



[MCU_LINK]: <http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1577/LN1877/PF260049>
[BOARD_LINK]: <https://github.com/IRNAS/Universal-Stepper-Driver-Rpi>