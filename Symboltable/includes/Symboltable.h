#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "../../Utility/container/string.h"
#include "../../Utility/container/unordered_map.h"
#include "../../Utility/container/binary_graveyard.h"
#include <functional>
#include <cstring>

class Symboltable {
public:

	typedef Information* key_type;

private:

	class StringHash {
		private:
			std::hash<String> hash;

		public:
			/*
			 * Calculates a hash with the SDBM hash function algorithm
			 */
			std::size_t operator()(const String* string) const {
				return this->hash(*string);
			}
	};

	class StringCompare {
		public:
			bool operator() (const String* left, const String* right) const {
				return *left == *right;
			}
	};

	UnorderedMap<String*, Information*, Symboltable::StringHash, Symboltable::StringCompare> map;
	BinaryGraveyard<String> keys;
	BinaryGraveyard<Information> values;
	
public:

	/*
	 * Inserts a given lexem to the symbol table and returns the key of it.
	 *
	 * @param lexem the lexem to be inserted
	 * @param token_type the TokenType to be associated with the lexem
	 * @return returns the key to the inserted lexem
	 */
	key_type insert(const String& lexem, TokenType token_type = TokenType::IDENTIFIER) {
		UnorderedMap<String*, Information*>::iterator iterator = this->map.find(&lexem);

		if (iterator == this->map.end()) {
			String* key = &keys.bury(lexem);
			Information* value = &values.bury(Information(key, token_type));

			return (*this->map.force_insert(key, value, iterator)).second;
		}

		return (*iterator).second;
	}

	Information* lookup(key_type key) const {
		return key;
	}
};

#endif /* SYMBOLTABLE_H */
