from pysys.constants import *
from pysys.basetest import BaseTest
from apama.correlator import CorrelatorHelper

class PySysTest(BaseTest):
	def execute(self):
		corr = CorrelatorHelper(self, name='correlator')
		corr.start(logfile='correlator.log')
		corr.injectEPL(filenames=['ManagementImpl.mon', 'Management.mon'], filedir=self.project.APAMA_HOME+'/monitors')
		corr.injectEPL(filenames=['L10N.mon'], filedir=self.input+'/../../../')
		corr.injectEPL(filenames=['Sample.mon'])
		corr.flush()
		corr.shutdown()

	def validate(self):
		self.assertOrderedGrep('correlator.out', exprList=[
			"0 arg string",
			"1 arg string: Hello World",
			"1 arg integer: 42",
			"1 arg float: 3.140000",
			"2 args: Hello World",
			"ordering: Hello 3.140000",
			"insufficent args: Hello  ",
			"excess args: Hello",
			"format: 3.14",
			"wrong: 42 Hello",
		])

