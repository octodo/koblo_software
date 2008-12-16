
#include "KSOS.h"


CKSXML_Write::CKSXML_Write(CKSPlugIn * pKSPlugIn)
{
	
	mpKSPlugIn = pKSPlugIn;
	
}

CKSXML_Write::~CKSXML_Write()
{
	
}
void CKSXML_Write::Write_XML(std::string str)
{
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	mpDoc.LinkEndChild( decl );
	
	
	Add_Comment("Koblo Studio music project XML format");
	Add_Comment("most ids are originally returned from the website, and are globally unique (between different projects)");
	Add_Comment("ids are always specified as attributes");
	Add_Comment("all times and track positions are specified in sample points");
	
//	TiXmlElement * element = new TiXmlElement( "Hello" );
//	TiXmlText * text = new TiXmlText( "World" );
//	element->LinkEndChild( text );
	
//	mpDoc.LinkEndChild( element );
	mpDoc.SaveFile( str.c_str() );

	
}

void CKSXML_Write::Add_Comment(std::string str)
{
	
//	TiXmlDeclaration* decl = new TiXmlDeclaration( s.c_str() );
	TiXmlComment*	comment = new TiXmlComment(str.c_str() );
	mpDoc.LinkEndChild( comment );

}