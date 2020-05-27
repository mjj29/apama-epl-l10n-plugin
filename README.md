# EPL L10NPlugin

Apama EPL Plugin for translating messages using GNU gettext

## GNU Gettext

Gettext is an integrated set of tools and libraries for translating strings within code and displaying translations to users based on their configured locale. More information can be found on [their website](https://www.gnu.org/software/gettext/) and the [manual for the Gettext utilities](https://www.gnu.org/software/gettext/manual/html_node/index.html)

## Supported Apama version

This works with Apama 10.3 or later

## Building the plugin

To build you will need an Apama installation and a copy of the Gettext libraries and utilities. Apama can be downloaded from [Apama Community](http://www.apamacommunity.com). Gettext is normally installed by default on all major Linux distributions. If you are using a thin base image or similar you may need to install the relevant packages first, which can be found in the packages `gettext` and `libc6-dev` on Debian-based distributions and the `gettext` and `glibc-headers` packages on Redhat-based distributions. Windows binaries can be downloaded from [this site](https://mlocati.github.io/articles/gettext-iconv-windows.html).

In an Apama command prompt on Linux run:

	mkdir -p $APAMA_WORK/lib
	g++ -std=c++11 -o $APAMA_WORK/lib/libL10NPlugin.so -I$APAMA_HOME/include -L$APAMA_HOME/lib -lapclient -shared -fPIC L10NPlugin.cpp


On Windows run:

	g++ -std=c++11 -o %APAMA_WORK%\lib\L10NPlugin.dll -I%APAMA_HOME%\include -L%APAMA_HOME%\lib -lapclient -shared L10NPlugin.cpp

## Running tests

To run the tests for the plugin you will need to use an Apama command prompt to run the tests from within the tests directory:

	pysys run

## Using the L10NPlugin

To use the L10NPlugin you must first inject L10N.mon, import it into your monitor:

	using com.apamax.L10N;

Then create an instance of L10N and initialize it with a name identifying the set of translations and the location of the translation files. There are several options to locate the translation files. By default it will look in $APAMA\_WORK/translations/identifier. Alternatively you can provide an absolute path, or the value of a configuration property:

	monitor test {
		L10N l10n;
		action onload() {
			l10n.init("test");
			l10n.initFromProperty("test", "TEST_TRANSLATIONS");
			l10n.initFromPath("test", "/path/to/translations");
		}
	}

Next it's recommended you setup some alias functions in your monitor, to make calling for translations simpler:

	monitor test {
		L10N l10n;
		action _(string s) returns string { return l10n.gettext(s); }
		action _N(string s1, string s2, integer n) returns string { return l10n.ngettext(s1, s2, n); }
		action printf(string f, sequence<any> a) returns string { return l10n.printf(f, a); }
		...
	}

You can then use these functions to write translatable strings:

	log _("Application setup and ready") at INFO;
	try {
		...
	} catch (Exception e) {
		log printf(_("Caught exception type %1$s message: %2$s"), e.getType(), e.getMessage());
	}
	on all MyEvent() as me {
			count := count + 1;
			log printf(_N("Received a total of one message, it is %1$s", "Received a total of %2$d messages, latest is %1$s", count), [<any>me.toString(), count]);
	}

The L10N plugin and gettext allow you to specify handling for plurals, which might be treated very differently in different languages with the use of the ngettext function, here aliased to \_N for convenience.

Next, you will want to extract the strings from your monitor and generate translations for them. This can be done with the standard gettext tooling to extract the text and generate .po and then .mo files for the translations. To extract the text strings from the .mon file run xgettext with these arguments:

	xgettext --package-name test --package-version 1.2 --default-domain test --output MyMessages.pot MyMonitor.mon -C  -k_ -k_N:1,2 -kgettext -kngettext:1,2 

Here the domain specifier should match the identifier you pass to init.

Next create an input file for each translation language (here shown with Spanish):

	msginit --no-translator --locale es_MX --output-file MessagesSpanish.po --input MyMessages.pot

The translator should provide translations in the MessagesSpanish.po file

Lastly, convert the translated .po file into a compiled .mo file:

	mkdir -p es_MX.utf8/LC_MESSAGES
	msgfmt --check --verbose --output-file ./es_MX.utf8/LC_MESSAGES/test.mo MessagesSpanish.po

Again, here the name of the .mo file should match the identifier passed to init.

Finally, if you run the correlator in the appropriate locale, it will return messages from the matching translated file. To see this in action, see the sample below.

## Sample

A simple end-to-end sample is provided in the sample folder. To run the sample in English run the following from an Apama command prompt:

	correlator --config sample

You should see it produce messages similar to these:

	Setup and waiting for messages
	Received a total of one message, it is EventOne
	Received a total of 2 messages, latest is EventTwo
	Received a total of 3 messages, latest is EventThree

To run the sample in Spanish to see the translations, run the following from an Apama command prompt:

	LANG=es_MX.utf8 correlator --config sample

You should see it produce messages similar to these:

	Configurar y esperar mensajes
	Recibió un total de 1 mensaje, es EventOne
	Recibió un total de 2 mensajes, el último es EventTwo
	Recibió un total de 3 mensajes, el último es EventThree

