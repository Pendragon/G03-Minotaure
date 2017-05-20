// Name        : BinaryInput.cpp
// Description : Gestion d'une valeur binaire
// Author      : (c) Arthur & Jean-Paul GERST
// Date        : 2017

#ifndef __BINARYINPUT_H__
#define __BINARYINPUT_H__


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*
 * Binary Input est une classe permettant de gerer une entree digitale
 *
 * Chaque changement d'état est detecté. Il est possible de gerer de veritables entrees, ou 
 * des entress virtuelles qui ne sont pas associées à des IO. 
 */
class BinaryInput {
private:
	bool real_port = true;
	int previous;
	int port;
	
public:
	int raised;
	int dropped;
	int current;
  bool changed = false;

	BinaryInput(void);
	BinaryInput(int _port = -1, int mode = INPUT);

	int set(int new_value);    
	int read(bool inverse = false);
};

#endif /* __BINARYINPUT_H__ */
