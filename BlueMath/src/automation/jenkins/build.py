#
# Python build script for use by hudson build agents.
#

import sys
import os
import shutil
import getopt
import os
import os.path
from subprocess import call, list2cmdline

def RunCmd(cmd, dryRun):
    print list2cmdline(cmd)
    sys.stdout.flush()
    if not dryRun:
        status = call(cmd)
        if status != 0:
            print "Build failed."
            sys.exit()

def BuildAll(buildType, version, platformName, target, cmakeArgs, makeArgs, dryRun, noMake, cleanBuild, externalBuild, externalBuildType):

    # Directory in which to perform out-of-source cmake build
    builddir = os.getcwd() + "/build/build-%s" % (buildType)

    # Top-level test directory
    testdir = os.getcwd() + "/testresults/%s/%s-%s" % (target, platformName, buildType)

    build64Str = ""
    print platformName, "WIN32"
    if platformName in ["OSX", "Linux", "linux-64"]:
        generator = "Unix Makefiles"
        makeProg = "make"
    elif platformName == "WIN32":
        generator = "NMake Makefiles JOM"
        makeProg = "jom"

    if platformName in ["OSX", "linux-64", "WIN32"]:
        build64Str = "-DBUILD_64=1"

    makeCmd = makeProg + " " + makeArgs + " " + target
    makeCmd = makeCmd.split()
    #if platformName != 'WIN32':

    cmakeCmd = ["cmake", "-DCMAKE_BUILD_TYPE=%s" % (buildType), "-DBUILD_ID=%s" % (version), build64Str, "-G", generator]
    cmakeCmd.append(cmakeArgs)

    #if externalBuild:

    cmakeCmd.append("../../LightSamp")

    if cleanBuild:
        if os.path.exists(builddir):
            print "Removing ", builddir
            if not dryRun:
                shutil.rmtree(builddir)

    if not os.path.exists(builddir):
        print "Creating build directory ", builddir
        if not dryRun:
            os.makedirs(builddir)

    if os.path.exists(testdir):
        print "Deleting existing test results directory ", testdir
        if not dryRun:
            shutil.rmtree(testdir)

    print "Creating test directory ", testdir
    if not dryRun:
        os.makedirs(testdir)

    if not dryRun:
        os.chdir(builddir)

    RunCmd(cmakeCmd, dryRun)

    if not noMake:
        RunCmd(makeCmd, dryRun)


def Usage():
    print "Usage: build [-a|--all] [-b type|--buildtype=type] [-c|--configonly] [-n|--dryrun] [-m args|--makeargs=args] [-p name|--platform=name] [-q dir|--qtdir dir] [-e| --externalBuild] [-z | --cmakeargs=args]"


def main(argv):
    buildType = os.getenv("TYPE")
    platformName = os.getenv("PLAT")
    makeArgs = os.getenv("MAKE_ARGS")
    cmakeArgs = os.getenv("CMAKE_ARGS")
    cleanBuild = os.getenv("CLEAN_BUILD")
    target = os.getenv("TARGET")
    version = os.getenv("VERSION")
    externalBuildType = os.getenv("EXTERNAL_BUILD_TYPE")
    externalBuild = False
    dryRun = False
    noMake = False

    if cmakeArgs is None:
        cmakeArgs = ""

    shortopts = "ab:cm:np:t:e:z"
    longopts = ["all", "buildtype=", "configonly", "dryrun", "makeargs=", "platform=", "target=", "externalBuild", "cmakeargs"]

    try:
        opts, args = getopt.getopt(argv[1:], shortopts, longopts)
    except getopt.GetoptError, err:
        print str(err)
        Usage()
        sys.exit(1)

    for o, a in opts:
        if o in ("-a", "--all"):
            cleanBuild = True
        elif o in ("-b", "--buildtype"):
            buildType = a
        elif o in ("-c", "--configonly"):
            noMake = True
        elif o in ("-m", "--makeargs"):
            makeArgs = a
        elif o in ("-z", "--cmakeargs"):
            cmakeArgs = a
        elif o in ("-n", "--dryrun"):
            dryRun = True
        elif o in ("-p", "--platform"):
            platformName = a
        elif o in ("-t", "--target"):
            target = a
        elif o in ("-e", "--externalBuild"):
            externalBuild = True
        else:
            assert False, "unhandled option"

    if platformName is None:
        print "Error: platform name (PLATFORM_NAME) not defined."
        Usage()
        sys.exit()

    if buildType is None:
       buildType = "Release"

    if target is None:
        target = "all"

    if makeArgs is None:
        makeArgs = ""

    if cmakeArgs is None:
        cmakeArgs = ""

    if version is None:
        version = "unknown"

    if externalBuildType is None:
        externalBuildType = ""

    BuildAll(buildType, version, platformName, target, cmakeArgs, makeArgs, dryRun, noMake, cleanBuild, externalBuild, externalBuildType)

if __name__ == "__main__":
    sys.exit(main(sys.argv))
