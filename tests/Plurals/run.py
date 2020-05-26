from pysys.constants import *
from pysys.basetest import BaseTest
from apama.correlator import CorrelatorHelper

class PySysTest(BaseTest):
	def execute(self):
		for lang in ['en_GB.utf8', 'es_MX.utf8']:
			corr = CorrelatorHelper(self, name=f'correlator_{lang}')
			corr.start(logfile=f'correlator_{lang}.log', environ={'LANG':lang}, arguments=[f'-DSAMPLE_TRANSLATIONS={self.input}/../../../sample'])
			corr.injectEPL(filenames=['ManagementImpl.mon', 'Management.mon'], filedir=self.project.APAMA_HOME+'/monitors')
			corr.injectEPL(filenames=['L10N.mon'], filedir=self.input+'/../../../')
			corr.injectEPL(filenames=['Sample.mon'])
			corr.flush()
			corr.shutdown()

	def validate(self):
		self.assertGrep('correlator_en_GB.utf8.out', expr=r'Single: Received a total of one message, it is %1\$s')
		self.assertGrep('correlator_en_GB.utf8.out', expr=r'Plural: Received a total of %2\$d messages, latest is %1\$s')
		self.assertGrep('correlator_es_MX.utf8.out', expr=r'Single: Recibió un total de %2\$d mensaje, es %1\$s')
		self.assertGrep('correlator_es_MX.utf8.out', expr=r'Plural: Recibió un total de %2\$d mensajes, el último es %1\$s')
