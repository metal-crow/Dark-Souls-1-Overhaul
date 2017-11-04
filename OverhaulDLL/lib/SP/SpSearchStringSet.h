/*

	Author:
		Sean Pesce


	SearchStringSet.h

	This class implements the SeQan library for efficient string searching. More info here:
	    https://www.seqan.de/

*/

#pragma once

#ifndef _SP_SEARCH_STRING_SET_H_
	#define _SP_SEARCH_STRING_SET_H_


#include <Windows.h>

// Fixes for compatibility issues between SeQan and Windows API
#ifdef min
	#undef min
#endif // min
#ifdef max
	#undef max
#endif // max

#include <string>
#include <list>

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4521 4800 4244 4996 4503) // Disable "multiple copy constructors", "data loss", and "name truncated" warnings from SeQan library
#endif // _MSC_VER
		#include "seqan/index.h" // String, StringSet, Index, Finder
#ifdef _MSC_VER
	#pragma warning( pop )
#endif // _MSC_VER



/*
	        //////// Enums ////////
*/

// String end options
enum SSS_SIDE {
	SSS_LEFT  = 0,
	SSS_RIGHT = 1
};

// Letter case options for new strings
enum SSS_PREFER_LETTER_CASE {
	SSS_NO_CASE,
	SSS_UPPERCASE,
	SSS_LOWERCASE
};


// Whether or not letter case is considered when determining if two strings are distinct
enum SSS_CASE_DISTINCTION {
	SSS_CASE_DISTINCTION_DEFAULT,
	SSS_CASE_SENSITIVE,
	SSS_CASE_INSENSITIVE
};


// Options for action to be taken when attempting to insert an invalid string
enum SSS_INVALID_INSERT_ACTION {
	SSS_NO_ACTION,
	SSS_REJECT,
	SSS_CONFORM
};


// String search matching strictness. (Note: All options other than SSS_DEFAULT and SSS_NOT_EXACT return exact matches)
enum SSS_MATCH_STRICTNESS {
	SSS_DEFAULT,      // Uses the default search strictness for the object, which is one of the options below
	SSS_EXACT,        // Returns exact matches only
	SSS_STARTS_WITH,  // Returns strings that start with the search string
	SSS_ENDS_WITH,    // Returns strings that end with the search string
	SSS_NOT_EXACT,    // Returns any string that contains, but is not equivalent to, the search string
	SSS_SUPER_STRING  // Returns any string that contains the search string
};


/*
	        //////// Types ////////
*/
template <typename T>
struct SSS_CONSTRAINT {
	T value; // Value to compare against to determine if constraint is violated
	SSS_INVALID_INSERT_ACTION act;  // Action to take if constraint is violated
};




template <typename charT>
class SpSearchStringSet {
private:
	seqan::StringSet<seqan::String<charT>> _set;
	seqan::Index<seqan::StringSet<seqan::String<charT>>> *_search_index = NULL;

	seqan::StringSet<seqan::String<charT>> _set_case_insensitive; // @TODO: Figure out better way to do this (method that uses less memory)
	seqan::Index<seqan::StringSet<seqan::String<charT>>> *_search_index_case_insensitive = NULL;

	// Signifies whether the search index is accurate for the current string set
	bool _outdated_index = true;

	// Determines which search strictness setting will be used if one is not specified
	SSS_MATCH_STRICTNESS _default_search_strictness = SSS_EXACT;

	// Returns whether the given string exists in the set. NOTE: Does not use indexing for search,
	//    so this function can be extremely slow on larger data sets. Also, this function only
	//    searches for exact matches.
	int _index_of_exact(std::basic_string<charT>& string);

public:
	
	/*
	            //////// Constructors & Destructors ////////
	*/

	// Default constructor (empty set)
	SpSearchStringSet<charT>() { this->build_index(); }

