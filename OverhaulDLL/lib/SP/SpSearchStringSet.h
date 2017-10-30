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



template <typename charT>
class SpSearchStringSet {
private:
	seqan::StringSet<seqan::String<charT>> set;
	seqan::Index<seqan::StringSet<seqan::String<charT>>> *index = NULL;

public:


	/*
	            //////// Enums ////////
	*/

	// String end options
	enum SSS_SIDE {
		SSS_LEFT  = 0,
		SSS_RIGHT = 1
	};

	// Letter case options
	enum SSS_LETTER_CASE {
		SSS_NO_CASE,
		SSS_UPPERCASE,
		SSS_LOWERCASE
	};


	// Options for action to be taken when attempting to insert an invalid string
	enum SSS_INVALID_INSERT_ACTION {
		SSS_NO_ACTION,
		SSS_REJECT,
		SSS_CONFORM
	};


	// String search matching strictness. (Note: All options other than SSS_NOT_EXACT return exact matches)
	enum SSS_MATCH_STRICTNESS {
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




	
	/*
	            //////// Constructors & Destructors ////////
	*/

	// Default constructor (empty set)
	SpSearchStringSet<charT>() { this->initialize_index(); }

	// Destructor
	~SpSearchStringSet<charT>()
	{
		if (index != NULL)
		{
			delete index;
			index = NULL;
		}
	}



	/*


	            //////// Settings variables ////////


	    Variables that affect the behavior of the string set
		when new strings are inserted.

	*/


	/* 
		trim[LEFT], trim[RIGHT]
		
	   Determine whether the string set should remove whitespace
	     (and other trimmable characters) from the start and end
		 of newly added strings
	*/
	//                 
	bool trim[2] = {
		             true,  // Trim left  side?
		             true   // Trim right side?
	               };


	// Characters to trim
	std::basic_string<charT> trim_mask = std::basic_string<charT>({ (charT)' ', (charT)'\t', (charT)'\r', (charT)'\n' }); // " \t\r\n"


	// Invalid characters
	SSS_CONSTRAINT<std::basic_string<charT>> invalid_chars = { std::basic_string<charT>() , SSS_CONFORM };


	// Preferred letter case
	SSS_CONSTRAINT<SSS_LETTER_CASE> prefer_case = { SSS_NO_CASE, SSS_CONFORM };


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


	// Determines if each string must be unique
	bool distinct_set = true;







	/*


	            //////// Member Functions ////////


	    

	*/



	/*
		Appends a new string to the set.
	*/
	int append(std::basic_string<charT>& new_string);
	int append(const charT* new_string);
	int append(charT* new_string);


