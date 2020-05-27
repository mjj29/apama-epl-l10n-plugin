/**
 * Title:        L10NPlugin.cpp
 * Description:  EPL Translation plugin using gettext
 * Copyright (c) 2020 Software AG, Darmstadt, Germany and/or its licensors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 * http:/www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 * See the License for the specific language governing permissions and limitations under the License. 
 */

#define __STDC_FORMAT_MACROS 1
#include <epl_plugin.hpp>
#include <inttypes.h>
#include <libintl.h>
#include <boost/format.hpp>

using namespace com::apama::epl;

namespace com {
namespace apamax {

/**
 * This plugin allows invoking the jemalloc API to force collections
 */
class L10NPlugin: public EPLPlugin<L10NPlugin>
{
public:
	L10NPlugin() : base_plugin_t("L10NPlugin")
	{
	}
	static void initialize(base_plugin_t::method_data_t &md)
	{
		md.registerMethod<decltype(&L10NPlugin::init), &L10NPlugin::init>("init");
		md.registerMethod<decltype(&L10NPlugin::gettext), &L10NPlugin::gettext>("gettext");
		md.registerMethod<decltype(&L10NPlugin::ngettext), &L10NPlugin::ngettext>("ngettext");
		md.registerMethod<decltype(&L10NPlugin::printf), &L10NPlugin::printf>("printf", "action<string, sequence<any> > returns string");
	}

	void init(const char *textdomain, const char *translationsdir)
	{
		setlocale(LC_ALL, "");
		bindtextdomain(textdomain, translationsdir);
	}
	const char *ngettext(const char *textdomain, const char *msgid1, const char *msgid2, int64_t n)
	{
		return dngettext(textdomain, msgid1, msgid2, n);
	}

	const char *gettext(const char *textdomain, const char *msgid)
	{
		return dgettext(textdomain, msgid);
	}
	struct format_visitor: public const_visitor<format_visitor, void>
	{
		boost::format &f;
		format_visitor(boost::format &f):f(f) {}
		void visitInteger(int64_t i) const { f % i; }
		void visitDouble(double d) const { f % d; }
		void visitString(const char *s) const { f % s; }
	};

	std::string printf(const char *s, const list_t &l)
	{
		boost::format f{s};
		f.exceptions( f.exceptions() &
				     ~ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit )  );
		for (auto &d:l) {
			apply_visitor(format_visitor{f}, d);
		}
		return f.str();
	}
	
};

/// Export this plugin
APAMA_DECLARE_EPL_PLUGIN(L10NPlugin)

}}
