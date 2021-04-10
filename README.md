# LogMerge

This application takes multiple input files, optionally filters by event type(s), and produces a single, UTC timestamp-ordered, output file that only has events of the specified event types. 

## Installation

Open your terminal and go to LogMerge/ directory.

```bash
cd /LogMerge
```

Type the command below to generate MakeFile for the project.

```bash
qmake "CONFIG+=LogMerge" LogMerge.pro
```
Next, type the command below:

```bash
make
```
This will build the program and generate an executable file to run the program with.


## Usage

This program lets you to choose a direct that all log files are located there and have the option to filter some data while merging all log files.

Go to the project directory.

```bash
cd /LogMerge
```

For merging files with no filters use the following pattern:

```bash
./LogMerge -dir /pathto/logfiles/
```

For entering the filter(s) use the following pattern.


```bash
./LogMerge -filter foo -dir /pathto/logfiles/
```

If you want to enter more than one filter make sure to put comma without space between them as an example below:

```bash
./LogMerge -filter foo,bar -dir /pathto/logfiles/
```