	// Destructor
	~SpSearchStringSet<charT>()
	{
		if (_search_index != NULL)
		{
			delete _search_index;
			_search_index = NULL;
		}
	}



	/*
	            //////// Settings variables ////////


	    Variables that affect the behavior of the string set
		when certain actions are performed, such as:
		    -Inserting new strings
			-Searching the set for a string
	*/


	/* 
		trim[LEFT], trim[RIGHT]
		
	   Determine whether the string set should remove whitespace
	     (and other trimmable characters) from the start and end
		 of newly added strings
	*/
	bool trim[2] = {
		             true,  // Trim left  side?
		             true   // Trim right side?
	               };


	// Characters to trim
	std::basic_string<charT> trim_mask = std::basic_string<charT>({ (charT)' ', (charT)'\t', (charT)'\r', (charT)'\n' }); // " \t\r\n"


	// Invalid characters
	SSS_CONSTRAINT<std::basic_string<charT>> invalid_chars = { std::basic_string<charT>() , SSS_CONFORM };


	// Preferred letter case
	SSS_CONSTRAINT<SSS_PREFER_LETTER_CASE> prefer_case = { SSS_NO_CASE, SSS_CONFORM };


	// Maximum number of characters
	SSS_CONSTRAINT<size_t> max_length = { FILENAME_MAX, SSS_REJECT };

	// Minimum number of characters
	SSS_CONSTRAINT<size_t> min_length = {            0, SSS_REJECT };

	// Character sequence to use as padding if string is less than min_length and action == CONFORM
	std::basic_string<charT> min_length_pad = std::basic_string<charT>({ (charT)'_' }); // "_"
	// Side to pad on if string is too short and action == CONFORM
	SSS_SIDE min_length_pad_side = SSS_RIGHT;


	// Substrings to replace (replace each instance of the first pair element with an instance of the second element)
	std::list<std::pair<std::basic_string<charT>, std::basic_string<charT>>> replacements;

	// Whether or not letter case is considered when determining if two strings are distinct
	SSS_CASE_DISTINCTION case_distinction = SSS_CASE_SENSITIVE;

	// Determines if each string must be unique
	bool distinct_set = true;







	/*
	            //////// Member Functions ////////
	*/



	/*
		Appends a new string to the set.
	*/
	int append(std::basic_string<charT>& new_string, bool rebuild_index = true);
	int append(const charT* new_string, bool rebuild_index = true);
	int append(charT* new_string, bool rebuild_index = true);


