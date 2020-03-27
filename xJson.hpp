
#pragma once

#include <cstdint>
#include <cmath>
#include <cctype>
#include <QString>
#include <deque>
#include <map>
#include <type_traits>
#include <initializer_list>
#include <ostream>
#include <iostream>

namespace xComponent{

	namespace json {
	
	using std::map;
	using std::deque;
	using std::enable_if;
	using std::initializer_list;
	using std::is_same;
	using std::is_convertible;
	using std::is_integral;
	using std::is_floating_point;
	
	namespace {
	    QString json_escape( const QString &str ) {
	        QString output;
	        for( int i = 0; i < str.size(); ++i )
				switch (str[i].toLatin1()) {
	                case '\"': output += "\\\""; break;
	                case '\\': output += "\\\\"; break;
	                case '\b': output += "\\b";  break;
	                case '\f': output += "\\f";  break;
	                case '\n': output += "\\n";  break;
	                case '\r': output += "\\r";  break;
	                case '\t': output += "\\t";  break;
	                default  : output += str[i]; break;
	            }
	        return std::move( output );
	    }

		QString json_escape_2(const QString &str) {
			QString output;
			for (int i = 0; i < str.size(); ++i)
				switch (str[i].toLatin1()) {
				case '\"': output += "\""; break;
				case '\\': output += "\\"; break;
				case '\b': output += "\b";  break;
				case '\f': output += "\f";  break;
				case '\n': output += "\n";  break;
				case '\r': output += "\r";  break;
				case '\t': output += "\t";  break;
				default: output += str[i]; break;
			}
			return std::move(output);
		}
	}
	
	class HJSON
	{
	    union BackingData {
	        BackingData( double d ) : Float( d ){}
	        BackingData( long   l ) : Int( l ){}
	        BackingData( bool   b ) : Bool( b ){}
	        BackingData( QString s ) : String( new QString( s ) ){}
	        BackingData()           : Int( 0 ){}
	
	        deque<HJSON>        *List;
	        map<QString,HJSON>   *Map;
	        QString             *String;
	        double              Float;
	        long                Int;
	        bool                Bool;
	    } Internal;
	
	    public:
	        enum class Class {
	            Null,
	            Object,
	            Array,
	            String,
	            Floating,
	            Integral,
	            Boolean
	        };
	
	        template <typename Container>
	        class JSONWrapper {
	            Container *object;
	
	            public:
	                JSONWrapper( Container *val ) : object( val ) {}
	                JSONWrapper( std::nullptr_t )  : object( nullptr ) {}
	
	                typename Container::iterator begin() { return object ? object->begin() : typename Container::iterator(); }
	                typename Container::iterator end() { return object ? object->end() : typename Container::iterator(); }
	                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::iterator(); }
	                typename Container::const_iterator end() const { return object ? object->end() : typename Container::iterator(); }
	        };
	
	        template <typename Container>
	        class JSONConstWrapper {
	            const Container *object;
	
	            public:
	                JSONConstWrapper( const Container *val ) : object( val ) {}
	                JSONConstWrapper( std::nullptr_t )  : object( nullptr ) {}
	
	                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::const_iterator(); }
	                typename Container::const_iterator end() const { return object ? object->end() : typename Container::const_iterator(); }
	        };
	
	        HJSON() : Internal(), Type( Class::Null ){}
	
	        HJSON( initializer_list<HJSON> list ) 
	            : HJSON() 
	        {
	            SetType( Class::Object );
	            for( auto i = list.begin(), e = list.end(); i != e; ++i, ++i )
	                operator[]( i->ToString() ) = *std::next( i );
	        }
	
	        HJSON( HJSON&& other )
	            : Internal( other.Internal )
	            , Type( other.Type )
	        { other.Type = Class::Null; other.Internal.Map = nullptr; }
	
	        HJSON& operator=( HJSON&& other ) {
	            ClearInternal();
	            Internal = other.Internal;
	            Type = other.Type;
	            other.Internal.Map = nullptr;
	            other.Type = Class::Null;
	            return *this;
	        }
	
	        HJSON( const HJSON &other ) {
	            switch( other.Type ) {
	            case Class::Object:
	                Internal.Map = 
	                    new map<QString,HJSON>( other.Internal.Map->begin(),
	                                          other.Internal.Map->end() );
	                break;
	            case Class::Array:
	                Internal.List = 
	                    new deque<HJSON>( other.Internal.List->begin(),
	                                      other.Internal.List->end() );
	                break;
	            case Class::String:
	                Internal.String = 
	                    new QString( *other.Internal.String );
	                break;
	            default:
	                Internal = other.Internal;
	            }
	            Type = other.Type;
	        }
	
