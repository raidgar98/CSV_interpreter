#pragma GCC error "not done, do not compile!"

#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>

#include <experimental/filesystem>

#include "sneaky_pointer.hpp"

#define THROW( throw_object, message ) try{ throw throw_object(message); }catch(const throw_object& ){ throw; }

//T class has to implement constructor with std::vector<std::string>
template<class T, typename chr = char>
class CSV final
{
private:

	using str = std::string<chr>;
	
	//1'st flag: is open
	//2'nd flag: is buffer has quote open
	//3'rd flag: is quotes are single ones or double ones
	sneaky_pointer<std::ifstream, 3> __m_input{ nullptr };
	std::vector<chr> __m_buffer;
	std::vector<str> __m_headers;

public:

	CSV() = delete;
	CSV(const CSV&) = delete;
	CSV(CSV&&) = delete;

	explicit CSV(const str& path_to_file) 
	{
		__m_input.set_flag(2, false);
		__m_input.set_flag(3, false);
		if(std::experimental::filesystem::exists(path_to_file))
		{
			__m_input.set_pointer(new std::ifstream(path_to_file));
			if( ! __m_input->good() )
			{
				__m_input->close();
				__m_input.set_pointer( nullptr );
				__m_input.set_flag( 1, false );
			}
			else
			{
				__m_input.set_flag( 1, true );
				str raw_header = get_line();
				str result{ next_word(raw_header) };
				while( result != str::npos )
					__m_headers.emplace_back(result));
			}
			
		}
	}

	~CSV()
	{
		if( __m_input.get_pointer() != nullptr )
		{
			if( __m_input->is_open() )
				__m_input->close();
			__m_input.set_pointer( nullptr );
		}
	}

	std::shared_ptr<T> next()
	{
		if( ! __m_input.get_flag(1) )
			THROW( std::bad_function_call, "file not open" );
		

	}

	std::shared_ptr<T> operator()()
	{
		return next();
	}

	std::map<const str, str> next_line_map();
	std::vector<str> next_line_vec();

private: //methodes (not safe)

	str get_line()
	{
		//TODO: implement recognize quotes 
		str ret{ "" };
		for( const chr& var : __m_buffer )
			if( __m_input.get_flag(2) )
			{
				ret += var;
				continue;
			}
			if(var == '\'' || var == '\"')
			{

			}
			if(var != '\n' && var != '\r')
				ret += var;
			else
				if(ret.length() == 0)
					return str::npos;
				else
					return ret;
		
		constexpr size_t buff_size = 10;
		std::unique_ptr<chr[]> buff{ new chr[ sizeof(chr) * buff_size ]};
		bool endline_appear = false;
		do
		{
			__m_buffer.read(buff.get(), buff_size);
			for( int i = 0; i < buff_size; i++ )
				if()

		}while( ! endline_appear );
	}

	str next_word(str& src) const;

};