	/*
	    Searches the set for a string and stores matching results in the given container object.

		If limit is set to 0, the function will return all matching strings.

		Returns the number of results.
	*/
	template <class storageT>
	int search(std::basic_string<charT>& string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	template <class storageT>
	int search(const charT* string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	template <class storageT>
	int search(charT* string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);


	/*
		Searches the set for a string and returns the first matching result.

		If no results are found, returns empty string ("") and error code is set to ERROR_FILE_NOT_FOUND.
	*/
	const char* quick_search(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	const char* quick_search(const charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	const char* quick_search(charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);

	/*
	    Checks if the set contains a string, and if so, returns the first index of a
		matching string in the set.

		If the string does not exist in the set, returns -1.
	*/
	int index_of(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	int index_of(const charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	int index_of(charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);


	// Returns whether the set contains a string
	bool contains(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	bool contains(const charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);
	bool contains(charT* string, SSS_MATCH_STRICTNESS strictness = SSS_DEFAULT);


	/*
	    Returns the string at the specified index in the set, or an empty string if
		an invalid index was specified.

		If index is invalid, error code is set to ERROR_RANGE_NOT_FOUND
	*/
	const charT* get(unsigned int index);

	// Returns the number of strings in the set
	inline size_t size();

	// Clears the string set (removes all strings, leaving an empty set)
	inline void clear();

	// (Re-)Initialize the set index structure
	inline void build_index(bool force_rebuild = false);

	// Returns default search strictness
	inline SSS_MATCH_STRICTNESS get_default_search_strictness();

	// Modifies default search strictness. Returns 0 on success, otherwise returns relevant error code
	inline int set_default_search_strictness(SSS_MATCH_STRICTNESS strictness);

	// Returns whether the search index is up-to-date
	inline bool index_is_up_to_date();




	/*
	            //////// Static Class Functions ////////
	*/

	// Trim input string by removing leading and/or trailing characters if they exist in the mask string
	static int trim_edges(std::basic_string<charT>& string, std::basic_string<charT>& mask = std::basic_string<charT>({ (charT)' ', (charT)'\t', (charT)'\r', (charT)'\n' }) /* " \t\r\n" */, bool trim_left = true, bool trim_right = true);

	// Convert all alphabetical characters in a string to lowercase
	static int to_lowercase(std::basic_string<charT>& string);

	// Convert all alphabetical characters in a string to uppercase
	static int to_uppercase(std::basic_string<charT>& string);

	// Returns true if the string contains characters from the invalid character set (otherwise, returns false)
	static inline bool contains_invalid_chars(std::basic_string<charT>& string, std::basic_string<charT>& invalid_chars);

	// Remove characters from a string if they appear in the set of invalid characters
	static int remove_invalid_chars(std::basic_string<charT>& string, std::basic_string<charT>& invalid_chars);

	// Replaces all instances of one substring with all instances of another substring in the specified super-string
	static void replace(std::basic_string<charT>& string, std::basic_string<charT>& find, std::basic_string<charT>& replace_with);
};



typedef SpSearchStringSet<char>    SpSearchStringSetA;
typedef SpSearchStringSet<wchar_t> SpSearchStringSetW;














////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// FUNCTION TEMPLATES ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 
template <typename charT>
int SpSearchStringSet<charT>::_index_of_exact(std::basic_string<charT>& string)
{
	if (case_distinction == SSS_CASE_INSENSITIVE)
	{
		std::basic_string<charT> string_case_insensitive = string;
		to_lowercase(string_case_insensitive);
		int size = seqan::length(_set_case_insensitive);
		for (int i = 0; i < size; i++)
		{
			if (string_case_insensitive == std::basic_string<charT>(seqan::toCString(_set_case_insensitive[i])))
				return i;
		}
	}
	else
	{
		int size = seqan::length(_set);
		for (int i = 0; i < size; i++)
		{
			if (string == std::basic_string<charT>(seqan::toCString(_set[i])))
				return i;
		}
	}
	return -1;
}


// Append a new string to the set
template <typename charT>
int SpSearchStringSet<charT>::append(std::basic_string<charT>& new_string, bool rebuild_index)
{
	std::basic_string<charT> string = std::basic_string<charT>(new_string);

	// Trim string
	trim_edges(string, trim_mask, trim[SSS_LEFT], trim[SSS_RIGHT]);

	// Check for preferred letter case
	if (prefer_case.value == SSS_LOWERCASE)
	{
		if(prefer_case.act == SSS_REJECT)
			return ERROR_ILLEGAL_CHARACTER;
		else if(prefer_case.act == SSS_CONFORM)
			to_lowercase(string);
	}
	else if (prefer_case.value == SSS_UPPERCASE)
	{
		if (prefer_case.act == SSS_REJECT)
			return ERROR_ILLEGAL_CHARACTER;
		else if (prefer_case.act == SSS_CONFORM)
			to_uppercase(string);
	}

	// Handle invalid characters
	if (invalid_chars.act == SSS_REJECT && contains_invalid_chars(string, invalid_chars.value))
		return ERROR_ILLEGAL_CHARACTER;
	else if (invalid_chars.act == SSS_CONFORM)
		remove_invalid_chars(string, invalid_chars.value);


	// String is too short
	if (string.length() < min_length.value)
	{
		if (min_length.act == SSS_REJECT)
			return ERROR_SHORT_NAMES_NOT_ENABLED_ON_VOLUME;
		else if (min_length.act == SSS_CONFORM)
		{
			while (string.length() < min_length.value)
			{
				if (min_length_pad_side == SSS_LEFT)
					string.insert(0, min_length_pad); // Pad string on the left
				else
					string.append(min_length_pad);    // Pad string on the right
			}
		}
				
	}

	// String is too long
	if (string.length() > max_length.value)
	{
		if (max_length.act == SSS_REJECT)
			return ERROR_LABEL_TOO_LONG;
		else if (max_length.act == SSS_CONFORM)
			string = string.substr(0, max_length.value); // Shorten the string
	}


	// Perform replacements
	for (std::pair<std::basic_string<charT>, std::basic_string<charT>> pair : replacements)
	{
		replace(string, pair.first, pair.second);
	}


	// Check for uniqueness
	if (distinct_set)
	{
		if((_outdated_index && _index_of_exact(string) != -1) // Search index is outdated; use slow search
			 || (!_outdated_index && contains(string, SSS_EXACT)))     // Search index is up-to-date; use fast search
			return ERROR_DUP_NAME;
	}


	// New string is valid; insert into set
	seqan::appendValue(_set, string);

	// Also create case-insensitive version
	std::basic_string<charT> string_case_insensitive = string;
	to_lowercase(string_case_insensitive);
	seqan::appendValue(_set_case_insensitive, string_case_insensitive);

	if (rebuild_index)
		build_index(); // Rebuild index
	else
		_outdated_index = true; // Mark index as outdated

	return ERROR_SUCCESS;
}



// Searches the set for a string and stores matching results in the given container object
template <typename charT>
template <class storageT>
int SpSearchStringSet<charT>::search(std::basic_string<charT>& string, storageT& container, size_t limit, SSS_MATCH_STRICTNESS strictness)
{
	// Rebuild index if it's outdated
	if (_outdated_index)
		build_index();

	size_t count = 0;
	std::vector<int> found_indices; // Indices of matching strings that were already found (to avoid duplicate results)
	if (strictness == SSS_DEFAULT)
		strictness = _default_search_strictness;

	std::basic_string<charT> search_string = string;
	seqan::Finder<seqan::Index<seqan::StringSet<seqan::String<charT>>>> string_finder;
	if (case_distinction == SSS_CASE_INSENSITIVE)
	{
		seqan::setHaystack(string_finder, *_search_index_case_insensitive);
		to_lowercase(search_string);
	}
	else
	{
		seqan::setHaystack(string_finder, *_search_index);
	}

	seqan::String<charT> search(search_string);

	while ((limit == 0 || count < limit) && seqan::find(string_finder, search))
	{
		if (std::find(found_indices.begin(), found_indices.end(), seqan::position(string_finder).i1) == found_indices.end())
		{
			switch (strictness)
			{
				case SSS_SUPER_STRING: // Accept any superstring of the search string
					container.insert(container.end(), std::basic_string<charT>(seqan::toCString(_set[seqan::position(string_finder).i1])));
					found_indices.push_back(seqan::position(string_finder).i1);
					count++;
					break;

				case SSS_NOT_EXACT: // Accept any superstring of the search string that is not equivalent to the search string
					if (!(seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) == seqan::length(search)))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(_set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_STARTS_WITH: // Accept superstrings where search string starts at character index 0
					if (seqan::position(string_finder).i2 == 0)
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(_set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_ENDS_WITH: // Accept superstrings where search string is at the end of the superstring
					//                                 i2 = starting character position in the matching string.              i1 = index in the "haystack" (set)
					if (seqan::position(string_finder).i2 == (seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) - seqan::length(search)))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(_set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_EXACT: // Only accept exact matches
				default:
					if (seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) == seqan::length(search))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(_set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;
			}
		}
	}
	return count;
}



// Searches the set for a string and returns the first matching result
template <typename charT>
const char* SpSearchStringSet<charT>::quick_search(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness)
{
	static std::basic_string<charT> last_result; // Stores the last result until a new quicksearch is performed
	last_result.clear(); // Clear last result

	if (strictness == SSS_DEFAULT)
		strictness = _default_search_strictness;

	std::vector<std::basic_string<charT>> temp_result;
	size_t result_count = this->search(string, temp_result, 1, strictness);

	if (result_count == 0) // No results
		SetLastError(ERROR_FILE_NOT_FOUND);
	else
	{
		last_result = std::basic_string<charT>(temp_result.at(0).c_str());
		SetLastError(ERROR_SUCCESS); // Clear error code
	}

	return last_result.c_str();
}





// Check if the set contains a string, and if so, return the index of the string in the set (otherwise return -1)
template <typename charT>
int SpSearchStringSet<charT>::index_of(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness)
{
	if (strictness == SSS_DEFAULT)
		strictness = _default_search_strictness;

	// Rebuild index if it's outdated
	if (_outdated_index)
		build_index();

	std::basic_string<charT> search_string = string;
	seqan::Finder<seqan::Index<seqan::StringSet<seqan::String<charT>>>> string_finder;
	if (case_distinction == SSS_CASE_INSENSITIVE)
	{
		seqan::setHaystack(string_finder, *_search_index_case_insensitive);
		to_lowercase(search_string);
	}
	else
	{
		seqan::setHaystack(string_finder, *_search_index);
	}

	seqan::String<charT> search(search_string);
	

	while (seqan::find(string_finder, search))
	{
		switch (strictness)
		{
			case SSS_SUPER_STRING: // Accept any superstring of the search string
				return seqan::position(string_finder).i1;
				break;

			case SSS_NOT_EXACT: // Accept any superstring of the search string that is not equivalent to the search string
				if (!(seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) == seqan::length(search)))
					return seqan::position(string_finder).i1;
				break;

			case SSS_STARTS_WITH: // Accept superstrings where search string starts at character index 0
				if (seqan::position(string_finder).i2 == 0)
					return seqan::position(string_finder).i1;
				break;

			case SSS_ENDS_WITH: // Accept superstrings where search string is at the end of the superstring
				if (seqan::position(string_finder).i2 == (seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) - seqan::length(search)) )
					return seqan::position(string_finder).i1;
				break;

			case SSS_EXACT: // Only accept exact matches
			default:
				//                                 i2 = starting character position in the matching string.                   i1 = index in the "haystack" (set)
				if (seqan::length(seqan::value(_set, seqan::position(string_finder).i1)) == seqan::length(search))
					return seqan::position(string_finder).i1;
				break;
		}
	}
	return -1;
}


// Check if the set contains a string
template <typename charT>
bool SpSearchStringSet<charT>::contains(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness)
{
	if (strictness == SSS_DEFAULT)
		strictness = _default_search_strictness;

	return (index_of(string, strictness) != -1);
}


// Returns the string at the specified index in the set, or an empty string (with error code set) if invalid index was specified
template <typename charT>
const charT* SpSearchStringSet<charT>::get(unsigned int index)
{
	static const charT NOT_FOUND = (charT)0; // Return pointer to this empty string if no result was found

	if (index >= seqan::length(_set)) // Specified index is out of bounds
	{
		SetLastError(ERROR_RANGE_NOT_FOUND);
		return &NOT_FOUND;
	}

	SetLastError(ERROR_SUCCESS);
	return (const charT*)(seqan::toCString(_set[index]));
}


// Returns the number of strings in the set
template <typename charT>
inline size_t SpSearchStringSet<charT>::size()
{
	return seqan::length(_set);
}


// Clears the string set (removes all strings, leaving an empty set)
template <typename charT>
inline void SpSearchStringSet<charT>::clear()
{
	seqan::clear(_set);
	seqan::clear(_set_case_insensitive);
	_outdated_index = true;
	build_index(true);
}


// (Re-)Initialize the set index structure
template <typename charT>
inline void SpSearchStringSet<charT>::build_index(bool force_rebuild)
{
	if (_search_index == NULL || _outdated_index)
	{
		if (_search_index != NULL)
		{
			delete _search_index;
			_search_index = NULL;
			delete _search_index_case_insensitive;
			_search_index_case_insensitive = NULL;
		}
		_search_index = new seqan::Index<seqan::StringSet<seqan::String<charT>>>(_set);
		_search_index_case_insensitive = new seqan::Index<seqan::StringSet<seqan::String<charT>>>(_set_case_insensitive);
		_outdated_index = false;
	}
}


// Returns default search strictness
template <typename charT>
inline SSS_MATCH_STRICTNESS SpSearchStringSet<charT>::get_default_search_strictness()
{
	return _default_search_strictness;
}

// Modifies default search strictness. Returns 0 on success, otherwise returns relevant error code
template <typename charT>
inline int SpSearchStringSet<charT>::set_default_search_strictness(SSS_MATCH_STRICTNESS strictness)
{
	if (strictness != SSS_DEFAULT)
	{
		_default_search_strictness = strictness;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_INVALID_PARAMETER;
	}
}

// Returns whether the search index is up-to-date
template <typename charT>
inline bool SpSearchStringSet<charT>::index_is_up_to_date()
{
	return !_outdated_index;
}





//////// Static Class Functions ////////




// Trim input string by removing leading and/or trailing characters from the mask string
template <typename charT>
int SpSearchStringSet<charT>::trim_edges(std::basic_string<charT>& string, std::basic_string<charT>& mask, bool trim_left, bool trim_right)
{
	if (mask.length() == 0)
		return ERROR_SUCCESS;

	// Remove leading mask characters
	if (trim_left)
	{
		int start = string.find_first_not_of(mask.c_str());
		string.erase(0, start);
	}

	// Remove trailing mask characters
	while (trim_right && mask.find(*(string.end() - 1)) != std::basic_string<charT>::npos)
	{
		string.erase(string.length() - 1, 1);
	}

	return ERROR_SUCCESS;
}


// Convert all characters in a string to lowercase
template <typename charT>
int SpSearchStringSet<charT>::to_lowercase(std::basic_string<charT>& string)
{
	unsigned int index = 0;
	while (string[index] != (charT)0) // While not '\0' (end of string)
	{
		if (string[index] >= (charT)'A' && string[index] <= (charT)'Z')
		{
			string[index] -= ((charT)'A' - (charT)'a');
		}
		index++;
	}
	return ERROR_SUCCESS;
}


// Convert all characters in a string to uppercase
template <typename charT>
int SpSearchStringSet<charT>::to_uppercase(std::basic_string<charT>& string)
{
	unsigned int index = 0;
	while (string[index] != (charT)0) // While not '\0' (end of string)
	{
		if (string[index] >= (charT)'a' && string[index] <= (charT)'z')
		{
			string[index] += ((charT)'A' - (charT)'a');
		}
		index++;
	}
	return ERROR_SUCCESS;
}


// Returns true if the string contains characters from the invalid character set (otherwise, returns false)
template <typename charT>
inline bool SpSearchStringSet<charT>::contains_invalid_chars(std::basic_string<charT>& string, std::basic_string<charT>& invalid_chars)
{
	return (string.find_first_of(invalid_chars) != std::basic_string<charT>::npos);
}


// Remove characters from a string if they appear in the set of invalid characters
template <typename charT>
int SpSearchStringSet<charT>::remove_invalid_chars(std::basic_string<charT>& string, std::basic_string<charT>& invalid_chars)
{
	if(invalid_chars.length() == 0)
		return ERROR_SUCCESS;

	int index = 0, invalid_start = -1;
	while (string[index] != (charT)0) // While not '\0' (end of string)
	{
		if (invalid_chars.find(string[index]) != std::basic_string<charT>::npos)
		{
			if (invalid_start != -1)
				invalid_start = index;
		}
		else if(invalid_start > -1)
		{
			string.erase(invalid_start, (index - invalid_start));
			invalid_start = -1;
		}
		index++;
	}
	return ERROR_SUCCESS;
}


// Replaces all instances of one substring with all instances of another substring
template <typename charT>
void SpSearchStringSet<charT>::replace(std::basic_string<charT>& string, std::basic_string<charT>& find, std::basic_string<charT>& replace_with)
{
	size_t pos = 0;
	if (find.length() > 0 && find != replace_with)
	{
		while ((pos = string.find(find)) != std::basic_string<charT>::npos)
		{
			string.replace(pos, find.length(), replace_with);
		}
	}
}












/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// FUNCTION TEMPLATE OVERLOADS ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Overload of SpSearchStringSet::append(std::basic_string<charT>&)
template <typename charT>
int SpSearchStringSet<charT>::append(const charT* new_string, bool rebuild_index)
{
	if (new_string == NULL)
		return ERROR_INVALID_ADDRESS;

	return append(std::basic_string<charT>(new_string), rebuild_index);
}

// Overload of SpSearchStringSet::append(std::basic_string<charT>&)
template <typename charT>
int SpSearchStringSet<charT>::append(charT* new_string, bool rebuild_index)
{
	if (new_string == NULL)
		return ERROR_INVALID_ADDRESS;

	return append(std::basic_string<charT>(new_string), rebuild_index);
}

// Overload of SpSearchStringSet::index_of(std::basic_string<charT>&)
template <typename charT>
int SpSearchStringSet<charT>::index_of(const charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return -1;
	}

	return index_of(std::basic_string<charT>(string), strictness);
}

// Overload of SpSearchStringSet::index_of(std::basic_string<charT>&)
template <typename charT>
int SpSearchStringSet<charT>::index_of(charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return -1;
	}

	return index_of(std::basic_string<charT>(string), strictness);
}


// Overload of SpSearchStringSet::contains(std::basic_string<charT>&)
template <typename charT>
bool SpSearchStringSet<charT>::contains(const charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return false;
	}

	return contains(std::basic_string<charT>(string), strictness);
}

// Overload of SpSearchStringSet::contains(std::basic_string<charT>&)
template <typename charT>
bool SpSearchStringSet<charT>::contains(charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return false;
	}

	return contains(std::basic_string<charT>(string), strictness);
}

// Overload of SpSearchStringSet::search(std::basic_string<charT>&, ...)
template <typename charT>
template <class storageT>
int SpSearchStringSet<charT>::search(const charT* string, storageT& container, size_t limit, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return 0;
	}

	return search(std::basic_string<charT>(string), container, limit, strictness);
}

// Overload of SpSearchStringSet::search(std::basic_string<charT>&, ...)
template <typename charT>
template <class storageT>
int SpSearchStringSet<charT>::search(charT* string, storageT& container, size_t limit, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return 0;
	}

	return search(std::basic_string<charT>(string), container, limit, strictness);
}

// Overload of SpSearchStringSet::quick_search(std::basic_string<charT>&, ...)
template <typename charT>
const char* SpSearchStringSet<charT>::quick_search(const charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return 0;
	}

	return quick_search(std::basic_string<charT>(string), strictness);
}

// Overload of SpSearchStringSet::quick_search(std::basic_string<charT>&, ...)
template <typename charT>
const char* SpSearchStringSet<charT>::quick_search(charT* string, SSS_MATCH_STRICTNESS strictness)
{
	if (string == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return 0;
	}

	return quick_search(std::basic_string<charT>(string), strictness);
}









#endif // _SP_SEARCH_STRING_SET_H_