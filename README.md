# MyRTOS

MyRTOS is a simple Real-Time Operating System (RTOS) designed to help users gain a deeper understanding of RTOS systems and the underlying logic of their operation. This RTOS is implemented on the STM32F103 development board (Cortex-M3 core) and features task stack switching, task management, and inter-task communication.

## Features

- **Task Management and Scheduling**: Utilizes bitmaps and linked lists for efficient task management and scheduling.
- **Interrupt and Scheduler Locking**: Provides mechanisms to protect shared resources using interrupt disable and scheduler lock.
- **Event Management**: Implements semaphores, mailboxes, event flags, and mutexes for robust inter-task communication.
- **Task Stack Switching**: Supports context switching between tasks to ensure real-time performance.

## Getting Started

### Prerequisites

To get started with MyRTOS, you will need the following tools and hardware:

- STM32F103 development board (Cortex-M3 core)
- STM32CubeIDE or another compatible IDE
- ARM GCC toolchain

### Installation

1. Clone the repository to your local machine:

    ```bash
    git clone https://github.com/ryuuuz/MyRTOS.git
    ```

2. Open the project in STM32CubeIDE or your preferred development environment.

3. Configure the project settings to match your hardware specifications.

4. Build and flash the firmware to your STM32F103 development board.

### Running the RTOS

Once the firmware is flashed, the RTOS will start running on the STM32F103 development board. You can observe the task switching and inter-task communication by monitoring the debug output or using a debugger to step through the code.
