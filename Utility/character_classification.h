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

inline bool is_space(char symbol) {
    switch(symbol) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
    case '\f': return true;
    default: return false;
    }
}

#endif // CHARACTER_CLASSIFICATION_H
