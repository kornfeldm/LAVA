![Lava Logo](LAVA/Assets/trapLogo.png)

# LAVA (Lightweight Antivirus Application) v3.1

##### Note: While we encourage anyone interested to explore our code and project as well as fork our project, it not intended to replace a professional piece of antivirus software and comes with no guarantees or warranty of any type. While we strive to produce the best possible software, the user assumes all risk as specified in the license agreement.

### What is LAVA?

LAVA is an academic project created by four undergraduate students at Hofstra University for a software engineering course (CSC 190). Built based on the open source ClamAV (https://www.clamav.net) using libClamAV, LAVA is a GUI-based application that offers the many anti-malware features. 

### Features (As of v3.1)
* **Full GUI Interface**
  * The user interface is completely visual, so the user should not need to interact with the command line interface. In fact, one LAVA initialized, the command line interface will disappear, leaving no distractions or clutter.
* **Types of Scans**
  * Complete Scan
     * Scans all attached drives and their subdirectories
  * Advanced Scan
     * Scans user-specified files and directories
  * Quick Scan
     * Scans all directories in "Antibody File" (Locations.LavaAnti) 
       * Antibody file starts with only the documents and desktop folders
       * As other scans such as Complete Scan or Advanced Scan detect malware, the Antibody File will be updated to contain the directories. If a parent directory is added, it will replace all its subdirectories. Likewise, subdirectories will not be added as they are already accounted for.
* **Scheduled Scans**
  * The user can opt to schedule a scan for later. When this happens a task will be added to Windows Task Scheduler to launch LAVA and have it scan the specified task at the specified time(s). The following parameters can be used:
    * Once
      * Runs the scan once at a specified date and time and deletes the task after it runs.
    * Daily
      * Runs the scan every X days starting on the specified date and time.
    * Weekly
      * Runs the scan every X weeks starting on the specified date and time. The start date determines the day of the weekday the scan will run.
    * Monthly
      * Runs the scan every X months starting on the specified date and time.
* **Database Updates**
  * Depending on which install method you use, the database may already be installed. If it is not (or is out of date), on startup LAVA will activate Freshclam (from ClamAV) to update or install the database files as needed with no input from the user required.

### How was LAVA made?

LAVA was programmed in C++ as a Visual Studio 2019 command-line application. We opted to write LAVA in C++ to minimize overhead and maximize performance while still having access to certain higher-level features of a programming language.

LAVA was made using the following projects:
* ClamAV (for malware detection, databases and Freshclam updates)
  * https://github.com/Cisco-Talos/clamav-devel
* Dirent (for directory traversal)
  * https://github.com/tronkko/dirent
* Nuklear (for GUI)
  * https://github.com/Immediate-Mode-UI/Nuklear
 
### How is LAVA licensed?

LAVA is licensed under the GNU GPLv3 agreement. The full agreement can be read here: (https://www.gnu.org/licenses/gpl-3.0.en.html)

## How Do I Download and Install LAVA?

The latest release LAVA can be downloaded from the release tab above. This is the only official place to download LAVA from. The latest release offers two types of downloads:
* Installer
  * A single exe that will install everything, add LAVA to the start menu, and optionally add a shortcut to the desktop. However, it requires administrator privileges to run. **In most cases we would recommend this option.**
* Zip file
  * The zip file contains the same version of LAVA in a smaller download. To use it, unzip the folder and place it where you would like to install it. We would not recommend a folder that requires administrator privileges as this can potentially cause issues with certain features. Run LAVA.exe in the main folder to run LAVA. On the first run, it will need to download the database files, but this will happen automatically.

## Solutions to Common Errors When Editing the Code

Here are some of the most common errors with the code we've seen and how to solve them:
* Visual Studio gives a DB error upon opening the project:
  1. Close Visual Studio
  2. Navigate to LAVA/LAVA/.vs/LAVA/v16/
  3. Delete Browse.VC.db
  4. Reopen Visual Studio. It will regenerate the file and this typical solves the problem.
* Compiling the code fails to find a DLL file
  * Copy the missing DLL file to the folder containing LAVA.exe.
* Compiling the code fails to find a font, a quarantine, or any other file
  * This is likely an issue with the relative file paths. We have them set for the release, not debugging in Visual Studio, so the paths may be wrong. Updating the relative path for each item causing a problem should solve it.