	        HJSON& operator=( const HJSON &other ) {
	            ClearInternal();
	            switch( other.Type ) {
	            case Class::Object:
	                Internal.Map = 
	                    new map<QString,HJSON>( other.Internal.Map->begin(),
	                                          other.Internal.Map->end() );
	                break;
	            case Class::Array:
	                Internal.List = 
	                    new deque<HJSON>( other.Internal.List->begin(),
	                                      other.Internal.List->end() );
	                break;
	            case Class::String:
	                Internal.String = 
	                    new QString( *other.Internal.String );
	                break;
	            default:
	                Internal = other.Internal;
	            }
	            Type = other.Type;
	            return *this;
	        }
	
	        ~HJSON() {
	            switch( Type ) {
	            case Class::Array:
	                delete Internal.List;
	                break;
	            case Class::Object:
	                delete Internal.Map;
	                break;
	            case Class::String:
	                delete Internal.String;
	                break;
	            default:;
	            }
	        }
	
	        template <typename T>
	        HJSON( T b, typename enable_if<is_same<T,bool>::value>::type* = 0 ) : Internal( b ), Type( Class::Boolean ){}
	
	        template <typename T>
	        HJSON( T i, typename enable_if<is_integral<T>::value && !is_same<T,bool>::value>::type* = 0 ) : Internal( (long)i ), Type( Class::Integral ){}
	
	        template <typename T>
	        HJSON( T f, typename enable_if<is_floating_point<T>::value>::type* = 0 ) : Internal( (double)f ), Type( Class::Floating ){}
	
	        template <typename T>
	        HJSON( T s, typename enable_if<is_convertible<T,QString>::value>::type* = 0 ) : Internal( QString( s ) ), Type( Class::String ){}
	
	        HJSON( std::nullptr_t ) : Internal(), Type( Class::Null ){}
	
	        static HJSON Make( Class type ) {
	            HJSON ret; ret.SetType( type );
	            return ret;
	        }
	
	        static HJSON Load( const QString & );
	
	        template <typename T>
	        void append( T arg ) {
	            SetType( Class::Array ); Internal.List->emplace_back( arg );
	        }
	
	        template <typename T, typename... U>
	        void append( T arg, U... args ) {
	            append( arg ); append( args... );
	        }
	
	        template <typename T>
	            typename enable_if<is_same<T,bool>::value, HJSON&>::type operator=( T b ) {
	                SetType( Class::Boolean ); Internal.Bool = b; return *this;
	            }
	
	        template <typename T>
	            typename enable_if<is_integral<T>::value && !is_same<T,bool>::value, HJSON&>::type operator=( T i ) {
	                SetType( Class::Integral ); Internal.Int = i; return *this;
	            }
	
	        template <typename T>
	            typename enable_if<is_floating_point<T>::value, HJSON&>::type operator=( T f ) {
	                SetType( Class::Floating ); Internal.Float = f; return *this;
	            }
	
	        template <typename T>
	            typename enable_if<is_convertible<T,QString>::value, HJSON&>::type operator=( T s ) {
	                SetType( Class::String ); *Internal.String = QString( s ); return *this;
	            }
	
	        HJSON& operator[]( const QString &key ) {
	            SetType( Class::Object ); return Internal.Map->operator[]( key );
	        }
	
	        HJSON& operator[]( unsigned index ) {
	            SetType( Class::Array );
	            if( index >= Internal.List->size() ) Internal.List->resize( index + 1 );
	            return Internal.List->operator[]( index );
	        }
	
	        HJSON &at( const QString &key ) {
	            return operator[]( key );
	        }
	
	        const HJSON &at( const QString &key ) const {
	            return Internal.Map->at( key );
	        }
	
	        HJSON &at( unsigned index ) {
	            return operator[]( index );
	        }
	
	        const HJSON &at( unsigned index ) const {
	            return Internal.List->at( index );
	        }
	
	        int length() const {
	            if( Type == Class::Array )
	                return Internal.List->size();
	            else
	                return -1;
	        }
	
	        bool hasKey( const QString &key ) const {
	            if( Type == Class::Object )
	                return Internal.Map->find( key ) != Internal.Map->end();
	            return false;
	        }
	
	        int size() const {
	            if( Type == Class::Object )
	                return Internal.Map->size();
	            else if( Type == Class::Array )
	                return Internal.List->size();
	            else
	                return -1;
	        }
	
	        Class JSONType() const { return Type; }
	
