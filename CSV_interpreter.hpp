#pragma GCC error "not done, do not compile!"

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <functional>
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
	constexpr size_t buff_size = 10;
	
	//1'st flag: is open
	//2'nd flag: is buffer has quote open
	//3'rd flag: is quotes are single ones (true) or double ones (false)
	sneaky_pointer<std::ifstream, 3> __m_input{ nullptr };
	std::list<chr> __m_buffer;	//list becauso of often adding and ereasing objects
	std::vector<str> __m_headers;	//only once resized

	const chr __m_delimiter;

public:

	CSV() = delete;
	CSV(const CSV&) = delete;
	CSV(CSV&&) = delete;

	//i_delimiter - if NULL (default) delimiter will be autodetected from [ | ; : , ]
	explicit CSV(const str& path_to_file, const chr i_delimiter = '\0') 
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
				{
					__m_headers.emplace_back(result));
					result = next_word( raw_header, i_delimiter != '\0');
				}
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
		
		str line = get_line();
		std::vector<str> args;
		args.reserve(__m_headers.size());

		for(int i = 0; i < __m_headers.size(); i++)
			args.emplace_back(next_word(line));
		
		return std::shared_ptr<T>{ new T{ args } };
	}

	std::shared_ptr<T> operator()()
	{
		return next();
	}

	std::map<const str, str> next_line_map();
	std::vector<str> next_line_vec();

private: //methodes

	str get_line() noexcept
	{
		str ret{ "" };
		std::unique_ptr<chr[]> buff{ new chr[ sizeof(chr) * buff_size ]};
		do
		{
			get_from_buffor(ret);
			if(ret.find("\n") != -1 || ret.find("\r" != -1))
				return ret;
			
			__m_buffer.read(buff.get(), buff_size);
			for( int i = 0; i < buff_size; i++ )
				__m_buffer.push_back(buff[i]);

		}while( true && ! __m_input->eof() );
	}

	str next_word(str& src, const bool initial_divide = false) const noexcept
	{
		if(src.length() == 0) return src;
		size_t idx = 0;
		str res{ "" };

		if(src[idx] == '\n' || src[idx] == '\r')
		{
			chr tmp = *(src.begin());
			src.erease(src.begin());
			return tmp;
		}

		const std::array<chr, 7> supported_delimiters = 
		{
			static_cast<chr>('|'), 
			static_cast<chr>(':'), 
			static_cast<chr>(';'), 
			static_cast<chr>(','), 
			static_cast<chr>('\t'), 
			static_cast<chr>('\n'), 
			static_cast<chr>('\r') 
		};

		const auto check_delimiter = [&](const chr& c) { 
			for(const auto& var : supported_delimiters)
				if(var == c) return true;
			return false;
		};
		bool quote_open = false;
		bool single_quote = false;
		auto it = src.begin();
		for(; it != src.end(); it++)
		{
			if(*it == '\'')
			{
				if(quote_open)
				{
					if(single_quote)
						quote_open = false;
					else
						res += *it;
				}
				else
				{
					quote_open = true;
					single_quote = true;
				}
				continue;
			}
			else if(*it == '"')
			{
				if(quote_open)
				{
					if( !single_quote)
						quote_open = false;
					else
						res += *it;
				}
				else
				{
					quote_open = true;
					quote_open = false;
				}
				continue;
			}
			else if( (initial_divide ? false : *it == __m_delimiter) || check_delimiter(*it) )
			{
				if( !initial_divide ) __m_delimiter = *it;						
				break;
			}else res += *it;
		}
		src.erease(src.begin(), it);
		return res;
	}

	void get_from_buffor(str& ret) noexcept
	{
		for( int i = 0; i < __m_buffer.size(); i++ )
		{
			if( __m_input.get_flag(2) )	//If quotes open
			{
				if( ( var == '\''	&& __m_input.get_flag(3)	) ||
					( var == '"'	&& ! __m_input.get_flag(3)	) )
					__m_input.set_flag(2, false);
				else 
				{
					ret += *(__m_buffer.begin());
					__m_buffer.pop_front()
				}
				continue;
			}
			else	//If quotes close
			{
				if( var == '\'' )
				{
					__m_input.set_flag(3, true);
					__m_input.set_flag(2, true);
				}
				else if( var == '"')
				{
					__m_input.set_flag(3, false);
					__m_input.set_flag(2, true);
				}
				continue;
			}
			
			if(var != '\n' && var != '\r')
			{
				ret += *(__m_buffer.begin());
				__m_buffer.pop_front()
			}
			else
				return;
		}
	}

};