	/*
	    Searches the set for a string and stores matching results in the given container object.

		Returns the number of results.
	*/
	template <class storageT>
	int search(std::basic_string<charT>& string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	template <class storageT>
	int search(const charT* string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	template <class storageT>
	int search(charT* string, storageT& container, size_t limit = 1, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);

	/*
	    Checks if the set contains a string, and if so, returns the first index of a
		matching string in the set.

		If the string does not exist in the set, returns -1.
	*/
	int index_of(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	int index_of(const charT* string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	int index_of(charT* string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);


	// Returns whether the set contains a string
	bool contains(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	bool contains(const charT* string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);
	bool contains(charT* string, SSS_MATCH_STRICTNESS strictness = SSS_EXACT);


	/*
	    Returns the string at the specified index in the set, or an empty string if
		an invalid index was specified.

		If index is invalid, error code is set to ERROR_RANGE_NOT_FOUND
	*/
	const charT* get(unsigned int index);

	// Returns the number of strings in the set
	inline size_t size();

	// (Re-)Initialize the set index structure
	inline void initialize_index();



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



// Append a new string to the set
template <typename charT>
int SpSearchStringSet<charT>::append(std::basic_string<charT>& new_string)
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
	if (distinct_set && (contains(string)))
		return ERROR_DUP_NAME;


	// New string is valid; insert into set
	seqan::appendValue(set, string);
	initialize_index(); // Rebuild index

	return ERROR_SUCCESS;
}



// Searches the set for a string and stores matching results in the given container object
template <typename charT>
template <class storageT>
int SpSearchStringSet<charT>::search(std::basic_string<charT>& string, storageT& container, size_t limit, SSS_MATCH_STRICTNESS strictness)
{
	size_t count = 0;
	std::vector<int> found_indices; // Indices of matching strings that were already found (to avoid duplicate results)

	seqan::String<charT> search(string.c_str());

	seqan::Finder<seqan::Index<seqan::StringSet<seqan::String<charT>>>> string_finder;

	seqan::setHaystack(string_finder, *index);

	while (count < limit && seqan::find(string_finder, search))
	{
		if (std::find(found_indices.begin(), found_indices.end(), seqan::position(string_finder).i1) == found_indices.end())
		{
			switch (strictness)
			{
				case SSS_SUPER_STRING: // Accept any superstring of the search string
					container.insert(container.end(), std::basic_string<charT>(seqan::toCString(set[seqan::position(string_finder).i1])));
					found_indices.push_back(seqan::position(string_finder).i1);
					count++;
					break;

				case SSS_NOT_EXACT: // Accept any superstring of the search string that is not equivalent to the search string
					if (!(seqan::length(seqan::value(set, seqan::position(string_finder).i1)) == seqan::length(search)))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_STARTS_WITH: // Accept superstrings where search string starts at character index 0
					if (seqan::position(string_finder).i2 == 0)
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_ENDS_WITH: // Accept superstrings where search string is at the end of the superstring
					//                                 i2 = starting character position in the matching string.              i1 = index in the "haystack" (set)
					if (seqan::position(string_finder).i2 == (seqan::length(seqan::value(set, seqan::position(string_finder).i1)) - seqan::length(search)))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;

				case SSS_EXACT: // Only accept exact matches
				default:
					if (seqan::length(seqan::value(set, seqan::position(string_finder).i1)) == seqan::length(search))
					{
						container.insert(container.end(), std::basic_string<charT>(seqan::toCString(set[seqan::position(string_finder).i1])));
						found_indices.push_back(seqan::position(string_finder).i1);
						count++;
					}
					break;
			}
		}
	}
	return count;
}





// Check if the set contains a string, and if so, return the index of the string in the set (otherwise return -1)
template <typename charT>
int SpSearchStringSet<charT>::index_of(std::basic_string<charT>& string, SSS_MATCH_STRICTNESS strictness)
{
	seqan::String<charT> search(string.c_str());

	seqan::Finder<seqan::Index<seqan::StringSet<seqan::String<charT>>>> string_finder;

	seqan::setHaystack(string_finder, *index);

	while (seqan::find(string_finder, search))
	{
		switch (strictness)
		{
			case SSS_SUPER_STRING: // Accept any superstring of the search string
				return seqan::position(string_finder).i1;
				break;

			case SSS_NOT_EXACT: // Accept any superstring of the search string that is not equivalent to the search string
				if (!(seqan::length(seqan::value(set, seqan::position(string_finder).i1)) == seqan::length(search)))
					return seqan::position(string_finder).i1;
				break;

			case SSS_STARTS_WITH: // Accept superstrings where search string starts at character index 0
				if (seqan::position(string_finder).i2 == 0)
					return seqan::position(string_finder).i1;
				break;

			case SSS_ENDS_WITH: // Accept superstrings where search string is at the end of the superstring
				if (seqan::position(string_finder).i2 == (seqan::length(seqan::value(set, seqan::position(string_finder).i1)) - seqan::length(search)) )
					return seqan::position(string_finder).i1;
				break;

			case SSS_EXACT: // Only accept exact matches
			default:
				//                                 i2 = starting character position in the matching string.                   i1 = index in the "haystack" (set)
				if (seqan::length(seqan::value(set, seqan::position(string_finder).i1)) == seqan::length(search))
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
	return (index_of(string, strictness) != -1);
}


// Returns the string at the specified index in the set, or an empty string (with error code set) if invalid index was specified
template <typename charT>
const charT* SpSearchStringSet<charT>::get(unsigned int index)
{
	static const charT NOT_FOUND = (charT)0; // Return pointer to this empty string if no result was found

	if (index >= seqan::length(set)) // Specified index is out of bounds
	{
		SetLastError(ERROR_RANGE_NOT_FOUND);
		return &NOT_FOUND;
	}

	SetLastError(ERROR_SUCCESS);
	return (const charT*)(seqan::toCString(set[index]));
}


// Returns the number of strings in the set
template <typename charT>
inline size_t SpSearchStringSet<charT>::size()
{
	return seqan::length(set);
}


// (Re-)Initialize the set index structure
template <typename charT>
inline void SpSearchStringSet<charT>::initialize_index()
{
	if (index != NULL)
	{
		delete index;
		index = NULL;
	}
	index = new seqan::Index<seqan::StringSet<seqan::String<charT>>>(set);
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
int SpSearchStringSet<charT>::append(const charT* new_string)
{
	if (new_string == NULL)
		return ERROR_INVALID_ADDRESS;

	return append(std::basic_string<charT>(new_string));
}

// Overload of SpSearchStringSet::append(std::basic_string<charT>&)
template <typename charT>
int SpSearchStringSet<charT>::append(charT* new_string)
{
	if (new_string == NULL)
		return ERROR_INVALID_ADDRESS;

	return append(std::basic_string<charT>(new_string));
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









#endif // _SP_SEARCH_STRING_SET_H_