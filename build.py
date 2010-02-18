#----------------------------------------------------------------------
# Name:         build.py
# Author:       (c) 2004 Jory Stone <matroskautils@jory.info>
# Licence:      LGPL license
#----------------------------------------------------------------------
"""Custom Build Step Python script for compiling MatroskaProp and MatroskaUtil
LGPL license - (c) 2004 Jory Stone <matroskautils@jory.info>

Usage: build.py [-v] [-f file] [-i file] [-r file] [-m file]

 -v         Verbose output
 -h         This usage help
 -f file    Input version resource file to update
 -i file    NSIS installer script to update
 -m file    The .vdproj installer project to update
 -r file    resource.cpp object file location (deletes it so it will be rebuilt)
 -p file    VC7.1 project file to archive
"""

import string
import codecs
import re
import os
import os.path
import time
import sys
import stat
import getopt
import shutil
import xml.parsers.expat
import zipfile
import tarfile 
import profile

class MatroskaUtilCustomBuild:
    version_major = -1
    version_minor = -1
    version_build = -1
    version_filename = None
    installer_script_filename = None
    vdproj_installer_filename = None
    resource_object_file = None
    project_filename = None
    solution_filename = None
    verbose = False
    autoincrement = False
    file_list = list()
    archive_base_path = ""
    solution_base_path = ""
    project_base_path = ""
    archive_filename = None
    archive_path = None
    
    def __init__(self):        
        print "MatroskaUtil Custom Build Step Python Script"
        try:
            opts, args = getopt.getopt(sys.argv[1:], "vhaf:i:r:p:s:m:", ["verbose", "help", "autoincrement", "file=", "installer=", "resouceobj=", "project=", "solution=", "msi="])
        except getopt.GetoptError, msg:
            print msg
            print __doc__,
            return 1

        for o, a in opts:
            if o in ("-v", "--verbose"):
                self.verbose = True
            elif o in ("-f", "--file"):
                self.version_filename = a
            elif o in ("-i", "--installer"):
                self.installer_script_filename = a
            elif o in ("-m", "--msi"):
                self.vdproj_installer_filename = a                
            elif o in ("-p", "--project"):
                self.project_filename = a
            elif o in ("-s", "--solution"):
                self.solution_filename = a
            elif o in ("-a", "--autoincrement"):
                self.autoincrement = True                
            elif o in ("-h", "--help"):
                print __doc__,
                return 1

    def AddFileListEntry(self, path):
        if (self.FilterEntry(path)):
            self.file_list.append(path)
            
    def FilterEntry(self, path):
        if (path.find("libebml") > -1):
            return False
        elif (path.find("libmatroska") > -1):
            return False
        return True
    
    def FilterEntries(self):    
        for file_list_index, file_list_entry in enumerate(self.file_list):
            if (file_list_entry.find("libebml") > -1):
                print "Removing " + str(file_list_index) + ":" + file_list_entry
                del self.file_list[file_list_index]
            elif (file_list_entry.find("libmatroska") > -1):
                print "Removing " + str(file_list_index) + ":" + file_list_entry                
                del self.file_list[file_list_index]
                
    def start_element(self, name, attrs):
        if (name.find("File") > -1):
            try:
                path = attrs['RelativePath']                
                path = os.path.normpath(os.path.join(self.project_base_path, path))
                #self.file_list.append(path)
                self.AddFileListEntry(path)
            except KeyError, msg:
                msg = ""

    def ArchiveSolution_TarBz2(self):
        self.archive_path = self.archive_filename + ".tar.bz2"
        tararchive = tarfile.open(self.archive_path, "w:bz2")
        for file_list_index, file_list_entry in enumerate(self.file_list):
            local_path = str(file_list_entry)
            arc_path = str(file_list_entry.replace(self.archive_base_path, ""))
            tararchive.add(local_path, arc_path)
        tararchive.close()

    def ArchiveSolution_TarGz(self):
        self.archive_path = self.archive_filename + ".tar.gz"
        tararchive = tarfile.open(self.archive_path, "w:gz")
        for file_list_index, file_list_entry in enumerate(self.file_list):
            local_path = str(file_list_entry)
            arc_path = str(file_list_entry.replace(self.archive_base_path, ""))
            tararchive.add(local_path, arc_path)
        tararchive.close()

    def ArchiveSolution_Zip(self):
        self.archive_path = self.archive_filename + ".zip"        
        archive = zipfile.ZipFile(self.archive_path, "w", zipfile.ZIP_DEFLATED)
        for file_list_index, file_list_entry in enumerate(self.file_list):
            local_path = str(file_list_entry)
            arc_path = str(file_list_entry.replace(self.archive_base_path, ""))
            archive.write(local_path, arc_path)
        archive.close()

    def ArchiveSolution(self):
        if self.solution_filename is None:
            raise ("Error: VC7.1 Solution Filename is required.\n" + __doc__)

        print "Parsing Solution file"
        self.solution_base_path = os.path.split(self.solution_filename)[0]
        path = os.path.normpath(os.path.join(self.solution_base_path, self.solution_filename))
        self.AddFileListEntry(path)

        solution_file = open(self.solution_filename, "r")
        
        solution_file_lines = solution_file.readlines()
        for solution_file_line in solution_file_lines:
            if (solution_file_line.find(".vcproj") > -1):
                solution_values = re.match("Project\(\"(\S+)\"\) = \"(\S+)\", \"(\S+)\", \"(\S+)\"", solution_file_line).groups()
                #print solution_values
                path = os.path.join(self.solution_base_path, solution_values[2])
                self.project_filename = os.path.normpath(path)
                if (self.FilterEntry(self.project_filename)):
                    self.ScanProject()
        
        #print self.file_list

        if self.archive_filename is None:
            self.archive_filename = os.path.splitext(self.solution_filename)[0]            
            if (self.version_major > 0):
                # Format it with the version
                self.archive_filename += "-" + str(self.version_major) + "." + str(self.version_minor)
            else:
                # Format it with the time
                self.archive_filename += time.strftime("-%Y%m%d")
            self.archive_filename += "-src"
            
        #profile.run('build.ArchiveSolution_Zip()')
        #profile.run('build.ArchiveSolution_TarGz()')
        #profile.run('build.ArchiveSolution_TarBz2()')
        self.ArchiveSolution_TarBz2()
        print len(self.file_list), "files archived to", os.path.split(self.archive_path)[1]

    def ScanProject(self):
        if self.project_filename is None:
            raise ("Error: VC7.1 Project Filename is required.\n" + __doc__)

        self.project_base_path = os.path.split(self.project_filename)[0]
        path = os.path.normpath(os.path.join(self.project_base_path, self.project_filename))
        self.file_list.append(path)
        # Parse XML based project file
        parser = xml.parsers.expat.ParserCreate()
        parser.StartElementHandler = self.start_element        
        parser.ParseFile(open(self.project_filename, "r"))
        #print self.file_list
            
        self.archive_base_path = os.path.commonprefix(self.file_list)
        
    def UpdateResourceRCFile(self):
        if self.version_filename is None:
            raise ("Error: Version Filename is required.\n" + __doc__)
        
        version_bak = self.version_filename + ".bak"
        version_file = open(self.version_filename, "r")        

        # Backup the version file before changing it
        shutil.copyfileobj(version_file, open(version_bak, "w"))
        version_file.seek(0)

        # Go through the file line-by-line
        version_file_lines = version_file.readlines()
        for version_file_line_index, version_file_line in enumerate(version_file_lines):
            if (version_file_line.find("FILEVERSION") > -1):
                # Extract the current version
                version_str = version_file_line.replace("FILEVERSION", "")
                version_str = version_str.strip()
                version_str = version_str.split(",")
                self.version_major = int(version_str[0])
                self.version_minor = int(version_str[1])
                self.version_build = int(version_str[3])
                print "Major: " + str(self.version_major) + " Minor: " + str(self.version_minor) + " Build Number: " + str(self.version_build)
                if self.autoincrement:
                    print "Auto-Incrementing the Build Count..."
                    self.version_build += 1
                version_file_lines[version_file_line_index] = " FILEVERSION " + str(self.version_major) + ", " + str(self.version_minor) + ", 0, " + str(self.version_build) + "\n"

            elif (version_file_line.find("FileVersion") > -1):
                if self.version_build < 0:
                    raise "Error: Version not found!!!"
                version_file_lines[version_file_line_index] = "            VALUE \"FileVersion\", \"" + str(self.version_major) + ", " + str(self.version_minor) + ", 0, " + str(self.version_build) + "\\0\" \n"

            elif (version_file_line.find("PRODUCTVERSION") > -1):
                if self.version_build < 0:
                    raise "Error: Version not found!!!"
                version_file_lines[version_file_line_index] = " PRODUCTVERSION " + str(self.version_major) + ", " + str(self.version_minor) + ", 0, " + str(self.version_build) + "\n"
                
            elif (version_file_line.find("ProductVersion") > -1):
                if self.version_build < 0:
                    raise "Error: Version not found!!!"
                version_file_lines[version_file_line_index] = "            VALUE \"ProductVersion\", \"" + str(self.version_major) + ", " + str(self.version_minor) + ", 0, " + str(self.version_build) + "\\0\" \n"
                
        # Close and reopen the file for writing
        version_file.close() 
        version_file = open(self.version_filename, "w")
        version_file.writelines(version_file_lines)

    def UpdateResourceHeaderFile(self):
        # Update the resource.h file too
        version_resource_filename = os.path.split(self.version_filename)[0] + "\\resource.h"
        version_resource_bak = version_resource_filename + ".bak"
        version_file = open(version_resource_filename, "r")

        # Backup the version file before changing it
        shutil.copyfileobj(version_file, open(version_resource_bak, "w"))
        version_file.seek(0)

        # Go through the file line-by-line
        version_file_lines = version_file.readlines()
        for version_file_line_index, version_file_line in enumerate(version_file_lines):
            if (version_file_line.find("RESOURCE_VERSION_MAJOR") > -1):
                version_file_lines[version_file_line_index] = "#define RESOURCE_VERSION_MAJOR " + str(self.version_major) + "\n"
                
            elif (version_file_line.find("RESOURCE_VERSION_MINOR") > -1):
                version_file_lines[version_file_line_index] = "#define RESOURCE_VERSION_MINOR " + str(self.version_minor) + "\n"
                
            elif (version_file_line.find("RESOURCE_VERSION_BUILD") > -1):
                version_file_lines[version_file_line_index] = "#define RESOURCE_VERSION_BUILD " + str(self.version_build) + "\n"
                            
        # Close and reopen the file for writing
        version_file.close() 
        version_file = open(version_resource_filename, "w")
        version_file.writelines(version_file_lines)

    def UpdateResourceCppFile(self):
        # Update the resource.h file too
        version_resource_filename = os.path.split(self.version_filename)[0] + "\\resource.cpp"

        version_file_lines = list()
        version_file_lines.append("#include \"resource.h\"\n\n")
        version_file_lines.append("long g_MyVersionMajor = RESOURCE_VERSION_MAJOR;\n")
        version_file_lines.append("long g_MyVersionMinor = RESOURCE_VERSION_MINOR;\n")
        version_file_lines.append("long g_MyBuildNumber = RESOURCE_VERSION_BUILD;\n")

        #if os.path.exists(version_resource_filename):
        #    version_file = open(version_resource_filename, "r")
        #    # Read in the whole file
        #    version_file_lines = version_file.readlines()
        #    version_file.close()                    
                            
        # Open the file for writing
        version_file = open(version_resource_filename, "w")
        version_file.writelines(version_file_lines)

        if self.resource_object_file is not None:
            print "Deleting resource.cpp object file..."
            os.remove(self.resource_object_file)
        
    def UpdateNSISInstallerFile(self):
        if self.installer_script_filename is None:
            raise ("Error: NSIS Installer Script Filename is required.\n" + __doc__)

        print "Updating NSIS installer script..."
        # Update the NSIS installer script
        installer_script_bak = self.installer_script_filename + ".bak"
        installer_script_file = open(self.installer_script_filename, "r")

        # Backup the version file before changing it
        shutil.copyfileobj(installer_script_file, open(installer_script_bak, "w"))
        installer_script_file.seek(0)

        # Go through the file line-by-line
        installer_script_lines = installer_script_file.readlines()
        for installer_script_line_index, installer_script_line in enumerate(installer_script_lines):
            if (installer_script_line.find("!define MATROSKAPROP_VERSION_MAJOR") > -1):
                installer_script_lines[installer_script_line_index] = "!define MATROSKAPROP_VERSION_MAJOR " + str(self.version_major) + "\n"
                
            elif (installer_script_line.find("!define MATROSKAPROP_VERSION_MINOR") > -1):
                installer_script_lines[installer_script_line_index] = "!define MATROSKAPROP_VERSION_MINOR " + str(self.version_minor) + "\n"
                
            elif (installer_script_line.find("!define MATROSKAPROP_VERSION_BUILD") > -1):
                installer_script_lines[installer_script_line_index] = "!define MATROSKAPROP_VERSION_BUILD " + str(self.version_build) + "\n"

            elif (installer_script_line.find("!define MATROSKAPROP_SOURCE_ARCHIVE") > -1):
                if self.archive_path is not None:
                    installer_script_lines[installer_script_line_index] = "!define MATROSKAPROP_SOURCE_ARCHIVE \"" + self.archive_path + "\"\n"
                            
        # Close and reopen the file for writing
        installer_script_file.close() 
        installer_script_file = open(self.installer_script_filename, "w")
        installer_script_file.writelines(installer_script_lines)

    def UpdateMSIInstallerFile(self):
        if self.vdproj_installer_filename is None:
            raise ("Error: MSI Installer Script Filename is required.\n" + __doc__)

        print "Updating MSI installer script..."
        # Update the NSIS installer script
        installer_script_bak = self.vdproj_installer_filename + ".bak"
        installer_script_file = open(self.vdproj_installer_filename, "r")

        # Backup the version file before changing it
        shutil.copyfileobj(installer_script_file, open(installer_script_bak, "w"))
        installer_script_file.seek(0)

        # Go through the file line-by-line
        installer_script_lines = installer_script_file.readlines()
        for installer_script_line_index, installer_script_line in enumerate(installer_script_lines):
            if (installer_script_line.find("\"ProductVersion\" = ") > -1):
                installer_script_lines[installer_script_line_index] = "\"ProductVersion\" = \"8:" + str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_build) + "\" \n"                
                            
        # Close and reopen the file for writing
        installer_script_file.close() 
        installer_script_file = open(self.vdproj_installer_filename, "w")
        installer_script_file.writelines(installer_script_lines)

        
    def Build(self):
        #self.version_filename = "D:\Visual Studio Projects\matroska\MatroskaUtils\\MatroskaCDL.rc"
        #self.installer_script_filename = "D:\Visual Studio Projects\matroska\MatroskaUtils\ShellExtension\MatroskaProp_Language.nsi"
        #self.project_filename = "D:\Visual Studio Projects\matroska\MatroskaUtils\ShellExtension\MatroskaProp.vcproj"
        #self.solution_filename = "D:\Visual Studio Projects\matroska\MatroskaUtils\ShellExtension\MatroskaProp.sln"

        if self.solution_filename is not None:
            self.ArchiveSolution()        

        if self.version_filename is not None:
            self.UpdateResourceRCFile()
            self.UpdateResourceHeaderFile()
            self.UpdateResourceCppFile()
        if self.installer_script_filename is not None:
            self.UpdateNSISInstallerFile()
    
if __name__ == '__main__':
    build = MatroskaUtilCustomBuild()
    build.Build()
