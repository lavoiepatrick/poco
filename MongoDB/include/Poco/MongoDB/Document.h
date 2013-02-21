//
// Document.h
//
// $Id$
//
// Library: MongoDB
// Package: MongoDB
// Module:  Document
//
// Definition of the Document class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef _MongoDB_Document_included
#define _MongoDB_Document_included

#include <algorithm>

#include "Poco/BinaryReader.h"
#include "Poco/BinaryWriter.h"

#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/Element.h"

namespace Poco
{
namespace MongoDB
{

class ElementFindByName
{
public:
	ElementFindByName(const std::string& name) : _name(name)
	{
	}

	bool operator()(const Element::Ptr& element)
	{
		return !element.isNull() && element->name() == _name;
	}

private:
	std::string _name;
};

class MongoDB_API Document
	/// Represents a BSON document
{
public:

	typedef SharedPtr<Document> Ptr;


	typedef std::vector<Document::Ptr> Vector;


	Document();
		/// Constructor


	virtual ~Document();
		/// Destructor


	Document& addElement(Element::Ptr element);
		/// Add an element to the document.
		/// The active document is returned to allow chaining of the add methods.


	template<typename T>
	Document& add(const std::string& name, T value)
		/// Creates an element with the given name and value and
		// adds it to the document.
		/// The active document is returned to allow chaining of the add methods.
	{
		return addElement(new ConcreteElement<T>(name, value));
	}


	Document& add(const std::string& name, const char* value)
		/// Creates an element with the given name and value and
		// adds it to the document.
		/// The active document is returned to allow chaining of the add methods.
	{
		return addElement(new ConcreteElement<std::string>(name, std::string(value)));
	}


	Document& addNewDocument(const std::string& name);
		/// Create a new document and add it to this document.
		/// Unlike the other add methods, this method returns
		/// a reference to the new document.


	void clear();
		/// Removes all elements from the document.


	void elementNames(std::vector<std::string>& keys) const;
		/// Puts all element names into std::vector.


	bool empty() const;
		/// Returns true when the document doesn't contain any documents.


	bool exists(const std::string& name);
		/// Returns true when the document has an element with the given name


	template<typename T>
	T get(const std::string& name) const
		/// Returns the element with the given name and tries to convert
		/// it to the template type. When the element is not found, a
		/// NotFoundException will be thrown. When the element can't be
		/// converted a BadCastException will be thrown.
	{
		Element::Ptr element = get(name);
		if ( element.isNull() )
		{
			throw NotFoundException(name);
		}
		else
		{
			if ( ElementTraits<T>::TypeId == element->type() )
			{
				ConcreteElement<T>* concrete = dynamic_cast<ConcreteElement<T>* >(element.get());
				if ( concrete != NULL )
				{
					return concrete->value();
				}
			}
			throw BadCastException("Invalid type mismatch!");
		}
	}


	template<typename T>
	T get(const std::string& name, const T& def) const
		/// Returns the element with the given name and tries to convert
		/// it to the template type. When the element is not found, or
		/// has the wrong type, the def argument will be returned.
	{
		Element::Ptr element = get(name);
		if ( element.isNull() )
		{
			return def;
		}

		if ( ElementTraits<T>::TypeId == element->type() )
		{
			ConcreteElement<T>* concrete = dynamic_cast<ConcreteElement<T>* >(element.get());
			if ( concrete != NULL )
			{
				return concrete->value();
			}
		}

		return def;
	}


	Element::Ptr get(const std::string& name) const;
		/// Returns the element with the given name.
		/// An empty element will be returned when the element is not found.


	template<typename T>
	bool isType(const std::string& name)
		/// Returns true when the type of the element equals the TypeId of ElementTrait
	{
		Element::Ptr element = get(name);
		if ( element.isNull() )
		{
			return false;
		}

		return ElementTraits<T>::TypeId == element->type();
	}


	void read(BinaryReader& reader);
		/// Reads a document from the reader


	virtual std::string toString(int indent = 0) const;
		/// Returns a String representation of the document.


	void write(BinaryWriter& writer);
		/// Writes a document to the reader


protected:

	ElementSet _elements;
};


inline Document& Document::addElement(Element::Ptr element)
{
	_elements.insert(element);
	return *this;
}


inline Document& Document::addNewDocument(const std::string& name)
{
	Document::Ptr newDoc = new Document();
	add(name, newDoc);
	return *newDoc;
}


inline void Document::clear()
{
	_elements.clear();
}


inline bool Document::empty() const
{
	return _elements.empty();
}


inline void Document::elementNames(std::vector<std::string>& keys) const
{
	for(ElementSet::const_iterator it = _elements.begin(); it != _elements.end(); ++it)
	{
		keys.push_back((*it)->name());
	}
}


inline bool Document::exists(const std::string& name)
{
	return std::find_if(_elements.begin(), _elements.end(), ElementFindByName(name)) != _elements.end();
}


// BSON Embedded Document
// spec: document
template<>
struct ElementTraits<Document::Ptr>
{
	enum { TypeId = 0x03 };

	static std::string toString(const Document::Ptr& value, int indent = 0)
	{
		return value.isNull() ? "null" : value->toString(indent);
	}
};

template<>
inline void BSONReader::read<Document::Ptr>(Document::Ptr& to)
{
	to->read(_reader);
}

template<>
inline void BSONWriter::write<Document::Ptr>(Document::Ptr& from)
{
	from->write(_writer);
}

}} // Namespace Poco::MongoDB

#endif //  _MongoDB_Document_included
