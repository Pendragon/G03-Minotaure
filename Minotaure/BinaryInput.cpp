// Name        : BinaryInput.cpp
// Description : Gestion d'une valeur binaire
// Author      : (c) Arthur & Jean-Paul GERST
// Date        : 2017

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include ".\BinaryInput.h"

/*
 * Binary Input est une classe permettant de gerer une entree digitale
 *
 * Chaque changement d'état est detecté. Il est possible de gerer de veritables entrees, ou 
 * des entress virtuelles qui ne sont pas associées à des IO. 
 */
BinaryInput::BinaryInput(int _port = -1, int mode = INPUT) 
{
	if (_port == -1) // Simple stockage de donnée binaire, pas de lecteur hardware
		real_port = false;
	else {
		pinMode(_port, mode);
		port = _port;
		real_port = true;      
	}
	previous = LOW;
	current = LOW;
	raised = LOW;
	dropped = LOW;
}

int BinaryInput::set(int new_value) 
{
	previous = current;
	current = new_value;
	changed = false;

	if (current) {
		if (previous == LOW) {
			raised = HIGH;
	  		changed = true;
		}
		else 
	  		raised = LOW;
	} else {
		if (previous == HIGH) {
			changed = true;
			dropped = HIGH;
		}
		else
	  		dropped = LOW;
	}
}

int BinaryInput::read(bool inverse = false) 
{
	// Ne lire les valeurs hardware que si c'est un port physique
	if (real_port) {
		if (inverse)
			set(!digitalRead(port));
		else
			set(digitalRead(port));
	}
}


