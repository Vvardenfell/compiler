#ifndef CHARACTER_CLASSIFICATION_H
#define CHARACTER_CLASSIFICATION_H

inline bool is_alpha(char symbol) {
    return symbol >= 'a' && symbol <= 'z' || symbol >= 'A' && symbol <= 'Z';
}

inline bool is_digit(char symbol) {
    return symbol >= '0' && symbol <= '9';
}

inline bool is_alnum(char symbol) {
    return is_alpha(symbol) || is_digit(symbol);
}

#endif // CHARACTER_CLASSIFICATION_H
