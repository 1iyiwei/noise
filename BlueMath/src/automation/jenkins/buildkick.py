#
# Wrapper to invoke platform-dependent build scripts
#

import sys
import os
import shutil
import getopt
import subprocess

def Usage():
    print "Usage: build-kick [primary build script arguments]"


def RunCmd(cmd, dryRun):
    print subprocess.list2cmdline(cmd)
    sys.stdout.flush()
    if not dryRun:
        status = subprocess.call(cmd)
        if status != 0:
            print "Build failed."
            sys.exit()

def BuildKick(platformName, buildArgs):

    if platformName == "WIN32" or sys.platform == "win32":
        # Windows requires a bit of hand-holding
        buildCmd = ["LightSamp\\src\\automation\\jenkins\\build-win32.bat"] + buildArgs
    else:
        # Call primary build script directly
        buildCmd = ["python", "LightSamp/src/automation/jenkins/build.py"] + buildArgs

    RunCmd(buildCmd, False)


def main(argv):
    platformName = os.getenv("PLAT")

    shortopts = "p:e"
    longopts = ["platform=","externalBuild"]

    buildArgs = argv[1:]

    try:
        opts, args = getopt.getopt(argv[1:], shortopts, longopts)
    except getopt.GetoptError, err:
        print str(err)
        Usage()
        sys.exit(1)

    for o, a in opts:
        if o in ("-p", "--platform"):
            platformName = a
        elif o in ("-e", "--externalBuild"):
            print "Build external version"
        else:
            assert False, "unhandled option"

    BuildKick(platformName, buildArgs)

if __name__ == "__main__":
    sys.exit(main(sys.argv))
