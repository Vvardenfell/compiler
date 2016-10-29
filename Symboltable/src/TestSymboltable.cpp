#include "../includes/Symboltable.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv) {

	Symboltable* symboltable;

	symboltable = new Symboltable();

	Key* key = symboltable->insert("Hallo");
	Key* key2 = symboltable->insert("Hallo");
	Key* key3 = symboltable->insert("too");
	Key* key4 = symboltable->insert("top");
	Key* key5 = symboltable->insert("tor");
	Key* key6 = symboltable->insert("tpp");
	Key* key7 = symboltable->insert("aaa");
	Key* key8 = symboltable->insert("]");
	Key* key9 = symboltable->insert("[");
	Key* key10 = symboltable->insert("?");
	Key* key11 = symboltable->insert("+");
	Key* key12 = symboltable->insert("-");
	Key* key13 = symboltable->insert("*");
	Key* key14 = symboltable->insert("-");
	Key* key16 = symboltable->insert(":");
	Key* key17 = symboltable->insert("<");
	Key* key18 = symboltable->insert(">");
	Key* key19 = symboltable->insert("=");
	Key* key20 = symboltable->insert(":=");
	Key* key21 = symboltable->insert("=:=");
	Key* key22 = symboltable->insert("!");
	Key* key23 = symboltable->insert("&&");
	Key* key24 = symboltable->insert(";");
	Key* key25 = symboltable->insert(")");
	Key* key26 = symboltable->insert("(");
	Key* key27 = symboltable->insert("{");
	Key* key28 = symboltable->insert("}");

	cout << symboltable->getSize() << endl;

	for (int i = 0; i < symboltable->getSize(); i++) {
		if (symboltable->getArray()[i]->getSize() > 0) {
			cout << symboltable->getArray()[i]->getSize() << endl;
		}
	}
}
