/*
Copyright (c) 2005, 
	Aaron Lefohn (lefohn@cs.ucdavis.edu)
All rights reserved.

This software is licensed under the BSD open-source license. See
http://www.opensource.org/licenses/bsd-license.php for more detail.

*************************************************************
Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following 
conditions are met:

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 

Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 

Neither the name of the University of Californa, Davis nor the names of 
the contributors may be used to endorse or promote products derived 
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "glErrorUtil.h"
#include <GL/glew.h>
using namespace std;

#ifndef NDEBUG
void CheckErrorsGL(const char* location, ostream& ostr) 
{
  GLuint errnum;
  const char *errstr;
  while (errnum = glGetError()) {
    errstr = reinterpret_cast<const char *>(gluErrorString(errnum));
    if(errstr) {
      ostr << errstr; 
    }
    else {
      ostr << "Error " << errnum;
    }
    
    if(location) ostr << " at " << location;		
    ostr << endl;
  }

  return;
}
#endif
