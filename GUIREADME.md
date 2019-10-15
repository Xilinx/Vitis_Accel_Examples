Executing Vitis Examples in the Vitis IDE
===============================================================================

The examples available in the Vitis GitHub repository can be compiled by using
either the provided Makefiles or the Vitis IDE. Documentation on how to perform
Makefile compilation is included in the SETUP.md file. This document outlines the steps to run an example application in the Vitis IDE.

To use an example in the IDE, it must be downloaded as an example template, then a new project must be created using the example template. The steps to do this are described below.

### Download the example template in the IDE
- Open the Vitis IDE by running the following command in the terminal window:
```
    vitis
```

- Select *Examples...* from the *Xilinx* menu. This will open a dialog where you can download example templates.

- When the dialog is opened the first time, click Download to download the example repositories. To check for updates to the examples, click *Refresh*.

- After downloading the examples, the examples will show as *Installed*.

*Note:* After installing the examples, they will be available as project templates when creating a new Vitis project.

- Close the dialog.


### Create the Hello World Application Example Project in the IDE
- In the Vitis IDE, go to *File > New > Application Project* to create a new project for the example design. This will open the *New Application Project Wizard*.

- In the *Templates* page, select an example that has been downloaded.

*Note:* some examples require specific hardware or runtime support, and will only be available for matching platforms and runtimes in the *New Application Project Wizard*.

- Finish the wizard. The project will be created with the sources and project settings required to build and run the example.

- Build and run the application.

