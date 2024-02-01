# Time Tagger CLI

This project contains a command-line interface (CLI) for a Time Tagger application. The application listens for signals on multiple channels and records the time codes of these signals. It then identifies coincidences and accidents based on a defined time window.

## Installation

To use the Time Tagger CLI, you need to have a C compiler installed. You can download and install a C compiler from the following link [GCC](https://gcc.gnu.org/install/index.html). If you just want to execute the current software you can jump this step.

Once you have the C compiler installed, you can compile the `time_tagger.c` file using the following command:

```bash
gcc -o time_tagger time_tagger.c
```

## Usage
After compiling the `time_tagger.c` file, you can run the Time Tagger CLI using the following command:
```bash
./time_tagger
```

During the experiment, you can enter the channel numbers ($0$ to $N-1$) to simulate a signal on a channel. The experiment will run for a specified duration, and at the end, it will print the time codes of the signals and the number of coincidences and accidents.

## Pimping

If you wish to modify the experiment duration (by default $10$ seconds) or the time window (by default $1$ second), you can do so by modifying the constants at the beginning of the file. SAme for the number of channels, by default set to $6$.
