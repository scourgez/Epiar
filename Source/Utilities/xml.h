/**\file		xml.h
 * \author		Chris Thielen (chris@epiar.net)
 * \date		Created: Monday, April 21, 2008
 * \date		Modified: Saturday, November 21, 2009
 * \brief       Interface with XML files
 * \details
 *
 */


#ifndef __H_XML__
#define __H_XML__

#include "includes.h"
#include <zlib.h>

class XMLFile {
	public:
		XMLFile();
		XMLFile( const string& filename );
		~XMLFile();
		bool Open( const string& filename );
		bool Save( void );
		bool Close();
		string Get( const string& path ); // cast/convert this to whatever return value you need
		void Set( const string& path, const string& value ); // cast/convert this to whatever return value you need
		void Set( const string& path, const float value ); // cast/convert this to whatever return value you need
		void Set( const string& path, const int value ); // cast/convert this to whatever return value you need

	protected:
		string filename;

	private:
		xmlDocPtr xmlPtr;
		map<string,xmlNodePtr> values;

		xmlNodePtr FindNode( const string& path, bool createIfMissing=false );
};

vector<string> TokenizedString(const string& path, const char* tokens);

#endif // __H_XML__
