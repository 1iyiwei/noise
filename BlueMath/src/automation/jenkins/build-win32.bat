@rem
@rem Strip quotation marks added to PATH by Hudson that cause vsvars32.bat to throw an error.

@set PATH=%PATH:"=%
call "%VS100COMNTOOLS%vsvars32.bat"
call "%VCINSTALLDIR%vcvarsall.bat" x64
call python LightSamp/src/automation/jenkins/build.py %*