	        /// Functions for getting primitives from the JSON object.
	        bool IsNull() const { return Type == Class::Null; }
	
	        QString ToString() const { bool b; return std::move( ToString( b ) ); }
	        QString ToString( bool &ok ) const {
	            ok = (Type == Class::String);
	            return ok ? std::move( json_escape( *Internal.String ) ): QString("");
	        }
	
	        double ToFloat() const { bool b; return ToFloat( b ); }
	        double ToFloat( bool &ok ) const {
	            ok = (Type == Class::Floating);
	            return ok ? Internal.Float : 0.0;
	        }
	
	        long ToInt() const { bool b; return ToInt( b ); }
	        long ToInt( bool &ok ) const {
	            ok = (Type == Class::Integral);
	            return ok ? Internal.Int : 0;
	        }
	
	        bool ToBool() const { bool b; return ToBool( b ); }
	        bool ToBool( bool &ok ) const {
	            ok = (Type == Class::Boolean);
	            return ok ? Internal.Bool : false;
	        }
	
	        JSONWrapper<map<QString,HJSON>> ObjectRange() {
	            if( Type == Class::Object )
	                return JSONWrapper<map<QString,HJSON>>( Internal.Map );
	            return JSONWrapper<map<QString,HJSON>>( nullptr );
	        }
	
	        JSONWrapper<deque<HJSON>> ArrayRange() {
	            if( Type == Class::Array )
	                return JSONWrapper<deque<HJSON>>( Internal.List );
	            return JSONWrapper<deque<HJSON>>( nullptr );
	        }
	
	        JSONConstWrapper<map<QString,HJSON>> ObjectRange() const {
	            if( Type == Class::Object )
	                return JSONConstWrapper<map<QString,HJSON>>( Internal.Map );
	            return JSONConstWrapper<map<QString,HJSON>>( nullptr );
	        }
	
	
	        JSONConstWrapper<deque<HJSON>> ArrayRange() const { 
	            if( Type == Class::Array )
	                return JSONConstWrapper<deque<HJSON>>( Internal.List );
	            return JSONConstWrapper<deque<HJSON>>( nullptr );
	        }
	
	        QString dump( int depth = 1, QString tab = "  ") const {
	            QString pad = "";
	            for( int i = 0; i < depth; ++i, pad += tab );
	
	            switch( Type ) {
	                case Class::Null:
	                    return "null";
	                case Class::Object: {
	                    QString s = "{\n";
	                    bool skip = true;
	                    for( auto &p : *Internal.Map ) {
	                        if( !skip ) s += ",\n";
	                        s += ( pad + "\"" + p.first + "\" : " + p.second.dump( depth + 1, tab ) );
	                        skip = false;
	                    }
	                    s += ( "\n" + pad.remove( 0, 2 ) + "}" ) ;
	                    return s;
	                }
	                case Class::Array: {
	                    QString s = "[";
	                    bool skip = true;
	                    for( auto &p : *Internal.List ) {
	                        if( !skip ) s += ", ";
	                        s += p.dump( depth + 1, tab );
	                        skip = false;
	                    }
	                    s += "]";
	                    return s;
	                }
	                case Class::String:
	                    return "\"" + json_escape( *Internal.String ) + "\"";
	                case Class::Floating:
	                    return QString::number( Internal.Float );
	                case Class::Integral:
						return QString::number(Internal.Int);
	                case Class::Boolean:
	                    return Internal.Bool ? "true" : "false";
	                default:
	                    return "";
	            }
	            return "";
	        }
			QString toJsonStr(int depth = 1, QString tab = "  ") const {
				QString pad = "";
				for (int i = 0; i < depth; ++i, pad += tab);

				switch (Type) {
				case Class::Null:
					return "null";
				case Class::Object: {
					QString s = "{";
					bool skip = true;
					for (auto &p : *Internal.Map) {
						if (!skip) s += ",";
						s += (pad + "\"" + p.first + "\" : " + p.second.toJsonStr(depth + 1, tab));
						skip = false;
					}
					s += (pad.remove(0, 2) + "}");
					return s;
				}
				case Class::Array: {
					QString s = "[";
					bool skip = true;
					for (auto &p : *Internal.List) {
						if (!skip) s += ", ";
						s += p.toJsonStr(depth + 1, tab);
						skip = false;
					}
					s += "]";
					return s;
				}
				case Class::String:
					return json_escape_2(*Internal.String);
				case Class::Floating:
					return QString::number(Internal.Float);
				case Class::Integral:
					return QString::number(Internal.Int);
				case Class::Boolean:
					return Internal.Bool ? "true" : "false";
				default:
					return "";
				}
				return "";
			}
	        friend std::ostream& operator<<( std::ostream&, const HJSON & );
	
