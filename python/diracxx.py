#
# diracxx.py : python wrapper around Dirac++ C++ classes for computing
#              QED amplitudes, cross sections, and polarization observables.
#
# usage example:
#   $ python
#   >>> from diracxx import *
#   >>> t1 = TThreeVectorReal(1,2,3)
#   >>> t2 = TThreeVectorComplex((0+1j),(2-5j),(3+2j),(8-4j))
#   >>> t2 += TThreeVectorComplex(t1)
#   >>> t2.Print()
#
# author: richard.t.jones at uconn.edu
# version: august 16, 2017

import ROOT
from libDirac import *