	    private:
	        void SetType( Class type ) {
	            if( type == Type )
	                return;
	
	            ClearInternal();
	          
	            switch( type ) {
	            case Class::Null:      Internal.Map    = nullptr;                break;
	            case Class::Object:    Internal.Map    = new map<QString,HJSON>(); break;
	            case Class::Array:     Internal.List   = new deque<HJSON>();     break;
	            case Class::String:    Internal.String = new QString();           break;
	            case Class::Floating:  Internal.Float  = 0.0;                    break;
	            case Class::Integral:  Internal.Int    = 0;                      break;
	            case Class::Boolean:   Internal.Bool   = false;                  break;
	            }
	
	            Type = type;
	        }
	
	    private:
	      /* beware: only call if YOU know that Internal is allocated. No checks performed here. 
	         This function should be called in a constructed JSON just before you are going to 
	        overwrite Internal... 
	      */
	      void ClearInternal() {
	        switch( Type ) {
	          case Class::Object: delete Internal.Map;    break;
	          case Class::Array:  delete Internal.List;   break;
	          case Class::String: delete Internal.String; break;
	          default:;
	        }
	      }
	
	    private:
	
	        Class Type = Class::Null;
	};
	
	HJSON Array() {
	    return std::move( HJSON::Make( HJSON::Class::Array ) );
	}
	
	template <typename... T>
	HJSON Array( T... args ) {
	    HJSON arr = HJSON::Make( HJSON::Class::Array );
	    arr.append( args... );
	    return std::move( arr );
	}
	
	HJSON Object() {
	    return std::move( HJSON::Make( HJSON::Class::Object ) );
	}
	
	std::ostream& operator<<(std::ostream &os, const HJSON &json) {
		os << json.dump().toStdString();
		return os;
	}
	
	namespace {
	    HJSON parse_next( const QString &, size_t & );
	
	    void consume_ws( const QString &str, size_t &offset ) {
			while (isspace(str[offset].toLatin1())) ++offset;
	    }
	
	    HJSON parse_object( const QString &str, size_t &offset ) {
	        HJSON Object = HJSON::Make( HJSON::Class::Object );
	
	        ++offset;
	        consume_ws( str, offset );
	        if( str[offset] == '}' ) {
	            ++offset; return std::move( Object );
	        }
	
	        while( true ) {
	            HJSON Key = parse_next( str, offset );
	            consume_ws( str, offset );
	            if( str[offset] != ':' ) {
	                std::cerr << "Error: Object: Expected colon, found '" << str[offset] << "'\n";
	                break;
	            }
	            consume_ws( str, ++offset );
	            HJSON Value = parse_next( str, offset );
	            Object[Key.ToString()] = Value;
	            
	            consume_ws( str, offset );
	            if( str[offset] == ',' ) {
	                ++offset; continue;
	            }
	            else if( str[offset] == '}' ) {
	                ++offset; break;
	            }
	            else {
	                std::cerr << "ERROR: Object: Expected comma, found '" << str[offset] << "'\n";
	                break;
	            }
	        }
	
	        return std::move( Object );
	    }
	
	    HJSON parse_array( const QString &str, size_t &offset ) {
	        HJSON Array = HJSON::Make( HJSON::Class::Array );
	        unsigned index = 0;
	        
	        ++offset;
	        consume_ws( str, offset );
	        if( str[offset] == ']' ) {
	            ++offset; return std::move( Array );
	        }
	
	        while( true ) {
	            Array[index++] = parse_next( str, offset );
	            consume_ws( str, offset );
	
	            if( str[offset] == ',' ) {
	                ++offset; continue;
	            }
	            else if( str[offset] == ']' ) {
	                ++offset; break;
	            }
	            else {
	                std::cerr << "ERROR: Array: Expected ',' or ']', found '" << str[offset] << "'\n";
	                return std::move( HJSON::Make( HJSON::Class::Array ) );
	            }
	        }
	
	        return std::move( Array );
	    }
	
	    HJSON parse_string( const QString &str, size_t &offset ) {
	        HJSON String;
	        QString val;
			for (char c = str.at(++offset).toLatin1(); c != '\"'; c = str[++offset].toLatin1()) {
	            if( c == '\\' ) {
					switch (str[++offset].toLatin1()) {
	                case '\"': val += '\"'; break;
	                case '\\': val += '\\'; break;
	                case '/' : val += '/' ; break;
	                case 'b' : val += '\b'; break;
	                case 'f' : val += '\f'; break;
	                case 'n' : val += '\n'; break;
	                case 'r' : val += '\r'; break;
	                case 't' : val += '\t'; break;
	                case 'u' : {
	                    val += "\\u" ;
	                    for( unsigned i = 1; i <= 4; ++i ) {
							c = str[offset + i].toLatin1();
	                        if( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )
	                            val += c;
	                        else {
	                            std::cerr << "ERROR: String: Expected hex character in unicode escape, found '" << c << "'\n";
	                            return std::move( HJSON::Make( HJSON::Class::String ) );
	                        }
	                    }
	                    offset += 4;
	                } break;
	                default  : val += '\\'; break;
	                }
	            }
	            else
	                val += c;
	        }
	        ++offset;
	        String = val;
	        return std::move( String );
	    }
	
	    HJSON parse_number( const QString &str, size_t &offset ) {
	        HJSON Number;
	        QString val, exp_str;
	        char c;
	        bool isDouble = false;
	        long exp = 0;
	        while( true ) {
				c = str[offset++].toLatin1();
	            if( (c == '-') || (c >= '0' && c <= '9') )
	                val += c;
	            else if( c == '.' ) {
	                val += c; 
	                isDouble = true;
	            }
	            else
	                break;
	        }
	        if( c == 'E' || c == 'e' ) {
				c = str[offset++].toLatin1();
	            if( c == '-' ){ ++offset; exp_str += '-';}
	            while( true ) {
					c = str[offset++].toLatin1();
	                if( c >= '0' && c <= '9' )
	                    exp_str += c;
	                else if( !isspace( c ) && c != ',' && c != ']' && c != '}' ) {
	                    std::cerr << "ERROR: Number: Expected a number for exponent, found '" << c << "'\n";
	                    return std::move( HJSON::Make( HJSON::Class::Null ) );
	                }
	                else
	                    break;
	            }
	            exp = std::stol( exp_str.toStdString() );
	        }
	        else if( !isspace( c ) && c != ',' && c != ']' && c != '}' ) {
	            std::cerr << "ERROR: Number: unexpected character '" << c << "'\n";
	            return std::move( HJSON::Make( HJSON::Class::Null ) );
	        }
	        --offset;
	        
	        if( isDouble )
	            Number = std::stod( val.toStdString() ) * std::pow( 10, exp );
	        else {
	            if( !exp_str.isEmpty() )
					Number = std::stol(val.toStdString()) * std::pow(10, exp);
	            else
					Number = std::stol(val.toStdString());
	        }
	        return std::move( Number );
	    }
	
	    HJSON parse_bool( const QString &str, size_t &offset ) {
	        HJSON Bool;
	        if( str.mid( offset, 4 ) == "true" )
	            Bool = true;
			else if (str.mid(offset, 5) == "false")
	            Bool = false;
	        else {
	            std::cerr << "ERROR: Bool: Expected 'true' or 'false', found '" << str.mid( offset, 5 ) << "'\n";
	            return std::move( HJSON::Make( HJSON::Class::Null ) );
	        }
	        offset += (Bool.ToBool() ? 4 : 5);
	        return std::move( Bool );
	    }
	
	    HJSON parse_null( const QString &str, size_t &offset ) {
	        HJSON Null;
			if (str.mid(offset, 4) != "null") {
	            std::cerr << "ERROR: Null: Expected 'null', found '" << str.mid( offset, 4 ) << "'\n";
	            return std::move( HJSON::Make( HJSON::Class::Null ) );
	        }
	        offset += 4;
	        return std::move( Null );
	    }
	
	    HJSON parse_next( const QString &str, size_t &offset ) {
	        char value;
	        consume_ws( str, offset );
			value = str[offset].toLatin1();
	        switch( value ) {
	            case '[' : return std::move( parse_array( str, offset ) );
	            case '{' : return std::move( parse_object( str, offset ) );
	            case '\"': return std::move( parse_string( str, offset ) );
	            case 't' :
	            case 'f' : return std::move( parse_bool( str, offset ) );
	            case 'n' : return std::move( parse_null( str, offset ) );
	            default  : if( ( value <= '9' && value >= '0' ) || value == '-' )
	                           return std::move( parse_number( str, offset ) );
	        }
	        std::cerr << "ERROR: Parse: Unknown starting character '" << value << "'\n";
	        return HJSON();
	    }
	}
	
	HJSON HJSON::Load( const QString &str ) {
	    size_t offset = 0;
	    return std::move( parse_next( str, offset ) );
	}
	
	} // End Namespace json

}//namespace xComponent
